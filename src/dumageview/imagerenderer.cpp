#include "dumageview/imagerenderer.h"

#include "dumageview/assert.h"
#include "dumageview/conv_vec.h"
#include "dumageview/imagewidget.h"
#include "dumageview/log.h"
#include "dumageview/math.h"
#include "dumageview/qtutil.h"
#include "dumageview/renderview_inl.h"
#include "dumageview/scopeguard.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_precision.hpp>

#include <QOpenGLContext>
#include <QOpenGLFunctions_2_1>
#include <QPoint>
#include <QRect>
#include <QRectF>
#include <QSize>

#include <algorithm>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace dumageview::imagerenderer::detail
{
  namespace
  {
    constexpr double zoomFactor{1.1};

    auto contextGuard(ImageWidget& widget)
    {
      widget.makeCurrent();
      return ScopeGuard{[&]{ widget.doneCurrent(); }};
    }
  }

  //
  // ImageRenderer member functions
  //

  ImageRenderer::ImageRenderer(
    ImageWidget& widget,
    QMetaObject::Connection&& contextDestroyConnection)
  :
    _widget(widget),
    _contextDestroyConnection(std::move(contextDestroyConnection))
  {
    auto* context = QOpenGLContext::currentContext();
    if (!context)
    {
      throw Error("Null context returned.");
    }

    DUMAGEVIEW_LOG_DEBUG(
      "Using {} version {}.{} ({} Profile)",
      context->isOpenGLES() ? "OpenGL ES" : "OpenGL",
      context->format().majorVersion(),
      context->format().minorVersion(),
      [&] {
        switch (context->format().profile())
        {
          case QSurfaceFormat::CoreProfile:
            return "Core";
          case QSurfaceFormat::CompatibilityProfile:
            return "Compatibility";
          default:
            return "No";
        }
      }()
    );

    _gl = context->versionFunctions<QOpenGLFunctions_2_1>();
    if (!_gl)
    {
      throw Error("OpenGL functions "
                  "(compatible with version 2.1) not available.");
    }

    _gl->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    _gl->glEnable(GL_BLEND);
    _gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  ImageRenderer::~ImageRenderer()
  {
    DUMAGEVIEW_LOG_TRACE("this = {}", this);
    removeImage();
    qtutil::disconnect(_contextDestroyConnection);
  }

  //
  // Image management
  //

  void ImageRenderer::setImage(QImage image_)
  {
    auto guard = contextGuard(_widget);

    // TODO: Test against GL_MAX_TEXTURE_SIZE
    _imageState.reset(new ImageState{
        image_, QOpenGLTexture{image_}, ZoomToFitView{}});

    auto& tex = _imageState->texture;
    tex.setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    tex.setMagnificationFilter(QOpenGLTexture::Linear);

    tex.setBorderColor(0.0f, 0.0f, 0.0f, 1.0f);
    tex.setWrapMode(QOpenGLTexture::ClampToBorder);

    _gl->glEnable(GL_TEXTURE_2D);

  }

  void ImageRenderer::removeImage()
  {
    auto guard = contextGuard(_widget);
    _imageState.reset();
    _gl->glDisable(GL_TEXTURE_2D);
  }

  //
  // Convenience accessor-like functions
  //

  glm::dvec2 ImageRenderer::imageSize() const
  {
    DUMAGEVIEW_ASSERT(_imageState);
    return conv::dvec(_imageState->image.size());
  }

  glm::dvec2 ImageRenderer::screenSize() const
  {
    return conv::dvec(_widget.size());
  }

  SizeInfo ImageRenderer::sizeInfo() const
  {
    DUMAGEVIEW_ASSERT(_imageState);
    return {imageSize(), screenSize()};
  }

  ViewMod<View> ImageRenderer::viewMod() const
  {
    DUMAGEVIEW_ASSERT(_imageState);
    return ViewMod(_imageState->view, sizeInfo());
  }

  //
  // Input events
  //

  void ImageRenderer::move(QPoint const& dPos)
  {
    if (!_imageState)
      return;

    auto vm = viewMod();
    vm.modify([&](auto& v) { v.position += conv::dvec(dPos); });
    _imageState->view = vm.normalize().view();
  }

  void ImageRenderer::zoomRel(int steps, QPointF const& pos)
  {
    if (!_imageState)
      return;

    auto vm = viewMod().reified();
    vm.setZoom(vm.view().scale * math::ipow(zoomFactor, steps),
               conv::dvec(pos));

    _imageState->view = vm.view();
  }

  void ImageRenderer::zoomAbs(double scale, QPointF const& pos)
  {
    if (!_imageState)
      return;

    auto vm = viewMod().reified();
    vm.setZoom(scale, conv::dvec(pos));

    _imageState->view = vm.view();
  }

  void ImageRenderer::zoomToFit()
  {
    if (!_imageState)
      return;

    _imageState->view = ZoomToFitView{};
  }

  //
  // Widget GL events
  //

  void ImageRenderer::resize(int w, int h)
  {
    DUMAGEVIEW_ASSERT(w == _widget.width());
    DUMAGEVIEW_ASSERT(h == _widget.height());

    _gl->glViewport(0, 0, w, h);

    _gl->glMatrixMode(GL_PROJECTION);
    _gl->glLoadIdentity();
    _gl->glOrtho(0.0, w, h, 0.0, -1.0, 1.0);

    _gl->glMatrixMode(GL_MODELVIEW);

    if (_imageState)
      _imageState->view = viewMod().normalize().view();
  }

  void ImageRenderer::draw()
  {
    _gl->glClear(GL_COLOR_BUFFER_BIT);

    if (!_imageState)
      return;

    auto transform = viewMod().imageToScreenMatrix();
    _gl->glLoadMatrixd(&transform[0][0]);

    _gl->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    _imageState->texture.bind();

    glm::dvec2 rectSize = imageSize();
    _gl->glBegin(GL_POLYGON);

    _gl->glTexCoord2d(0.0, 0.0);
    _gl->glVertex2d(0.0, 0.0);

    _gl->glTexCoord2d(1.0, 0.0);
    _gl->glVertex2d(rectSize.x, 0.0);

    _gl->glTexCoord2d(1.0, 1.0);
    _gl->glVertex2d(rectSize.x, rectSize.y);

    _gl->glTexCoord2d(0.0, 1.0);
    _gl->glVertex2d(0.0, rectSize.y);

    _gl->glEnd();
  }
}

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

namespace dumageview::imagerenderer::detail {
  namespace {
    constexpr double zoomFactor{1.1};

    auto contextGuard(ImageWidget& widget) {
      widget.makeCurrent();
      return ScopeGuard{[&] {
        widget.doneCurrent();
      }};
    }
  }

  //
  // ImageRenderer member functions
  //

  ImageRenderer::ImageRenderer(
    ImageWidget& widget,
    QMetaObject::Connection&& contextDestroyConnection)
      : widget_(widget),
        contextDestroyConnection_(std::move(contextDestroyConnection)) {
    auto* context = QOpenGLContext::currentContext();
    if (!context) {
      throw Error("Null context returned.");
    }

    DUMAGEVIEW_LOG_DEBUG("Using {} version {}.{} ({} Profile)",
                         context->isOpenGLES() ? "OpenGL ES" : "OpenGL",
                         context->format().majorVersion(),
                         context->format().minorVersion(),
                         [&] {
                           switch (context->format().profile()) {
                             case QSurfaceFormat::CoreProfile:
                               return "Core";
                             case QSurfaceFormat::CompatibilityProfile:
                               return "Compatibility";
                             default:
                               return "No";
                           }
                         }());

    gl_ = context->versionFunctions<QOpenGLFunctions_2_1>();
    if (!gl_) {
      throw Error(
        "OpenGL functions "
        "(compatible with version 2.1) not available.");
    }

    gl_->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    gl_->glEnable(GL_BLEND);
    gl_->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  ImageRenderer::~ImageRenderer() {
    DUMAGEVIEW_LOG_TRACE("this = {}", this);
    removeImage();
    qtutil::disconnect(contextDestroyConnection_);
  }

  //
  // Image management
  //

  void ImageRenderer::setImage(QImage image) {
    auto guard = contextGuard(widget_);

    // TODO: Test against GL_MAX_TEXTURE_SIZE
    imageState_.reset(
      new ImageState{image, QOpenGLTexture{image}, ZoomToFitView{}});

    auto& tex = imageState_->texture;
    tex.setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    tex.setMagnificationFilter(QOpenGLTexture::Linear);

    tex.setBorderColor(0.0f, 0.0f, 0.0f, 1.0f);
    tex.setWrapMode(QOpenGLTexture::ClampToBorder);

    gl_->glEnable(GL_TEXTURE_2D);
  }

  void ImageRenderer::removeImage() {
    auto guard = contextGuard(widget_);
    imageState_.reset();
    gl_->glDisable(GL_TEXTURE_2D);
  }

  //
  // Convenience accessor-like functions
  //

  glm::dvec2 ImageRenderer::getImageSize() const {
    DUMAGEVIEW_ASSERT(imageState_);
    return conv::dvec(imageState_->image.size());
  }

  glm::dvec2 ImageRenderer::getScreenSize() const {
    return conv::dvec(widget_.size());
  }

  SizeInfo ImageRenderer::getSizeInfo() const {
    DUMAGEVIEW_ASSERT(imageState_);
    return {getImageSize(), getScreenSize()};
  }

  ViewMod<View> ImageRenderer::getViewMod() const {
    DUMAGEVIEW_ASSERT(imageState_);
    return ViewMod(imageState_->view, getSizeInfo());
  }

  //
  // Input events
  //

  void ImageRenderer::move(QPoint const& dPos) {
    if (!imageState_) return;

    auto vm = getViewMod();
    vm.modify([&](auto& v) {
      v.position += conv::dvec(dPos);
    });
    imageState_->view = vm.normalize().getView();
  }

  void ImageRenderer::zoomRel(int steps, QPointF const& pos) {
    if (!imageState_) return;

    auto vm = getViewMod().reified();
    vm.setZoom(vm.getView().scale * math::ipow(zoomFactor, steps),
               conv::dvec(pos));

    imageState_->view = vm.getView();
  }

  void ImageRenderer::zoomAbs(double scale, QPointF const& pos) {
    if (!imageState_) return;

    auto vm = getViewMod().reified();
    vm.setZoom(scale, conv::dvec(pos));

    imageState_->view = vm.getView();
  }

  void ImageRenderer::zoomToFit() {
    if (!imageState_) return;

    imageState_->view = ZoomToFitView{};
  }

  //
  // Widget GL events
  //

  void ImageRenderer::resize(int w, int h) {
    DUMAGEVIEW_ASSERT(w == widget_.width());
    DUMAGEVIEW_ASSERT(h == widget_.height());

    gl_->glViewport(0, 0, w, h);

    gl_->glMatrixMode(GL_PROJECTION);
    gl_->glLoadIdentity();
    gl_->glOrtho(0.0, w, h, 0.0, -1.0, 1.0);

    gl_->glMatrixMode(GL_MODELVIEW);

    if (imageState_) imageState_->view = getViewMod().normalize().getView();
  }

  void ImageRenderer::draw() {
    gl_->glClear(GL_COLOR_BUFFER_BIT);

    if (!imageState_) return;

    auto transform = getViewMod().imageToScreenMatrix();
    gl_->glLoadMatrixd(&transform[0][0]);

    gl_->glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    imageState_->texture.bind();

    glm::dvec2 rectSize = getImageSize();
    gl_->glBegin(GL_POLYGON);

    gl_->glTexCoord2d(0.0, 0.0);
    gl_->glVertex2d(0.0, 0.0);

    gl_->glTexCoord2d(1.0, 0.0);
    gl_->glVertex2d(rectSize.x, 0.0);

    gl_->glTexCoord2d(1.0, 1.0);
    gl_->glVertex2d(rectSize.x, rectSize.y);

    gl_->glTexCoord2d(0.0, 1.0);
    gl_->glVertex2d(0.0, rectSize.y);

    gl_->glEnd();
  }
}

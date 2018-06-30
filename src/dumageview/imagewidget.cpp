#include "dumageview/imagewidget.h"

#include "dumageview/application.h"
#include "dumageview/assert.h"
#include "dumageview/conv_vec.h"
#include "dumageview/log.h"
#include "dumageview/math.h"
#include "dumageview/qtutil.h"

#include <glm/glm.hpp>

#include <QContextMenuEvent>
#include <QDesktopWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QWheelEvent>

namespace dumageview
{
  namespace
  {
    constexpr int wheelStepSize = 120;
  }

  ImageWidget::ImageWidget(ActionSet& actions_, QWidget* parent_)
  :
    Base(parent_),
    _actions(actions_)
  {
    setUpdateBehavior(NoPartialUpdate);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  }

  ImageWidget::~ImageWidget()
  {
    DUMAGEVIEW_LOG_TRACE("this={}", this);
  }

  //
  // Widget properties
  //

  QSize ImageWidget::sizeHint() const
  {
    QRect screenRect = QApplication::desktop()->availableGeometry(this);
    auto partialScreen = screenRect.size() * 3 / 5;

    if (!_image)
      return partialScreen;

    auto longDim =
      (math::aspectRatio(_image->size()) > math::aspectRatio(partialScreen))
        ? math::getX
        : math::getY;

    auto longConv =
      [&](auto const& v) { return longDim(conv::dvec(v)); };

    double scale = longConv(partialScreen) / longConv(_image->size());
    return _image->size() * scale;
  }

  QSize ImageWidget::minimumSizeHint() const
  {
    // arbitrary still-usable size
    return {200, 200};
  }

  //
  // Image slots
  //

  void ImageWidget::resetImage(QImage const& image)
  {
    if (image.isNull())
    {
      imageLoadFailed("Cannot load null image");
      return;
    }

    _image = image;
    activateZoomToFit();

    if (_renderer)
      _renderer->setImage(image);

    updateGeometry();
    update();
  }

  void ImageWidget::removeImage()
  {
    _image.reset();
    deactivateZoomToFit();

    if (_renderer)
      _renderer->removeImage();

    updateGeometry();
    update();
  }

  //
  // Zooming
  //

  void ImageWidget::zoomIn()
  {
    zoom(1, conv::qpointf(size()) * 0.5);
  }

  void ImageWidget::zoomOut()
  {
    zoom(-1, conv::qpointf(size()) * 0.5);
  }

  void ImageWidget::zoom(int steps, QPointF const& center)
  {
    if (!_renderer)
      return;

    deactivateZoomToFit();
    _renderer->zoomRel(steps, center);

    update();
  }

  void ImageWidget::zoomOriginal()
  {
    if (!_renderer)
      return;

    deactivateZoomToFit();
    _renderer->zoomAbs(1.0, conv::qpointf(size()) * 0.5);

    update();
  }

  void ImageWidget::zoomToFit()
  {
    if (!_renderer)
      return;

    activateZoomToFit();
    _renderer->zoomToFit();

    update();
  }

  void ImageWidget::activateZoomToFit()
  {
    _actions.zoomToFit.setEnabled(false);
  }

  void ImageWidget::deactivateZoomToFit()
  {
    _actions.zoomToFit.setEnabled(true);
  }

  bool ImageWidget::zoomToFitActive() const
  {
    // The state of zoom-to-fit is the opposite of the action enable state,
    return !_actions.zoomToFit.isEnabled();
  }

  //
  // GL handlers
  //

  void ImageWidget::initializeGL()
  {
    DUMAGEVIEW_ASSERT(!_renderer);

    try
    {
      auto connection = qtutil::connect(
        context(), &QOpenGLContext::aboutToBeDestroyed,
        this, &ImageWidget::cleanupGL);

      _renderer = std::make_unique<ImageRenderer>(*this, std::move(connection));

      if (_image)
        _renderer->setImage(*_image);
    }
    catch (imagerenderer::Error const& error)
    {
      log::error("Could not initialize graphics: {}", error.what());
      Application::singletonInstance().exit(application::exitcode::glInitError);
    }
  }

  void ImageWidget::resizeGL(int w, int h)
  {
    DUMAGEVIEW_ASSERT(w == width());
    DUMAGEVIEW_ASSERT(h == height());
    DUMAGEVIEW_ASSERT(_renderer);

    _renderer->resize(w, h);
  }

  void ImageWidget::paintGL()
  {
    DUMAGEVIEW_ASSERT(_renderer);
    _renderer->draw();
  }

  void ImageWidget::cleanupGL()
  {
    DUMAGEVIEW_ASSERT(_renderer);
    DUMAGEVIEW_LOG_TRACE("this={}, _renderer={}", this, _renderer.get());
    _renderer.reset();
  }

  //
  // Event handlers
  //

  void ImageWidget::mouseDoubleClickEvent(QMouseEvent* evt)
  {
    DUMAGEVIEW_ASSERT(evt);

    // Test environment has the following double-click sequence:
    // - mousePressEvent
    // - mouseReleaseEvent
    // - mouseDoubleClickEvent
    // - mouseReleaseEvent
    //
    // However, looking on the internet, this does not seem to be universal.

    switch (evt->button())
    {
      case Qt::LeftButton:
        _lastMousePos = evt->pos();
        _actions.fullScreen.trigger();
        break;

      default:
        Base::mouseDoubleClickEvent(evt);
        break;
    }
  }

  void ImageWidget::mousePressEvent(QMouseEvent* evt)
  {
    DUMAGEVIEW_ASSERT(evt);

    switch (evt->button())
    {
      case Qt::LeftButton:
        _lastMousePos = evt->pos();
        break;

      case Qt::BackButton:
        _actions.prevImage.trigger();
        break;

      case Qt::ForwardButton:
        _actions.nextImage.trigger();
        break;

      default:
        Base::mousePressEvent(evt);
        break;
    }
  }

  void ImageWidget::mouseReleaseEvent(QMouseEvent* evt)
  {
    DUMAGEVIEW_ASSERT(evt);

    switch (evt->button())
    {
      case Qt::LeftButton:
        _lastMousePos.reset();
        break;

      default:
        Base::mouseReleaseEvent(evt);
        break;
    }
  }

  void ImageWidget::mouseMoveEvent(QMouseEvent* evt)
  {
    DUMAGEVIEW_ASSERT(evt);

    if (evt->buttons() & Qt::LeftButton)
    {
      if (_renderer && _lastMousePos)
      {
        _renderer->move(evt->pos() - *_lastMousePos);
        update();
      }

      _lastMousePos = evt->pos();
    }
    else
    {
      Base::mouseMoveEvent(evt);
    }
  }

  void ImageWidget::wheelEvent(QWheelEvent* evt)
  {
    DUMAGEVIEW_ASSERT(evt);
    int steps = evt->angleDelta().y() / wheelStepSize;
    zoom(steps, evt->posF());
  }

  void ImageWidget::keyPressEvent(QKeyEvent* evt)
  {
    Base::keyPressEvent(evt);
  }

  void ImageWidget::contextMenuEvent(QContextMenuEvent* evt)
  {
    DUMAGEVIEW_ASSERT(evt);

    contextMenuWanted(evt->globalPos());
  }
}

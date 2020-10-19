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

namespace dumageview {
  namespace {
    constexpr int wheelStepSize = 120;
  }

  ImageWidget::ImageWidget(ActionSet& actions, QWidget* parent)
      : Base(parent), actions_(actions) {
    setUpdateBehavior(NoPartialUpdate);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  }

  ImageWidget::~ImageWidget() {
    DUMAGEVIEW_LOG_TRACE("this={}", this);
  }

  //
  // Widget properties
  //

  QSize ImageWidget::sizeHint() const {
    QRect screenRect = QApplication::desktop()->availableGeometry(this);
    auto partialScreen = screenRect.size() * 3 / 5;

    if (!image_) return partialScreen;

    auto longDim =
      (math::aspectRatio(image_->size()) > math::aspectRatio(partialScreen))
        ? math::getX
        : math::getY;

    auto longConv = [&](auto const& v) {
      return longDim(conv::dvec(v));
    };

    double scale = longConv(partialScreen) / longConv(image_->size());
    return image_->size() * scale;
  }

  QSize ImageWidget::minimumSizeHint() const {
    // arbitrary still-usable size
    return {200, 200};
  }

  //
  // Image slots
  //

  void ImageWidget::resetImage(QImage const& image) {
    if (image.isNull()) {
      imageLoadFailed("Cannot load null image");
      return;
    }

    image_ = image;
    activateZoomToFit();

    if (renderer_) renderer_->setImage(image);

    updateGeometry();
    update();
  }

  void ImageWidget::removeImage() {
    image_.reset();
    deactivateZoomToFit();

    if (renderer_) renderer_->removeImage();

    updateGeometry();
    update();
  }

  //
  // Zooming
  //

  void ImageWidget::zoomIn() {
    zoom(1, conv::qpointf(size()) * 0.5);
  }

  void ImageWidget::zoomOut() {
    zoom(-1, conv::qpointf(size()) * 0.5);
  }

  void ImageWidget::zoom(int steps, QPointF const& center) {
    if (!renderer_) return;

    deactivateZoomToFit();
    renderer_->zoomRel(steps, center);

    update();
  }

  void ImageWidget::zoomOriginal() {
    if (!renderer_) return;

    deactivateZoomToFit();
    renderer_->zoomAbs(1.0, conv::qpointf(size()) * 0.5);

    update();
  }

  void ImageWidget::zoomToFit() {
    if (!renderer_) return;

    activateZoomToFit();
    renderer_->zoomToFit();

    update();
  }

  void ImageWidget::activateZoomToFit() {
    actions_.zoomToFit.setEnabled(false);
  }

  void ImageWidget::deactivateZoomToFit() {
    actions_.zoomToFit.setEnabled(true);
  }

  bool ImageWidget::zoomToFitActive() const {
    // The state of zoom-to-fit is the opposite of the action enable state,
    return !actions_.zoomToFit.isEnabled();
  }

  //
  // GL handlers
  //

  void ImageWidget::initializeGL() {
    DUMAGEVIEW_ASSERT(!renderer_);

    try {
      auto connection = qtutil::connect(context(),
                                        &QOpenGLContext::aboutToBeDestroyed,
                                        this,
                                        &ImageWidget::cleanupGL);

      renderer_ = std::make_unique<ImageRenderer>(*this, std::move(connection));

      if (image_) renderer_->setImage(*image_);
    } catch (imagerenderer::Error const& error) {
      log::error("Could not initialize graphics: {}", error.what());
      Application::getSingletonInstance().exit(
        application::exitcode::glInitError);
    }
  }

  void ImageWidget::resizeGL(int w, int h) {
    DUMAGEVIEW_ASSERT(w == width());
    DUMAGEVIEW_ASSERT(h == height());
    DUMAGEVIEW_ASSERT(renderer_);

    renderer_->resize(w, h);
  }

  void ImageWidget::paintGL() {
    DUMAGEVIEW_ASSERT(renderer_);
    renderer_->draw();
  }

  void ImageWidget::cleanupGL() {
    DUMAGEVIEW_ASSERT(renderer_);
    DUMAGEVIEW_LOG_TRACE("this={}, renderer_={}", this, renderer_.get());
    renderer_.reset();
  }

  //
  // Event handlers
  //

  void ImageWidget::mouseDoubleClickEvent(QMouseEvent* evt) {
    DUMAGEVIEW_ASSERT(evt);

    // Test environment has the following double-click sequence:
    // - mousePressEvent
    // - mouseReleaseEvent
    // - mouseDoubleClickEvent
    // - mouseReleaseEvent
    //
    // However, looking on the internet, this does not seem to be universal.

    switch (evt->button()) {
      case Qt::LeftButton:
        lastMousePos_ = evt->pos();
        actions_.fullScreen.trigger();
        break;

      default:
        Base::mouseDoubleClickEvent(evt);
        break;
    }
  }

  void ImageWidget::mousePressEvent(QMouseEvent* evt) {
    DUMAGEVIEW_ASSERT(evt);

    switch (evt->button()) {
      case Qt::LeftButton:
        lastMousePos_ = evt->pos();
        break;

      case Qt::BackButton:
        actions_.prevImage.trigger();
        break;

      case Qt::ForwardButton:
        actions_.nextImage.trigger();
        break;

      default:
        Base::mousePressEvent(evt);
        break;
    }
  }

  void ImageWidget::mouseReleaseEvent(QMouseEvent* evt) {
    DUMAGEVIEW_ASSERT(evt);

    switch (evt->button()) {
      case Qt::LeftButton:
        lastMousePos_.reset();
        break;

      default:
        Base::mouseReleaseEvent(evt);
        break;
    }
  }

  void ImageWidget::mouseMoveEvent(QMouseEvent* evt) {
    DUMAGEVIEW_ASSERT(evt);

    if (evt->buttons() & Qt::LeftButton) {
      if (renderer_ && lastMousePos_) {
        renderer_->move(evt->pos() - *lastMousePos_);
        update();
      }

      lastMousePos_ = evt->pos();
    } else {
      Base::mouseMoveEvent(evt);
    }
  }

  void ImageWidget::wheelEvent(QWheelEvent* evt) {
    DUMAGEVIEW_ASSERT(evt);
    int steps = evt->angleDelta().y() / wheelStepSize;
    zoom(steps, evt->posF());
  }

  void ImageWidget::keyPressEvent(QKeyEvent* evt) {
    Base::keyPressEvent(evt);
  }

  void ImageWidget::contextMenuEvent(QContextMenuEvent* evt) {
    DUMAGEVIEW_ASSERT(evt);

    contextMenuWanted(evt->globalPos());
  }
}

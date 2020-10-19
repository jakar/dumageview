#ifndef DUMAGEVIEW_IMAGEWIDGET_H_
#define DUMAGEVIEW_IMAGEWIDGET_H_

#include "dumageview/actionset.h"
#include "dumageview/imagerenderer.h"

#include <glm/fwd.hpp>

#include <QImage>
#include <QOpenGLWidget>
#include <QPoint>
#include <QPointF>
#include <QWidget>

#include <memory>

namespace dumageview {
  class ImageWidget : public QOpenGLWidget {
    Q_OBJECT;

   public:
    explicit ImageWidget(ActionSet& actions, QWidget* parent = nullptr);

    virtual ~ImageWidget();

    void resetImage(QImage const& image);

    void removeImage();

    void zoomToFit();

    void zoomOriginal();

    void zoomIn();

    void zoomOut();

    QSize sizeHint() const override;

    QSize minimumSizeHint() const override;

   Q_SIGNALS:
    void imageLoadFailed(QString const& message);

    void contextMenuWanted(QPoint const& globalPos);

   protected:
    //
    // GL handlers
    //

    void initializeGL() override;

    void resizeGL(int width, int height) override;

    void paintGL() override;

    void cleanupGL();

    //
    // Event handlers
    //

    void mouseDoubleClickEvent(QMouseEvent* evt) override;

    void mousePressEvent(QMouseEvent* evt) override;

    void mouseReleaseEvent(QMouseEvent* evt) override;

    void mouseMoveEvent(QMouseEvent* evt) override;

    void wheelEvent(QWheelEvent* evt) override;

    void keyPressEvent(QKeyEvent* evt) override;

    void contextMenuEvent(QContextMenuEvent* evt) override;

   private:
    using Base = QOpenGLWidget;

    void zoom(int steps, QPointF const& center);

    bool zoomToFitActive() const;
    void activateZoomToFit();
    void deactivateZoomToFit();

    //
    // Private data
    //

    ActionSet& actions_;
    std::optional<QImage> image_;
    std::unique_ptr<ImageRenderer> renderer_;

    std::optional<QPoint> lastMousePos_;
  };
}

#endif  // DUMAGEVIEW_IMAGEWIDGET_H_

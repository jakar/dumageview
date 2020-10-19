#ifndef DUMAGEVIEW_IMAGERENDERER_H_
#define DUMAGEVIEW_IMAGERENDERER_H_

#include "dumageview/math_fwd.h"
#include "dumageview/renderview.h"

#include <glm/glm.hpp>

#include <QImage>
#include <QObject>
#include <QOpenGLTexture>
#include <QPoint>
#include <QSize>

#include <memory>
#include <stdexcept>
#include <variant>

class QOpenGLFunctions_2_1;
namespace dumageview {
  class ImageWidget;
}

namespace dumageview::imagerenderer::detail {
  using renderview::ManualView;
  using renderview::View;
  using renderview::ZoomToFitView;

  using renderview::SizeInfo;
  using renderview::ViewMod;

  class Error : public virtual std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  struct ImageState {
    QImage image;
    QOpenGLTexture texture;
    View view;
  };

  /**
   * Maintains GL-related things.
   * Only alive when GL is initialized.
   */
  class ImageRenderer {
   public:
    ImageRenderer(ImageWidget& widget,
                  QMetaObject::Connection&& contextDestroyConnection);

    ~ImageRenderer();

    void setImage(QImage image);

    void removeImage();

    void move(QPoint const& dPos);

    void zoomRel(int steps, QPointF const& pos);

    void zoomAbs(double scale, QPointF const& pos);

    void zoomToFit();

    void resize(int w, int h);

    void draw();

   private:
    ImageRenderer(ImageRenderer const&) = delete;
    ImageRenderer& operator=(ImageRenderer const&) = delete;

    glm::dvec2 getImageSize() const;
    glm::dvec2 getScreenSize() const;

    SizeInfo getSizeInfo() const;
    ViewMod<View> getViewMod() const;

    //
    // Private data
    //

    ImageWidget& widget_;

    QMetaObject::Connection contextDestroyConnection_;

    QOpenGLFunctions_2_1* gl_ = nullptr;

    std::unique_ptr<ImageState> imageState_;
  };
}

namespace dumageview {
  namespace imagerenderer {
    using detail::Error;
    using detail::ImageRenderer;
  }

  using imagerenderer::ImageRenderer;
}

#endif  // DUMAGEVIEW_IMAGERENDERER_H_

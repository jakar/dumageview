#ifndef DUMAGEVIEW_IMAGERENDERER_H_
#define DUMAGEVIEW_IMAGERENDERER_H_

#include "dumageview/math_fwd.h"
#include "dumageview/renderview.h"

#include <glm/glm.hpp>

#include <QImage>
#include <QOpenGLTexture>
#include <QPoint>
#include <QSize>
#include <QObject>

#include <memory>
#include <stdexcept>
#include <variant>

class QOpenGLFunctions_2_1;
namespace dumageview { class ImageWidget; }

namespace dumageview::imagerenderer::detail
{
  using renderview::View;
  using renderview::ZoomToFitView;
  using renderview::ManualView;

  using renderview::ViewMod;
  using renderview::SizeInfo;

  class Error : public virtual std::runtime_error
  {
    using std::runtime_error::runtime_error;
  };

  struct ImageState
  {
    QImage image;
    QOpenGLTexture texture;
    View view;
  };

  /**
   * Maintains GL-related things.
   * Only alive when GL is initialized.
   */
  class ImageRenderer
  {
   public:

    ImageRenderer(ImageWidget& widget,
                  QMetaObject::Connection&& contextDestroyConnection);

    ~ImageRenderer();

    void setImage(QImage image_);

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

    glm::dvec2 imageSize() const;
    glm::dvec2 screenSize() const;

    SizeInfo sizeInfo() const;
    ViewMod<View> viewMod() const;

    //
    // Private data
    //

    ImageWidget& _widget;

    QMetaObject::Connection _contextDestroyConnection;

    QOpenGLFunctions_2_1* _gl = nullptr;

    std::unique_ptr<ImageState> _imageState;
  };
}

namespace dumageview
{
  namespace imagerenderer
  {
    using detail::Error;
    using detail::ImageRenderer;
  }

  using imagerenderer::ImageRenderer;
}

#endif // DUMAGEVIEW_IMAGERENDERER_H_

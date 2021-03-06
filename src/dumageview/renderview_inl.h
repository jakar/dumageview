#ifndef DUMAGEVIEW_RENDERVIEW_INL_H_
#define DUMAGEVIEW_RENDERVIEW_INL_H_

#include "dumageview/renderview.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_precision.hpp>

#include <boost/hana.hpp>

namespace dumageview::renderview {
  //
  // BaseViewMod member functions
  //

  template<class D>
  template<class F>
  D& BaseViewMod<D>::modify(F&& func) {
    auto visitor =
      boost::hana::overload([](ZoomToFitView) {}, std::forward<F>(func));
    std::visit(visitor, view_);
    return dref();
  }

  template<class D>
  D& BaseViewMod<D>::normalize() {
    modify([this](auto& rv) {
      auto scaledImageSize = rv.scale * size_.image;
      ViewMod vm{rv, size_};

      for (auto const& dim : math::dimensions2D) {
        if (dim(scaledImageSize) < dim(size_.screen)) {
          rv = vm.center(dim).getView();
        } else {
          rv = vm.clamp(dim).getView();
        }
      }
    });

    return dref();
  }

  template<class D>
  ViewMod<ManualView> BaseViewMod<D>::reified() const {
    auto visitor = boost::hana::overload(
      [](ManualView const& v) {
        return v;
      },
      [&](ZoomToFitView) {
        auto [longDim, shortDim] =
          (math::aspectRatio(size_.image) > math::aspectRatio(size_.screen))
            ? std::pair{math::getX, math::getY}
            : std::pair{math::getY, math::getX};

        double scale = longDim(size_.screen) / longDim(size_.image);

        ManualView rv{scale, {0.0, 0.0}};
        ViewMod vm{rv, size_};
        return vm.center(shortDim).getView();
      });

    auto rv = std::visit(visitor, view_);
    return {rv, size_};
  }

  //
  // ViewMod<View> member functions
  //

  inline glm::dvec2 ViewMod<View>::imageToScreen(glm::dvec2 const& pos) const {
    return reified().imageToScreen(pos);
  }

  inline glm::dvec2 ViewMod<View>::screenToImage(glm::dvec2 const& pos) const {
    return reified().screenToImage(pos);
  }

  inline glm::dmat4 ViewMod<View>::imageToScreenMatrix() const {
    return reified().imageToScreenMatrix();
  }

  inline glm::dmat4 ViewMod<View>::screenToImageMatrix() const {
    return reified().screenToImageMatrix();
  }

  //
  // ViewMod<ManualView> member functions
  //

  inline auto ViewMod<ManualView>::center(math::Getter const& dim) -> ViewMod& {
    dim(getView().position) = dim(getSize().screen * 0.5)
                              - dim(getSize().image * getView().scale * 0.5);
    return *this;
  }

  inline auto ViewMod<ManualView>::clamp(math::Getter const& dim) -> ViewMod& {
    auto botRight = imageToScreen(getSize().image);

    if (dim(getView().position) > 0.0) {
      dim(getView().position) = 0.0;
    } else if (dim(botRight) < dim(getSize().screen)) {
      dim(getView().position) += dim(getSize().screen) - dim(botRight);
    }

    return *this;
  }

  inline auto ViewMod<ManualView>::setZoom(double scale,
                                           glm::dvec2 const& screenFixed) -> ViewMod& {
    // zoom around fixed point
    auto imageFixed = screenToImage(screenFixed);
    imageFixed = glm::clamp(imageFixed, glm::dvec2{0.0}, getSize().image);

    getView().scale = std::clamp(scale, minZoom, maxZoom);
    getView().position = screenFixed - getView().scale * imageFixed;

    normalize();
    return *this;
  }

  inline glm::dvec2 ViewMod<ManualView>::imageToScreen(
    glm::dvec2 const& imagePos) const {
    return getView().scale * imagePos + getView().position;
  }

  inline glm::dvec2 ViewMod<ManualView>::screenToImage(
    glm::dvec2 const& screenPos) const {
    return (1.0 / getView().scale) * (screenPos - getView().position);
  }

  inline glm::dmat4 ViewMod<ManualView>::imageToScreenMatrix() const {
    glm::dmat4 eye{1.0};
    glm::dvec2 scale{getView().scale};

    auto t = glm::translate(eye, glm::dvec3{getView().position, 0.0});
    auto s = glm::scale(eye, glm::dvec3{scale, 1.0});

    return t * s;
  }

  inline glm::dmat4 ViewMod<ManualView>::screenToImageMatrix() const {
    glm::dmat4 eye{1.0};
    glm::dvec2 invScale{1.0 / getView().scale};

    auto t = glm::translate(eye, glm::dvec3{-1.0 * getView().position, 0.0});
    auto s = glm::scale(eye, glm::dvec3{invScale, 1.0});

    return s * t;
  }
}

#endif  // DUMAGEVIEW_RENDERVIEW_INL_H_

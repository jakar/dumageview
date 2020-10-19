#ifndef DUMAGEVIEW_RENDERVIEW_H_
#define DUMAGEVIEW_RENDERVIEW_H_

#include "dumageview/math.h"

#include <glm/glm.hpp>

#include <type_traits>
#include <variant>

namespace dumageview::renderview {
  inline constexpr double minZoom{0.05};
  inline constexpr double maxZoom{20.0};

  struct ZoomToFitView {};

  struct ManualView {
    double scale{1.0};
    glm::dvec2 position{0.0};
  };

  using View = std::variant<ZoomToFitView, ManualView>;

  struct SizeInfo {
    glm::dvec2 image;
    glm::dvec2 screen;
  };

  /**
   * Modifies a view.
   * This is a feeble attempt to separate view-fiddling stuff from other image
   * drawing stuff. Definitely not the cleanest or most efficient way to go
   * about it, but it does avoid allocs.
   */
  template<class V>
  class ViewMod;

  template<>
  class ViewMod<View>;

  template<>
  class ViewMod<ManualView>;

  template<class V>
  ViewMod(V, SizeInfo const&) -> ViewMod<std::decay_t<V>>;

  template<class Derived>
  class BaseViewMod {
   public:
    BaseViewMod(View const& v, SizeInfo const& s) : _view{v}, _size{s} {
    }
    BaseViewMod(BaseViewMod const&) = default;

    template<class F>
    Derived& modify(F&& func);

    Derived& normalize();
    ViewMod<ManualView> reified() const;

    SizeInfo const& size() const {
      return _size;
    }

    Derived& dref() {
      return static_cast<Derived&>(*this);
    }
    Derived const& dref() const {
      return static_cast<Derived const&>(*this);
    }

   protected:
    View _view;
    SizeInfo const _size;
  };

  template<>
  class ViewMod<View> : public BaseViewMod<ViewMod<View>> {
   public:
    using Base = BaseViewMod<ViewMod<View>>;
    using Base::Base;

    glm::dvec2 imageToScreen(glm::dvec2 const& pos) const;
    glm::dvec2 screenToImage(glm::dvec2 const& pos) const;

    glm::dmat4 imageToScreenMatrix() const;
    glm::dmat4 screenToImageMatrix() const;

    View& view() {
      return _view;
    }
    View const& view() const {
      return _view;
    }
  };

  template<>
  class ViewMod<ManualView> : public BaseViewMod<ViewMod<ManualView>> {
   public:
    using Base = BaseViewMod<ViewMod<ManualView>>;

    ViewMod(ManualView const& v, SizeInfo const& s) : Base(v, s) {
    }
    ViewMod(ViewMod const&) = default;

    ViewMod& center(math::Getter const& dim);
    ViewMod& clamp(math::Getter const& dim);

    ViewMod& setZoom(double scale, glm::dvec2 const& screenFixed);

    glm::dvec2 imageToScreen(glm::dvec2 const& pos) const;
    glm::dvec2 screenToImage(glm::dvec2 const& pos) const;

    glm::dmat4 imageToScreenMatrix() const;
    glm::dmat4 screenToImageMatrix() const;

    ManualView& view() {
      return std::get<ManualView>(_view);
    }
    ManualView const& view() const {
      return std::get<ManualView>(_view);
    }
  };
}

#endif  // DUMAGEVIEW_RENDERVIEW_H_

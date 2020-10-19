#ifndef DUMAGEVIEW_MATH_H_
#define DUMAGEVIEW_MATH_H_

/**
 * \file
 * Loose collection of vaguely semi-math-related things.
 */

#include "dumageview/assert.h"
#include "dumageview/conv_vec.h"

#include <glm/glm.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <array>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace dumageview::math {
  //
  // Generic accessors
  //

  namespace detail::accessors {
    template<class V>
    struct Scalar;
    template<>
    struct Scalar<glm::ivec2> {
      using type = int;
    };
    template<>
    struct Scalar<glm::dvec2> {
      using type = double;
    };
    template<>
    struct Scalar<glm::ivec2 const> {
      using type = int const;
    };
    template<>
    struct Scalar<glm::dvec2 const> {
      using type = double const;
    };
    template<class V>
    using ScalarT = typename Scalar<V>::type;

    template<class V>
    using GetterFunc = ScalarT<V>& (*)(V&);

    template<class V>
    struct TypedGetter {
      GetterFunc<V> lhs;
      GetterFunc<V const> rhs;
    };

    struct Getter {
      TypedGetter<glm::ivec2> iget;
      TypedGetter<glm::dvec2> dget;

      constexpr decltype(auto) operator()(glm::ivec2& v) const {
        return iget.lhs(v);
      }

      constexpr decltype(auto) operator()(glm::ivec2 const& v) const {
        return iget.rhs(v);
      }

      constexpr decltype(auto) operator()(glm::dvec2& v) const {
        return dget.lhs(v);
      }

      constexpr decltype(auto) operator()(glm::dvec2 const& v) const {
        return dget.rhs(v);
      }
    };

    template<class V>
    inline constexpr GetterFunc<V> vGetX = [](V& v) -> auto& {
      return v.x;
    };

    template<class V>
    inline constexpr GetterFunc<V> vGetY = [](V& v) -> auto& {
      return v.y;
    };

    inline constexpr Getter getX{{vGetX<glm::ivec2>, vGetX<glm::ivec2 const>},
                                 {vGetX<glm::dvec2>, vGetX<glm::dvec2 const>}};

    inline constexpr Getter getY{{vGetY<glm::ivec2>, vGetY<glm::ivec2 const>},
                                 {vGetY<glm::dvec2>, vGetY<glm::dvec2 const>}};

    inline constexpr auto dimensions2D = {getX, getY};
  }

  using detail::accessors::dimensions2D;
  using detail::accessors::Getter;
  using detail::accessors::getX;
  using detail::accessors::getY;

  //
  // Misc
  //

  template<class Vec>
  double aspectRatio(Vec const& u) {
    auto v = conv::dvec(u);
    DUMAGEVIEW_ASSERT(v.x > 0.0);
    DUMAGEVIEW_ASSERT(v.y > 0.0);
    return v.x / v.y;
  }

  template<class Num>
  constexpr Num ipow(Num base, int exp) {
    if (exp < 0) {
      if constexpr (std::is_floating_point_v<Num>) {
        return ipow(static_cast<Num>(1) / base, -exp);
      } else {
        throw std::domain_error("exponent must be nonnegative");
      }
    } else if (exp == 0) {
      return static_cast<Num>(1);
    } else if (exp % 2 == 0) {
      auto s = ipow(base, exp / 2);
      return s * s;
    } else {
      return ipow(base, exp - 1) * base;
    }
  }

  template<class Num, class Int>
  Num mod(Num n, Int m) {
    static_assert(std::is_integral_v<Int>);

    static_assert(std::is_integral_v<Num> || std::is_floating_point_v<Num>);

    if (m <= 0) throw std::domain_error("modulus must be positive");

    Num mm = boost::numeric_cast<Num>(m);

    if constexpr (std::is_integral_v<Num>) {
      return ((n % mm) + mm) % mm;
    } else if constexpr (std::is_floating_point_v<Num>) {
      Num r = std::fmod(n, mm);
      if (r < 0)
        return r + mm;
      else
        return r;
    } else {
      DUMAGEVIEW_ASSERT(false);
      return 0;
    }
  }
}

#endif  // DUMAGEVIEW_MATH_H_

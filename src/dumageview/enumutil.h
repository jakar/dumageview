#ifndef DUMAGEVIEW_ENUMUTIL_H_
#define DUMAGEVIEW_ENUMUTIL_H_

#include <boost/hana.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <type_traits>

namespace dumageview::enumutil
{
  /**
   * Casts enum value to underlying interger type.
   */
  template<class E>
  constexpr auto cast(E enumVal,
                      std::enable_if_t<std::is_enum_v<E>>* = nullptr)
  {
    return static_cast<std::underlying_type_t<E>>(enumVal);
  }

  /**
   * Casts enum value to arbitrary interger type.
   */
  template<class I, class E>
  constexpr I cast(
    E enumVal,
    std::enable_if_t<std::is_enum_v<E> && std::is_integral_v<I>>* = nullptr)
  {
    using U = std::underlying_type_t<E>;

    if constexpr(std::is_same_v<U, I> || boost::hana::is_embedded<U, I>{})
    {
      return static_cast<I>(enumVal);
    }
    else
    {
      return boost::numeric_cast<I>(cast<E>(enumVal));
    }
  }
}

#endif // DUMAGEVIEW_ENUMUTIL_H_

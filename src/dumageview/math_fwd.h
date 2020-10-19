#ifndef DUMAGEVIEW_MATH_FWD_H_
#define DUMAGEVIEW_MATH_FWD_H_

namespace dumageview::math {
  template<class Vec>
  double aspectRatio(Vec const& u);

  template<class Num>
  constexpr Num ipow(Num base, int exp);

  template<class Num, class Int>
  Num mod(Num n, Int m);

  namespace detail::accessors {
    struct Getter;
  }

  using detail::accessors::Getter;
}

#endif  // DUMAGEVIEW_MATH_FWD_H_

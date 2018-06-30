#ifndef DUMAGEVIEW_CLASSTOOLS_H_
#define DUMAGEVIEW_CLASSTOOLS_H_

#include "dumageview/assert.h"

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace dumageview::classtools
{
  enum class Mutability { mutable_, const_ };

  /**
   * Keeps reference to global instance of class.
   */
  template<Mutability mut, class Derived>
  class Singleton
  {
   public:
    using Ref =
      std::conditional_t<
        mut == Mutability::mutable_,
        Derived&,
        Derived const&
      >;

    virtual ~Singleton()
    {
      DUMAGEVIEW_ASSERT(_instance != nullptr);
      _instance = nullptr;
    }

    static Ref singletonInstance()
    {
      DUMAGEVIEW_ASSERT(_instance);
      return static_cast<Ref>(*_instance);
    }

    static bool singletonInstantiated()
    {
      return (_instance != nullptr);
    }

   protected:
    Singleton()
    {
      DUMAGEVIEW_ASSERT(_instance == nullptr);
      _instance = this;
    }

   private:
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    inline static Singleton* _instance = nullptr;
  };

  template<class D>
  using ConstSingleton = Singleton<Mutability::const_, D>;

  template<class D>
  using MutableSingleton = Singleton<Mutability::mutable_, D>;

  //
  // Misc utils
  //

  template<class T>
  auto copyUnique(T&& val)
  {
    return std::make_unique<std::decay_t<T>>(std::forward<T>(val));
  }

  template<class T>
  auto copyShared(T&& val)
  {
    return std::make_shared<std::decay_t<T>>(std::forward<T>(val));
  }
}

#endif // DUMAGEVIEW_CLASSTOOLS_H_

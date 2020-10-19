#ifndef DUMAGEVIEW_CLASSTOOLS_H_
#define DUMAGEVIEW_CLASSTOOLS_H_

#include "dumageview/assert.h"

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace dumageview::classtools {
  enum class Mutability { kMutable, kConst };

  /**
   * Keeps reference to global instance of class.
   */
  template<Mutability mut, class Derived>
  class Singleton {
   public:
    using Ref =
      std::conditional_t<mut == Mutability::kMutable, Derived&, Derived const&>;

    virtual ~Singleton() {
      DUMAGEVIEW_ASSERT(instance_ != nullptr);
      instance_ = nullptr;
    }

    static Ref getSingletonInstance() {
      DUMAGEVIEW_ASSERT(instance_);
      return static_cast<Ref>(*instance_);
    }

    static bool isSingletonInstantiated() {
      return (instance_ != nullptr);
    }

   protected:
    Singleton() {
      DUMAGEVIEW_ASSERT(instance_ == nullptr);
      instance_ = this;
    }

   private:
    Singleton(Singleton const&) = delete;
    Singleton& operator=(Singleton const&) = delete;

    inline static Singleton* instance_ = nullptr;
  };

  template<class D>
  using ConstSingleton = Singleton<Mutability::kConst, D>;

  template<class D>
  using MutableSingleton = Singleton<Mutability::kMutable, D>;

  //
  // Misc utils
  //

  template<class T>
  auto copyUnique(T&& val) {
    return std::make_unique<std::decay_t<T>>(std::forward<T>(val));
  }

  template<class T>
  auto copyShared(T&& val) {
    return std::make_shared<std::decay_t<T>>(std::forward<T>(val));
  }
}

#endif  // DUMAGEVIEW_CLASSTOOLS_H_

#ifndef DUMAGEVIEW_SCOPEGUARD_H_
#define DUMAGEVIEW_SCOPEGUARD_H_

#include <type_traits>
#include <utility>

namespace dumageview {
  template<class F>
  class [[nodiscard]] ScopeGuard {
   public:
    explicit ScopeGuard(F const& handler) noexcept : handler_(handler) {
    }

    explicit ScopeGuard(F && handler) noexcept : handler_(std::move(handler)) {
    }

    ScopeGuard(ScopeGuard && rhs) noexcept
        : handler_(std::move(rhs.handler_)), released_(rhs.released_) {
      rhs.release();
    }

    ~ScopeGuard() noexcept {
      if (!released_) handler_();
    }

    void release() noexcept {
      released_ = true;
    }

   private:
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;

    F handler_;
    bool released_ = false;
  };

  template<class F>
  ScopeGuard(F) -> ScopeGuard<std::decay_t<F>>;
}

#endif  // DUMAGEVIEW_SCOPEGUARD_H_

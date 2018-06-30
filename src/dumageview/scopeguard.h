#ifndef DUMAGEVIEW_SCOPEGUARD_H_
#define DUMAGEVIEW_SCOPEGUARD_H_

#include <utility>
#include <type_traits>

namespace dumageview
{
  template<class F>
  class [[nodiscard]] ScopeGuard
  {
   public:
    explicit ScopeGuard(F const& handler) noexcept
    :
      _handler(handler)
    {
    }

    explicit ScopeGuard(F&& handler) noexcept
    :
      _handler(std::move(handler))
    {
    }

    ScopeGuard(ScopeGuard&& rhs) noexcept
    :
      _handler(std::move(rhs._handler)),
      _released(rhs._released)
    {
      rhs.release();
    }

    ~ScopeGuard() noexcept
    {
      if (!_released)
        _handler();
    }

    void release() noexcept
    {
      _released = true;
    }

   private:
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard&&) = delete;

    F _handler;
    bool _released = false;
  };

  template<class F> ScopeGuard(F) -> ScopeGuard<std::decay_t<F>>;
}

#endif  // DUMAGEVIEW_SCOPEGUARD_H_

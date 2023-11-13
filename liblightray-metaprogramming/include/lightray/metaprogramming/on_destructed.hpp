#pragma once

#include <utility>

namespace lightray::mtp
{
    template <std::invocable Fn>
    struct on_destructed
    {
    private:
        Fn _fn;
        bool _alive;

    public:
        constexpr on_destructed() noexcept(noexcept(Fn{})) : _fn{}, _alive(true) {}

        on_destructed(on_destructed&& other) noexcept(noexcept(Fn{std::move(other._fn)}))
        :   _fn{std::move(other._fn)},
            _alive(std::exchange(other._alive, false))
        {}

        on_destructed(const on_destructed&) = delete;
        on_destructed& operator=(const on_destructed&) = delete;
        on_destructed& operator=(on_destructed&&) = delete;

        template <typename... Args>
        on_destructed(Args&&... args) noexcept(noexcept(Fn{std::forward<Args>(args)...}))
        :   _fn{std::forward<Args>(args)...}, _alive(true)
        {}

        ~on_destructed() noexcept(noexcept(_fn()))
        {
            if (_alive) _fn();
        }
    };

    template <typename Fn>
    on_destructed(Fn) -> on_destructed<Fn>;

} // namespace lightray::mtp

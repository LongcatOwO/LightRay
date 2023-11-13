#pragma once

#include <utility>

namespace lightray::mtp
{
    /*
     * Turns a group of functors into an overloaded functor between
     * all of them.
     *
     * Author: P. Lutchanont
     */
    template <typename... Fns>
    struct overload : Fns...
    {
        // default constructs each functor
        overload() = default;

        // constructs each functor using each arguments.
        template <typename... Fs>
        constexpr overload(Fs&&... fs) noexcept(noexcept((Fns(std::forward<Fs>(fs)), ...)))
        :   Fns(std::forward<Fs>(fs))...
        {}

        // use the combined set of overloaded operator() of from all functors together.
        using Fns::operator()...;

    }; // struct overloaded

    // deduce by value.
    template <typename... Fns>
    overload(Fns...) -> overload<Fns...>;
    
} // namespace lightray::mtp

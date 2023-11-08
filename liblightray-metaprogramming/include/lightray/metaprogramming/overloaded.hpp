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
    struct overloaded : Fns...
    {
        // default constructs each functor
        overloaded() = default;

        // constructs each functor using each arguments.
        template <typename... Fs>
        constexpr overloaded(Fs&&... fs) noexcept(noexcept((Fns(std::forward<Fs>(fs)), ...)))
        :   Fns(std::forward<Fs>(fs))...
        {}

        // use the combined set of overloaded operator() of from all functors together.
        using Fns::operator()...;

    }; // struct overloaded

    // deduce by value.
    template <typename... Fns>
    overloaded(Fns...) -> overloaded<Fns...>;
    
} // namespace lightray::mtp

#pragma once

#include <lightray/metaprogramming/type.hpp>
#include <lightray/metaprogramming/value.hpp>

namespace lightray::mtp::functors
{
    template <template <typename...> class Trait>
    constexpr auto t2t_trait_func = []<typename... Args>
    {
        return type<typename Trait<Args...>::type>;
    };

    template <template <typename...> class Trait>
    constexpr auto t2v_trait_func = []<typename... Args>
    {
        return value<Trait<Args...>::value>;
    };

    template <template <auto...> class Trait>
    constexpr auto v2t_trait_func = []<auto... Args>
    {
        return type<typename Trait<Args...>::type>;
    };

    template <template <auto...> class Trait>
    constexpr auto v2v_trait_func = []<auto... Args>
    {
        return value<Trait<Args...>::value>;
    };

} // namespace lightray::mtp::functors

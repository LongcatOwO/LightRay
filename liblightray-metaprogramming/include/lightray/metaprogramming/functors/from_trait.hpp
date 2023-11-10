#pragma once

#include <lightray/metaprogramming/type.hpp>
#include <lightray/metaprogramming/value.hpp>

namespace lightray::mtp::functors
{
    template <template <typename...> class Trait>
    constexpr auto from_t2t_trait = []<typename... Args>
    {
        return type<typename Trait<Args...>::type>;
    };

    template <template <typename...> class Trait>
    constexpr auto from_t2v_trait = []<typename... Args>
    {
        return value<Trait<Args...>::value>;
    };

    template <template <auto...> class Trait>
    constexpr auto from_v2t_trait = []<auto... Args>
    {
        return type<typename Trait<Args...>::type>;
    };

    template <template <auto...> class Trait>
    constexpr auto from_v2v_trait = []<auto... Args>
    {
        return value<Trait<Args...>::value>;
    };

} // namespace lightray::mtp::functors

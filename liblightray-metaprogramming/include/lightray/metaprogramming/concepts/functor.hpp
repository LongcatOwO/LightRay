#pragma once

#include "common.hpp"

namespace lightray::mtp::concepts
{
    template <typename Functor, typename... Args>
    concept type_functor = requires (Functor f)
    {
        f.template operator()<Args...>();
    };

    template <typename Functor, typename... Args>
    concept nothrow_type_functor = requires (Functor f)
    {
        { f.template operator()<Args...>() } noexcept;
    };

    template <typename Functor, typename Result, typename... Args>
    concept type_to_value_functor = type_functor<Functor, Args...> && requires (Functor f)
    {
        { decltype(f.template operator()<Args...>())::value } -> convertible_to<Result>;
    };

    template <typename Functor, typename... Args>
    concept type_to_type_functor = type_functor<Functor, Args...> && requires (Functor f)
    {
        typename decltype(f.template operator()<Args...>())::type;
    };


    template <typename Functor, auto... Args>
    concept value_functor = requires (Functor f)
    {
        f.template operator()<Args...>();
    };

    template <typename Functor, auto... Args>
    concept nothrow_value_functor = requires (Functor f)
    {
        { f.template operator()<Args...>() } noexcept;
    };

    template <typename Functor, typename Result, auto... Args>
    concept value_to_value_functor = value_functor<Functor, Args...> && requires (Functor f)
    {
        { decltype(f.template operator()<Args...>())::value } -> convertible_to<Result>;
    };

    template <typename Functor, typename Result, auto... Args>
    concept value_to_type_functor = requires (Functor f)
    {
        typename decltype(f.template operator()<Args...>())::type;
    };

} // namespace lightray::mtp::concepts

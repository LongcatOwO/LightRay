#pragma once

#include <cassert>
#include <concepts>
#include <type_traits>

#include <lightray/metaprogramming/traits/qualifier_traits.hpp>

// for enum_cast
#include "enum_util.hpp" // IWYU pragma: export


namespace lightray::mtp
{
    template <typename BaseOrDerived, typename T>
    concept public_base_or_derived_of = std::derived_from<T, BaseOrDerived> || std::derived_from<BaseOrDerived, T>;;

    template <typename BaseOrDerived, typename T>
    concept base_or_derived_of = std::is_base_of_v<BaseOrDerived, T> || std::is_base_of_v<T, BaseOrDerived>;

    template <typename To, typename From>
    constexpr auto assert_dynamic_cast(From&& from) noexcept -> void
    {
        if constexpr (std::is_polymorphic_v<std::remove_cvref_t<From>>)
        {
            assert(dynamic_cast<decltype(&std::declval<To>())>(&from));
        }
    }

    template <typename To, typename From>
    requires 
        std::is_reference_v<To> 
     && public_base_or_derived_of<std::remove_cvref_t<To>, std::remove_cvref_t<From>>
    constexpr auto cast(From&& from) noexcept -> To
    {
        assert_dynamic_cast<To>(std::forward<From>(from));
        return static_cast<To>(from);
    }

    template <typename To, typename From>
    requires public_base_or_derived_of<std::remove_cvref_t<To>, std::remove_cvref_t<From>>
    constexpr auto ldefault_cast(From&& from) noexcept -> decltype(auto)
    {
        if constexpr (std::is_reference_v<To>)
            return cast<To>(std::forward<From>(from));
        else
            return cast<std::add_lvalue_reference_t<To>>(std::forward<From>(from));
    }

    template <typename To, typename From>
    requires public_base_or_derived_of<std::remove_cvref_t<To>, std::remove_cvref_t<From>>
    constexpr auto rdefault_cast(From&& from) noexcept -> decltype(auto)
    {
        if constexpr (std::is_reference_v<To>)
            return cast<To>(std::forward<From>(from));
        else
            return cast<std::add_rvalue_reference_t<To>>(std::forward<From>(from));
    }

    template <typename To, base_or_derived_of<To> From>
    requires
        std::is_reference_v<To>
     && base_or_derived_of<std::remove_cvref_t<To>, std::remove_cvref_t<From>>
    constexpr auto private_cast(From&& from) noexcept -> To
    {
        assert_dynamic_cast<To>(std::forward<From>(from));
        return (To) from;
    }

    template <typename To, typename From>
    requires std::is_convertible_v<From, To>
    constexpr auto implicit_cast(From&& from)
    noexcept(std::is_nothrow_convertible_v<From&&, To>)
    -> To
    {
        return std::forward<From>(from);
    }

    template <typename To, public_base_or_derived_of<To> From>
    requires traits::qualifier_traits<To>::is_unqualified
    constexpr auto forward_cast(From&& from) noexcept -> decltype(auto)
    {
        using qual_traits = traits::qualifier_traits<From&&>;
        using target_type = typename qual_traits::template apply<To>;
        assert_dynamic_cast<target_type>(std::forward<From>(from));
        return static_cast<target_type>(from);
    }

    template <typename To, base_or_derived_of<To> From>
    requires traits::qualifier_traits<To>::is_unqualified
    constexpr auto private_forward_cast(From&& from) noexcept -> decltype(auto)
    {
        using qual_traits = traits::qualifier_traits<From&&>;
        using target_type = typename qual_traits::template apply<To>;
        assert_dynamic_cast<target_type>(std::forward<From>(from));
        return (target_type) from; // safe as type relation is checked
                                   // and target_type guaranteed to be reference qualified
    }

} // namespace lightray::mtp

#pragma once

#include <concepts>
#include <type_traits>

namespace lightray::mtp
{
    constexpr struct auto_t
    {
        auto_t() = default;
        constexpr auto_t(auto) {}

    } auto_;

    constexpr struct decltype_auto_t
    {
        decltype_auto_t() = default;
        constexpr decltype_auto_t(auto&&) {}

    } decltype_auto;

    constexpr struct none_t
    {
        constexpr auto operator==(none_t) const noexcept -> bool { return true; }

        template <typename T> requires (!std::same_as<none_t, std::remove_cvref_t<T>>)
        constexpr auto operator==(T&&) const noexcept -> bool { return false; }

    } none;

} // namespace lightray::mtp

#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace lightray::mtp
{
    template <typename T>
    concept enum_type = std::is_enum_v<T>;

    template <typename T>
    concept enum_flag_type = enum_type<T> && requires (const T& t) { lightray_mtp_enum_flag(t); };

    template <enum_type To, std::same_as<std::underlying_type_t<To>> From>
    constexpr auto enum_cast(const From& from) noexcept -> To
    {
        return static_cast<To>(from);
    }

} // namespace lightray::mtp

template <lightray::mtp::enum_flag_type T> 
constexpr auto operator^ (const T& lhs, const T& rhs) noexcept -> T
{
    return lightray::mtp::enum_cast<T>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
}

template <lightray::mtp::enum_flag_type T>
constexpr auto operator& (const T& lhs, const T& rhs) noexcept -> T
{
    return lightray::mtp::enum_cast<T>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

template <lightray::mtp::enum_flag_type T>
constexpr auto operator| (const T& lhs, const T& rhs) noexcept -> T
{
    return lightray::mtp::enum_cast<T>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

template <lightray::mtp::enum_flag_type T>
constexpr auto operator~ (const T& t) noexcept -> T
{
    return lightray::mtp::enum_cast<T>(~std::to_underlying(t));
}

template <lightray::mtp::enum_flag_type T>
constexpr auto has_flag(const T& flags, const T& flags_to_check) noexcept -> bool
{
    return (flags & flags_to_check) == flags_to_check;
}

#pragma once

#include <concepts>
#include <type_traits>

namespace lightray::mtp
{
    // type declared as 'enum'
    template <typename T>
    concept enum_type = std::is_enum_v<T>;

    // enum_type with enum flag operations enabled
    // by declaring the function lightray_mtp_enum_flag(T) in the same namespace.
    template <typename T>
    concept enum_flag_type = enum_type<T> && requires (const T& t) { lightray_mtp_enum_flag(t); };

    // Cast 'from' into the enum type 'To'.
    // Fails at compile-time if from's type does not match exactly with To's underlying type.
    template <enum_type To, std::same_as<std::underlying_type_t<To>> From>
    constexpr auto enum_cast(From from) noexcept -> To
    {
        return static_cast<To>(from);
    }

    // Cast the enum value 'e' to its underlying type
    template <enum_type Enum>
    constexpr auto to_underlying(Enum e) noexcept -> std::underlying_type_t<Enum>
    {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }

} // namespace lightray::mtp

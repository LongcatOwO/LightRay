#pragma once

#include "enum_util.hpp"

// Enum flag bitwise XOR operator.
// Participates in overload resolution only if Enum is an enum_flag_type.
template <lightray::mtp::enum_flag_type Enum> 
constexpr auto operator^ (Enum lhs, Enum rhs) noexcept -> Enum
{
    namespace lr = lightray::mtp;
    return lr::enum_cast<Enum>(lr::to_underlying(lhs), lr::to_underlying(rhs));
}

// Enum flag bitwise AND operator.
// Participates in overload resolution only if Enum is an enum_flag_type.
template <lightray::mtp::enum_flag_type Enum>
constexpr auto operator& (Enum lhs, Enum rhs) noexcept -> Enum
{
    namespace lr = lightray::mtp;
    return lr::enum_cast<Enum>(lr::to_underlying(lhs) & lr::to_underlying(rhs));
}

// Enum flag bitwise OR operator
// Participates in overload resolution only if Enum is an enum_flag_type.
template <lightray::mtp::enum_flag_type Enum>
constexpr auto operator| (Enum lhs, Enum rhs) noexcept -> Enum
{
    namespace lr = lightray::mtp;
    return lr::enum_cast<Enum>(lr::to_underlying(lhs) | lr::to_underlying(rhs));
}

// Enum flag bitwise NOT operator
// Participates in overload resolution only if Enum is an enum_flag_type.
template <lightray::mtp::enum_flag_type Enum>
constexpr auto operator~ (Enum e) noexcept -> Enum
{
    namespace lr = lightray::mtp;
    return lr::enum_cast<Enum>(~lr::to_underlying(e));
}

// True if all bit fields in flags_to_check are also set in flags.
// Otherwise, false.
template <lightray::mtp::enum_flag_type Enum>
constexpr auto has_flag(Enum flags, Enum flags_to_check) noexcept -> bool
{
    return (flags & flags_to_check) == flags_to_check;
}

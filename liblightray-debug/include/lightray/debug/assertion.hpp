#pragma once

#include <source_location>
#include <stdexcept>
#include <string_view>

#include "export.hpp"

namespace lightray::debug
{
    struct LIGHTRAY_DEBUG_SYMEXPORT assertion_error : std::runtime_error
    {
        assertion_error(std::string_view message, const std::source_location& location = std::source_location::current());
    };

    LIGHTRAY_DEBUG_SYMEXPORT auto assert_true(
        bool condition,
        std::string_view message,
        const std::source_location& location = std::source_location::current()
    ) -> void;

    // template <typename T, typename U>
    // auto assert_equal(const T& expected, const U& actual) -> void
    // {
    // }


} // namespace lightray::debug

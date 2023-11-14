#include <format>
#include <string_view>
#include <source_location>

#include <lightray/debug/assertion.hpp>

namespace lightray::debug
{
    assertion_error::assertion_error(
        std::string_view message,
        const std::source_location& location
    ) 
    :   std::runtime_error(std::format(
            "{}:{}:{}:\nInside [{}]:\n{}",
            location.file_name(),
            location.line(),
            location.column(),
            location.function_name(),
            message
        ))
    {}

    auto assert_true(
        bool condition,
        std::string_view message,
        const std::source_location& location
    ) -> void
    {
        if (condition) return;

        throw assertion_error(std::format("assert_true(cond, message) failed:\n{}", message), location);
    }


}; // namespace lightray::debug

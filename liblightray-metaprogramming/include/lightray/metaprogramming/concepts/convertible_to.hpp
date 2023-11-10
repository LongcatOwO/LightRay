#pragma once

#include <concepts>
#include <type_traits>

namespace lightray::mtp::concepts
{
    struct auto_t
    {
        auto_t(auto) {}
    };

    struct decltype_auto_t
    {
        decltype_auto_t(auto&&) {}
    };

    template <typename From, typename To>
    concept convertible_to = std::convertible_to<From, To> || std::same_as<void, std::remove_cv_t<To>>;

} // namespace lightray::mtp::concepts

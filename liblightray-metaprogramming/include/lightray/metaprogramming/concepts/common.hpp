#pragma once

#include <concepts>
#include <type_traits>

namespace lightray::mtp::concepts
{
    template <typename From, typename To>
    concept convertible_to = std::convertible_to<From, To> || std::same_as<void, std::remove_cv_t<To>>;

    template <typename Derived, typename... Bases>
    concept public_derived_from = (... && std::derived_from<Derived, Bases>);
    
    template <typename Derived, typename... Bases>
    concept derived_from = (... && std::is_base_of_v<Bases, Derived>);

} // namespace lightray::mtp::concepts

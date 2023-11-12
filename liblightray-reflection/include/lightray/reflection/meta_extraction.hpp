#pragma once

#include <utility>

namespace lightray::refl
{
    constexpr struct extract_meta_t {} extract_meta;

    template <typename T>
    using meta_type_t = decltype(extract_meta << std::declval<T>());

    template <typename T>
    concept reflected = requires { typename meta_type_t<T>; };

    template <reflected T>
    constexpr auto meta_type = meta_type_t<T>{};

} // namespace lightray::refl

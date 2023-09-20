#ifndef _LIGHTRAY_COREDEF_HPP_
#define _LIGHTRAY_COREDEF_HPP_


#include <type_traits>


namespace lightray
{
    enum class category : unsigned
    {
        type                = 1 << 0,
        type_template       = 1 << 1,
        variable            = 1 << 2,
        variable_template   = 1 << 3,
        function            = 1 << 4,
    };

    constexpr category operator|(category lhs, category rhs) noexcept
    {
        using type = std::underlying_type_t<category>;
        return static_cast<category>(
            static_cast<type>(lhs) | static_cast<type>(rhs)
        );
    }

    constexpr category operator&(category lhs, category rhs) noexcept
    {
        using type = std::underlying_type_t<category>;
        return static_cast<category>(
            static_cast<type>(lhs) & static_cast<type>(rhs)
        );
    }

    constexpr category operator^(category lhs, category rhs) noexcept
    {
        using type = std::underlying_type_t<category>;
        return static_cast<category>(
            static_cast<type>(lhs) ^ static_cast<type>(rhs)
        );
    }

    template <typename T>
    using meta = decltype(lightray_meta_get(std::declval<T>()));

    template <typename T>
    concept reflected = requires { typename meta<T>; };
}


#endif

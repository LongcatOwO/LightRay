#ifndef _LIGHTRAY_TRAITS_COMMON_HPP_
#define _LIGHTRAY_TRAITS_COMMON_HPP_


#include <cstddef>
#include <type_traits>
#include <utility>


namespace lightray::traits
{
    template <typename T>
    constexpr bool always_false = false;

    template <typename T>
    constexpr bool always_true = true;

    template <typename C, auto Pointer>
    struct pointer_invoker {};

    template <typename C, typename T, T *Pointer>
    struct pointer_invoker<C, Pointer>
    {
        static constexpr decltype(auto) invoke(std::nullptr_t) noexcept
        { return *Pointer; }

        template <typename Self>
        requires std::is_base_of_v<C, std::remove_cvref_t<Self>>
        static constexpr decltype(auto) invoke(Self &&self) noexcept
        { return *Pointer; }
    };

    template <typename C, typename M, M C::* Pointer>
    struct pointer_invoker<C, Pointer>
    {
        template <typename Self>
        requires std::is_base_of_v<C, std::remove_cvref_t<Self>>
        static constexpr decltype(auto) invoke(Self &&self) noexcept
        { return std::forward<Self>(self).*Pointer; }
    };

    template <typename T>
    struct remove_member_pointer {};

    template <typename C, typename M>
    struct remove_member_pointer<M C::*> { using type = M; };

    template <typename T>
    using remove_member_pointer_t = typename remove_member_pointer<T>::type;
}


#endif

#pragma once

#include <type_traits>

#include <lightray/metaprogramming/type.hpp>

namespace lightray::mtp::traits
{
    /*
     * Provides a member typedef template 'type' which
     * adds const qualifier to the given template parameter
     * if State is true, otherwise do nothing.
     *
     * Author: P. Lutchanont
     */
    template <bool State>
    struct set_const;

    template <>
    struct set_const<true>
    {
        template <typename T>
        using type = const T;

    }; // set_const<true>

    template <>
    struct set_const<false>
    {
        template <typename T>
        using type = T;

    }; // set_const<false>

    /*
     * Aliases to typename set_const<State>::template type<T>.
     */
    template <bool State, typename T>
    using set_const_t = typename set_const<State>::template type<T>;

    /*
     * Provides a member typedef template 'type' which
     * adds volatile qualifier to the given template paramter
     * if State is true, otherwise do nothing.
     *
     * Author: P. Lutchanont
     */
    template <bool State>
    struct set_volatile;

    template <>
    struct set_volatile<true>
    {
        template <typename T>
        using type = volatile T;

    }; // set_volatile<true>

    template <>
    struct set_volatile<false>
    {
        template <typename T>
        using type = T;

    }; // set_volatile<false>

    /*
     * Aliases to typename set_volatile<State>::template type<T>.
     */
    template <bool State, typename T>
    using set_volatile_t = typename set_volatile<State>::template type<T>;


    /*
     * Provides a member typedef template 'type' which
     * adds lvalue reference qualifier to the given template parameter
     * if State is true, otherwise do nothing.
     *
     * Author: P. Lutchanont
     */
    template <bool State>
    struct set_lvalue_reference;

    template <>
    struct set_lvalue_reference<true>
    {
        template <typename T>
        using type = T&;

    }; // set_lvalue_reference<true>

    template <>
    struct set_lvalue_reference<false>
    {
        template <typename T>
        using type = T;

    }; // set_lvalue_reference<false>

    /*
     * Aliases to typename set_lvalue_reference<State>::template type<T>.
     */
    template <bool State, typename T>
    using set_lvalue_reference_t = typename set_lvalue_reference<State>::template type<T>;


    /*
     * Provides a member typedef template 'type' which
     * adds rvalue reference qualifier to the given template parameter
     * if State is true, otherwise do nothing.
     *
     * Author: P. Lutchanont
     */
    template <bool State>
    struct set_rvalue_reference;

    template <>
    struct set_rvalue_reference<true>
    {
        template <typename T>
        using type = T&&;

    }; // set_rvalue_reference<true>

    template <>
    struct set_rvalue_reference<false>
    {
        template <typename T>
        using type = T;

    }; // set_rvalue_reference<false>

    /*
     * Aliases to typename set_rvalue_reference<State>::template type<T>.
     */
    template <bool State, typename T>
    using set_rvalue_reference_t = typename set_rvalue_reference<State>::template type<T>;


    /*
     * Provides traits for the qualifiers of T.
     * This includes const, volatile, lvalue reference, and rvalue reference qualifiers.
     *
     * As the traits are purely for the qualifier of the type, not the type itself,
     * this means that different types with same cv-ref qualifier have equivalent
     * qualifier_traits.
     *
     * qualifier_traits<const int&> is equivalent to qualifier_traits<const std::string&>.
     * 
     * This class also supports implicit conversion between its own equivalent templated types.
     * This means that qualifier_traits<const int&> is implicitly convertible to 
     * qualifier_traits<const std::string&> and vice versa.
     *
     * Author: P. Lutchanont
     */
    template <typename T>
    struct qualifier_traits
    {
        // Defines to true if T has const qualifier after removing its reference qualifier.
        // Otherwise, false.
        static constexpr auto is_const = std::is_const_v<std::remove_reference_t<T>>;

        // Defines to true if T has volatile qualifier after removing its reference qualifier.
        // Otherwise, false.
        static constexpr auto is_volatile = std::is_volatile_v<std::remove_reference_t<T>>;

        // Defines to true if T has reference qualifier.
        // Otherwise, false.
        static constexpr auto is_reference = std::is_reference_v<T>;

        // Defines to true if T has lvalue reference qualifier.
        // Otherwise, false.
        static constexpr auto is_lvalue_reference = std::is_lvalue_reference_v<T>;

        // Defines to true if T has rvalue reference qualifier.
        // Otherwise, false.
        static constexpr auto is_rvalue_reference = std::is_rvalue_reference_v<T>;

        static constexpr auto is_qualified = is_const || is_volatile || is_reference;

        static constexpr auto is_unqualified = !is_qualified;

        // If is_const is true, add const qualifier to U. Otherwise, do nothing.
        template <typename U>
        using apply_const = set_const_t<is_const, U>;

        // If is_volatile is true, add volatile qualifier to U. Otherwise do nothing.
        template <typename U>
        using apply_volatile = set_volatile_t<is_volatile, U>;

        // Equivalent to apply_volatile< apply_const< U >>.
        template <typename U>
        using apply_cv = apply_volatile< apply_const< U >>;

        // If is_lvalue_reference is true, add lvalue reference qualifier to U. Otherwise do nothing.
        template <typename U>
        using apply_lvalue = set_lvalue_reference_t<is_lvalue_reference, U>;

        // If is_rvalue_reference is true, add rvalue reference qualifier to U, Otherwise do nothing.
        template <typename U>
        using apply_rvalue = set_rvalue_reference_t<is_rvalue_reference, U>;

        // Applies the cv-ref qualifiers of T to U
        // This is equivalent to apply_rvalue< apply_lvalue< apply_volatile< apply_const< U >>>>
        template <typename U> 
        using apply = apply_rvalue< apply_lvalue< apply_volatile< apply_const< U >>>>;
    
        // Implicit conversion between templated types of qualifier_traits with identical cv-ref qualifiers.
        template <typename U>
        requires
        (
            is_const            == qualifier_traits<U>::is_const
         && is_volatile         == qualifier_traits<U>::is_volatile
         && is_lvalue_reference == qualifier_traits<U>::is_lvalue_reference
         && is_rvalue_reference == qualifier_traits<U>::is_rvalue_reference
        )
        constexpr operator qualifier_traits<U>() const noexcept { return {}; }

        template <typename U>
        constexpr auto operator==(qualifier_traits<U>) const noexcept -> bool { return false; }

        template <typename U>
        requires
        (
            is_const            == qualifier_traits<U>::is_const
         && is_volatile         == qualifier_traits<U>::is_volatile
         && is_lvalue_reference == qualifier_traits<U>::is_lvalue_reference
         && is_rvalue_reference == qualifier_traits<U>::is_rvalue_reference
        )
        constexpr auto operator==(qualifier_traits<U>) const noexcept -> bool { return true; }

    }; // struct qualifier_traits

} // namespace lightray::mtp::traits

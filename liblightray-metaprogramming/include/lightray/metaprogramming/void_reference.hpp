#pragma once

#include <type_traits>
#include <utility>

#include <lightray/metaprogramming/traits/qualifier_traits.hpp>

namespace lightray::mtp
{
    /*
     * This class acts as a wrapper over a void pointer masking it as 
     * a reference to void, allowing the programmer to reason about 
     * the lvalueness/rvalueness of the object pointed to by the void pointer.
     *
     * QualifierTraits must be a templated type of qualifier_traits.
     * Additionally, the given qualifier_traits must have a reference qualifier.
     *
     * Unlike most other classes in this library, this class is inherently UNSAFE!
     *
     * Never initialize this class using prvalue or you will get a dangling reference,
     * not temporary lifetime extension.
     *
     * Dangling reference can happen the same way as any regular reference or pointer.
     *
     * Using this class can also violate type safety just as easy as a void pointer.
     * 
     * Use this class carefully and judiciously!
     *
     * Author: P. Lutchanont
     */
    template <typename QualifierTraits>
    struct void_reference;

    template <typename QualT>
    requires traits::qualifier_traits<QualT>::is_reference
    struct void_reference<traits::qualifier_traits<QualT>>
    {
        template <typename>
        friend struct void_reference;

        // the qualifier traits associated with this void reference.
        using qualifier_traits = traits::qualifier_traits<QualT>;

        // the cv-qualified void type according to qualifier_traits.
        using cv_qualified_type = typename qualifier_traits::template apply_cv<void>;

        // pointer to cv-qualified void type.
        // this is also the type of the stored void pointer.
        using pointer_type = cv_qualified_type*;
        
        // applies the cv-ref qualifiers of this void reference to T
        template <typename T>
        using qualified_type = typename qualifier_traits::template apply<T>;

    private:
        pointer_type _ptr;

    public:
        /*
         * Be careful of dangling reference! No warning will be issued if you pass prvalue into
         * the constructor. Temporary lifetime extension does not work here!
         */
        template <typename T>
        requires requires (T&& t)
        {
            [](typename qualifier_traits::template apply<std::remove_cvref_t<T>> u){} (std::forward<T>(t));
        }
        constexpr void_reference(T&& t) noexcept : _ptr(&t) {}

        /*
         * Implicit conversion to other templated void_reference type with equivalent
         * qualifier traits.
         */
        template <typename T>
        requires std::is_convertible_v<traits::qualifier_traits<T>, qualifier_traits>
        constexpr void_reference(const void_reference<traits::qualifier_traits<T>>& other) noexcept
        : _ptr(other._ptr)
        {}

        /*
         * Implicit conversion to other templated void_reference type with equivalent
         * qualifier traits.
         */
        template <typename T>
        requires std::is_convertible_v<traits::qualifier_traits<T>, qualifier_traits>
        constexpr void_reference(void_reference<traits::qualifier_traits<T>>&& other) noexcept
        : _ptr(std::move(other._ptr))
        {}

        /*
         * Implicit conversion to other templated void_reference type with equivalent
         * qualifier traits.
         */
        template <typename T>
        requires std::is_convertible_v<traits::qualifier_traits<T>, qualifier_traits>
        constexpr void_reference& operator=(const void_reference<traits::qualifier_traits<T>>& other) noexcept
        {
            _ptr = other._ptr;
            return *this;
        }

        /*
         * Implicit conversion to other templated void_reference type with equivalent
         * qualifier traits.
         */
        template <typename T>
        requires std::is_convertible_v<traits::qualifier_traits<T>, qualifier_traits>
        constexpr void_reference& operator=(void_reference<traits::qualifier_traits<T>>&& other) noexcept
        {
            return _ptr = std::move(other._ptr);
            return *this;
        }

        // Passing T as qualified type not allowed to avoid confusion.
        template <typename T>
        constexpr auto as() const = delete;


        /*
         * Cast this void reference to the cv-ref qualified version of T. 
         * T must be an unqualified type.
         * This has the same semantic and danger of casting to and from void pointer.
         * Namely, pointer offsets won't be calculated for polymorphic types.
         * No type checking is made, etc.
         */
        template <typename T>
        requires
        (
            !std::is_const_v<T>
         && !std::is_volatile_v<T>
         && !std::is_reference_v<T>
        )
        constexpr auto as() const noexcept -> qualified_type<T>
        {
            using cv_qualified_pointer = typename qualifier_traits::template apply_cv<T>*;
            return static_cast<qualified_type<T>>(*static_cast<cv_qualified_pointer>(_ptr));
        }
    };

    // deduce by perfect forwarding reference.
    template <typename T>
    void_reference(T&& t) -> void_reference<traits::qualifier_traits<T&&>>;

} // namespace lightray::mtp

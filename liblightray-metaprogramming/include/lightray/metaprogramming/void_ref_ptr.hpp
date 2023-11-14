#pragma once

#include <type_traits>

#include <lightray/metaprogramming/cast.hpp>
#include <lightray/metaprogramming/traits/qualifier_traits.hpp>

namespace lightray::mtp
{
    /*
     * A wrapper over void pointer that expresses the reference qualifier of the pointed-to object.
     * This class is created due to the fact that it is impossible to express the reference qualifier
     * from a pointer type, nor it is possible to create a reference-qualified void type.
     *
     * QualifierTraits must be a templated type of qualifier_traits, and the qualifier_traits must be
     * of a reference type.
     *
     * This class is UNSAFE. In particular, the function 'as' does not provide any checks for the
     * validity of the cast, and is just as unsafe as performing static_cast from a void pointer.
     *
     * This class sole purpose is to express the reference qualifier of type void and nothing else.
     *
     * Author: P. Lutchanont
     */
    template <typename QualifierTraits>
    struct void_ref_ptr;

    template <typename QualT>
    requires std::is_reference_v<QualT>
    struct void_ref_ptr<traits::qualifier_traits<QualT>>
    {
        template <typename>
        friend struct void_ref_ptr;

        // Qualifier traits of the referent.
        using qualifier_traits = traits::qualifier_traits<QualT>;

        // Stored pointer type.
        using pointer_type = typename qualifier_traits::template apply_cv<void>*;

    private:
        pointer_type _ptr;

    public:
        // Default pointer initialization.
        void_ref_ptr() = default;

        explicit constexpr void_ref_ptr(pointer_type ptr) noexcept : _ptr(ptr) {}

        // Implicit conversion according to the reference conversion rule based on the qualifier traits.
        template <typename QualU>
        requires std::is_convertible_v<
            typename qualifier_traits::template apply<int>,
            typename traits::qualifier_traits<QualU>::template apply<int>
        >
        constexpr operator void_ref_ptr<traits::qualifier_traits<QualU>>() const noexcept
        {
            return void_ref_ptr<traits::qualifier_traits<QualU>>{_ptr};
        }

        // Explicit qualifier conversion. Still need to respect the cv-ness.
        template <typename QualU>
        requires std::is_convertible_v<
            pointer_type,
            typename void_ref_ptr<traits::qualifier_traits<QualU>>::pointer_type
        >
        explicit constexpr operator void_ref_ptr<traits::qualifier_traits<QualU>>() const noexcept
        {
            return void_ref_ptr<traits::qualifier_traits<QualU>>{_ptr};
        }

        // Cast the void pointer to the given type with qualifiers applied to it.
        // T must be unqualified. This is to avoid confusion.
        template <typename T>
        requires traits::qualifier_traits<T>::is_unqualified
        constexpr auto as() const noexcept -> typename qualifier_traits::template apply<T>
        {
            using pointer_t = typename qualifier_traits::template apply_cv<T>*;
            using ref_t = typename qualifier_traits::template apply<T>;
            return cast<ref_t>(*static_cast<pointer_t>(_ptr));
        }

        // Implicit conversion to the stored pointer.
        constexpr operator pointer_type() const noexcept
        {
            return _ptr;
        }

        // Gets the stored pointer.
        constexpr auto pointer() const noexcept -> pointer_type
        {
            return _ptr;
        }
    };


} // namespace lightray::mtp

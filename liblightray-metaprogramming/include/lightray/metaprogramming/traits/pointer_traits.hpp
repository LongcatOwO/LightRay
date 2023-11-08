#pragma once

namespace lightray::mtp::traits
{
    /*
     * Provides unified traits between regular pointer type
     * and member pointer type.
     *
     * The type of the pointer itself must be unqualified,
     * but the pointed-to type can be qualified.
     * 
     * The traits from this class are only provided
     * if T is either a pointer or pointer-to-member
     *
     * The following traits are provided:
     *
     *  pointer_type: typename: Identical to the given pointer type.
     *
     *  element_type: typename:
     *      The type that is pointed by the pointer type.
     *
     *      For pointer-to-member, this strips away the pointer-to-member
     *      part of the type.
     *
     *      For example,
     *          if T = const int (C::*), then element_type = const int.
     *          if T = void (C::*)(const char*), then element_type = void (const char*).
     *
     *  class_type: typename:
     *      Defined to void for regular pointer type.
     *      Defined to the class type of the pointer-to-member type.
     *
     *      For example:
     *          if T = int*, then class_type = void.
     *          if T = void (S::*)(char), then class_type = S.
     *
     * Author: P. Lutchanont
     */
    template <typename T>
    struct pointer_traits {};

    template <typename T>
    struct pointer_traits<T*>
    {
        using pointer_type = T*;
        using element_type = T;
        using class_type = void;

    }; // struct pointer_traits<T*>

    template <typename T, typename C>
    struct pointer_traits<T C::*>
    {
        using pointer_type = T C::*;
        using element_type = T;
        using class_type = C;

    }; // struct pointer_traits<T C::*>

} // namespace lightray::mtp::traits

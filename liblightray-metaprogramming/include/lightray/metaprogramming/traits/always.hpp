#pragma once

#include <type_traits>

namespace lightray::mtp::traits
{
    /*
     * Utility type trait which always inherit from std::false_type
     * Useful for static assertion in templated contexts.
     *
     * For example:
     *  void f(int);
     *
     *  template <typename T>
     *  void f(T) { static_assert(always_false<T>::value, "f is only allowed for int!"); }
     *
     * The above example prevents any kind of conversion from happening while 
     * giving a helpful error message.
     *
     * Author: P. Lutchanont
     */
    template <typename T>
    struct always_false : std::false_type {};

    /*
     * Aliases to always_false<T>::value.
     */
    template <typename T>
    constexpr bool always_false_v = always_false<T>::value;

    /*
     * Made to be the counterpart of always_false. Not sure of its use case.
     */
    template <typename T>
    struct always_true : std::true_type {};

    /*
     * Aliases to always_true<T>::value.
     */
    template <typename T>
    constexpr bool always_true_v = always_true<T>::value;

} // namespace lightray::mtp::traits

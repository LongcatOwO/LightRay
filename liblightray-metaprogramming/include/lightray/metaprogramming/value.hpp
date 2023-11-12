#pragma once

#include <utility>

#include <lightray/metaprogramming/concepts/functor.hpp>

namespace lightray::mtp
{
    /*
     * An empty meta type holding a compile-time constant as its non-type template parameter.
     * This type is cheaply and trivially (default|copy|move) (constructible|assignable),
     * and should perferrably be passed around by value.
     *
     * This type provides utility methods for operating on its non-type template parameter V.
     */
    template <auto V>
    struct value_t
    {
        /*
         * Aliases to decltype(V).
         */
        using type = decltype(V);

        /*
         * Equals to V.
         */
        static constexpr auto value = V;

        /*
         * Returns V.
         */
        static constexpr auto get() noexcept -> auto { return value; }

        constexpr operator type() const noexcept(noexcept(type{value}))
        {
            return value;
        }

        /*
         * Transform this value_t using fn.
         * Fn must be a functor with the following signature:
         *  template <auto V> [return-type] operator()
         *
         * i.e. the following construct must be valid:
         *  std::forward<Fn>(fn).template operator()<V>();
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  value<123>.transform([]<auto V>{ return V * 3; });
         *  value<3.14>.transform([]<auto V>{  return value<V * 2>; });
         *
         * The result of the invocation of fn is perfect forwarded.
         */
        template <concepts::value_functor<value> Fn>
        static constexpr auto transform(Fn&& fn) noexcept(concepts::nothrow_value_functor<Fn, value>)
        -> decltype(auto) { return std::forward<Fn>(fn).template operator()<value>(); }

        /*
         * Same as transform.
         */
        template <typename Fn>
        static constexpr auto map(Fn&& fn)
        noexcept(noexcept(transform(std::forward<Fn>(fn))))
        -> decltype(auto) { return transform(std::forward<Fn>(fn)); }

    }; // struct value_t

    /*
     * Equals to value_t<V>{}.
     */
    template <auto V>
    constexpr auto value = value_t<V>{};

    using true_t = value_t<true>;
    constexpr auto true_ = value<true>;

    using false_t = value_t<false>;
    constexpr auto false_ = value<false>;

    /*
     * Returns value_t<+V>{}.
     */
    template <auto V> requires requires { typename value_t<+V>; }
    constexpr auto operator+(value_t<V>) noexcept(noexcept(+V))
    -> value_t<+V> { return {}; }

    /*
     * Returns value_t<-V>{}.
     */
    template <auto V> requires requires { typename value_t<-V>; }
    constexpr auto operator-(value_t<V>) noexcept(noexcept(-V))
    -> value_t<-V> { return {}; }

    /*
     * Returns value_t<~V>{}.
     */
    template <auto V> requires requires { typename value_t<~V>; }
    constexpr auto operator~(value_t<V>) noexcept(noexcept(~V))
    -> value_t<~V> { return {}; }

    /*
     * Returns value_t<!V>{}.
     */
    template <auto V> requires requires { typename value_t<!V>; }
    constexpr auto operator!(value_t<V>) noexcept(noexcept(!V))
    -> value_t<!V> { return {}; }

    /*
     * Returns value_t<V + W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V + W>; }
    constexpr auto operator+(value_t<V>, value_t<W>) noexcept(noexcept(V + W))
    -> value_t<V + W> { return {}; }

    /*
     * Returns value_t<V - W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V - W>; }
    constexpr auto operator-(value_t<V>, value_t<W>) noexcept(noexcept(V - W))
    -> value_t<V - W> { return {}; }

    /*
     * Returns value_t<V * W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V * W>; }
    constexpr auto operator*(value_t<V>, value_t<W>) noexcept(noexcept(V * W))
    -> value_t<V * W> { return {}; }

    /*
     * Returns value_t<V / W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V / W>; }
    constexpr auto operator/(value_t<V>, value_t<W>) noexcept(noexcept(V / W))
    -> value_t<V / W> { return {}; }

    /*
     * Returns value_t<V % W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V % W>; }
    constexpr auto operator%(value_t<V>, value_t<W>) noexcept(noexcept(V % W))
    -> value_t<V % W> { return {}; }

    /*
     * Returns value_t<V ^ W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V ^ W>; }
    constexpr auto operator^(value_t<V>, value_t<W>) noexcept(noexcept(V ^ W))
    -> value_t<V ^ W> { return {}; }

    /*
     * Returns value_t<V & W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V & W>; }
    constexpr auto operator&(value_t<V>, value_t<W>) noexcept(noexcept(V & W))
    -> value_t<V & W> { return {}; }

    /*
     * Returns value_t<V | W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V | W>; }
    constexpr auto operator|(value_t<V>, value_t<W>) noexcept(noexcept(V | W))
    -> value_t<V | W> { return {}; }

    /*
     * Returns value_t<(V < W)>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<(V < W)>; }
    constexpr auto operator<(value_t<V>, value_t<W>) noexcept(noexcept(V < W))
    -> value_t<(V < W)> { return {}; }

    /*
     * Returns value_t<(V > W)>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<(V > W)>; }
    constexpr auto operator>(value_t<V>, value_t<W>) noexcept(noexcept(V > W))
    -> value_t<(V > W)> { return {}; }

    /*
     * Returns value_t<V <= W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V <= W>; }
    constexpr auto operator<=(value_t<V>, value_t<W>) noexcept(noexcept(V <= W))
    -> value_t<V <= W> { return {}; }

    /*
     * Returns value_t<V >= W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V >= W>; }
    constexpr auto operator>=(value_t<V>, value_t<W>) noexcept(noexcept(V >= W))
    -> value_t<V >= W> { return {}; }

    /*
     * Returns value_t<V == W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V == W>; }
    constexpr auto operator==(value_t<V>, value_t<W>) noexcept(noexcept(V == W))
    -> value_t<V == W> { return {}; }

    /*
     * Returns value_t<V != W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V != W>; }
    constexpr auto operator!=(value_t<V>, value_t<W>) noexcept(noexcept(V != W))
    -> value_t<V != W> { return {}; }

    /*
     * Returns value_t<V <=> W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V <=> W>; }
    constexpr auto operator<=>(value_t<V>, value_t<W>) noexcept(noexcept(V <=> W))
    -> value_t<V <=> W> { return {}; }

    /*
     * Returns value_t<(V << W)>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<(V << W)>; }
    constexpr auto operator<<(value_t<V>, value_t<W>) noexcept(noexcept(V << W))
    -> value_t<(V << W)> { return {}; }

    /*
     * Returns value_t<(V >> W)>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<(V >> W)>; }
    constexpr auto operator>>(value_t<V>, value_t<W>) noexcept(noexcept(V >> W))
    -> value_t<(V >> W)> { return {}; }

    /*
     * Returns value_t<V && W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V && W>; }
    constexpr auto operator&&(value_t<V>, value_t<W>) noexcept(noexcept(V && W))
    -> value_t<V && W> { return {}; }

    /*
     * Returns value_t<V || W>{}.
     */
    template <auto V, auto W> requires requires { typename value_t<V || W>; }
    constexpr auto operator||(value_t<V>, value_t<W>) noexcept(noexcept(V || W))
    -> value_t<V || W> { return {}; }

} // lightray::mtp

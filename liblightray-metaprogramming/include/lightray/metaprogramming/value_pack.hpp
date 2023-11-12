#pragma once

#include <utility>

#include <lightray/metaprogramming/concepts/functor.hpp>

#include "tags.hpp"
#include "value.hpp"

namespace lightray::mtp
{
    template <std::size_t I, auto V>
    struct indexed_value_t : value_t<V>
    {
        static constexpr auto index = I;
    }; // struct indexed_value_t

    namespace detail
    {
        template <std::size_t I, auto V>
        constexpr indexed_value_t<I, V> get(indexed_value_t<I, V>) noexcept { return {}; }

        template <typename ISeq, auto... Vs>
        struct value_pack_indexer;

        template <std::size_t... Is, auto... Vs>
        struct value_pack_indexer<std::index_sequence<Is...>, Vs...> : indexed_value_t<Is, Vs>... {};

    } // namespace detail

    template <auto... Vs>
    struct value_pack_t;

    template <typename T>
    concept value_pack_type = requires (T&& t)
    {
        []<auto... Vs>(value_pack_t<Vs...> const &){} (t);

    }; // concept value_pack_type

    template <auto... Vs, auto... Ws>
    constexpr auto operator+(value_pack_t<Vs...>, value_pack_t<Ws...>) noexcept -> value_pack_t<Vs..., Ws...>;

    /*
     * An empty meta type holding multiple values as its non-type template parameters.
     * This type is cheaply and trivially (default|copy|move) (constructible|assignable),
     * and should be preferrably be passed around by value.
     *
     * This type provides utility methods for operating on its non-type template parameters Vs.
     */
    template <auto... Vs>
    struct value_pack_t : detail::value_pack_indexer<std::make_index_sequence<sizeof...(Vs)>, Vs...>
    {
        /*
         * Returns the number of values this pack contains (i.e. sizeof...(Vs)).
         */
        static constexpr auto size() noexcept -> std::size_t { return sizeof...(Vs); }

        /*
         * Returns true if this pack does not contain any type, otherwise false.
         */
        static constexpr auto empty() noexcept -> bool { return size() == 0; }

        /*
         * Returns V where V is the I-th type in the non-type template parameters Vs.
         * If wrapped = true, returns value<V> instead.
         */
        template <std::size_t I, bool wrapped = false>
        static constexpr auto get() noexcept -> auto
        {
            if constexpr (wrapped)
                return detail::get<I>(value_pack_t{});
            else
                return detail::get<I>(value_pack_t{}).get();
        }

        /*
         * Find the first value in this pack which satisfies the predicate p.
         *
         * p must be a functor with the following signature:
         *  template <auto V> [return-type] operator()
         *
         * Additionally, p must return a value whose type have a nested constexpr static member data 'value'
         * which is convertible to bool.
         *
         * i.e. the following construct must be valid for all V in Vs:
         * auto result = std::forward<Fn>(fn).template operator()<V>();
         * if constexpr (decltype(result)::value) {...}
         *
         * Given the expressions above, this function returns V where V is the first value in Vs
         * which satisfies the condition:
         *  decltype(result)::value
         *
         * If wrapped = true, returns value<V> instead.
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  // this returns 8
         *  value_pack<1, 3, 5, 7, 8, 9>.find_if([]<auto V>{ return value<V % 2 == 0>; });
         */
        template <bool wrapped = false, typename Predicate>
        requires (... && concepts::value_to_value_functor<Predicate, bool, Vs>)
        static constexpr auto find_if(Predicate&& p) noexcept -> auto
        {
            const auto impl = [&]<auto I>(auto self, value_t<I>)
            {
                if constexpr (I < size())
                {
                    if constexpr (decltype(std::forward<Predicate>(p).template operator()<get<I>()>())::value)
                        return get<I, wrapped>();
                    else
                        return self(self, value<I + 1>);
                }
            };
            return impl(impl, value<0>);
        }

        /*
         * Performs filter operation on each value in Vs using predicate p.
         * 
         * p must be a functor with the following signature:
         *  template <auto V> [return-type] operator()
         *
         * Additionally, p must return a value whose type have a nested constexpr static member data 'value'
         * which is convertible to bool.
         *
         * i.e. the following construct must be valid for all V in Vs:
         *  auto result = std::forward<Fn>(fn).template operator()<V>();
         *  if constexpr (decltype(result)::value) {...}
         *
         * Given the expressions above, this function returns value_pack<Ws...> where Ws is a non-type
         * template parameter pack and each non-type W in Ws satisfies the following condition:
         *  decltype(result)::value
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  // this returns value_pack<0, 3, 6, 9>
         *  value_pack<0, 1, 2, 3, 4, 5, 6, 7, 8, 9>.filter([]<auto V>{ return value<V % 3 == 0>; });
         */
        template <typename Predicate>
        requires (... && concepts::value_to_value_functor<Predicate, bool, Vs>)
        static constexpr auto filter(Predicate&& p) noexcept -> auto
        {
            const auto decide = [&]<auto V>(value_t<V>)
            {
                if constexpr (decltype(std::forward<Predicate>(p).template operator()<V>())::value)
                    return value_pack_t<V>{};
                else
                    return value_pack_t<>{};
            };
            return (value_pack_t<>{} + ... + decide(value<Vs>));
        }

        /*
         * Performs a flattening operation on non-type template parameters Vs.
         *
         *  For each V in Vs, decltype(V) must be a type formed by the type template value_pack_t
         *  Let Vs... = Vs[0], Vs[1], Vs[2], ..., Vs[sizeof...(Vs) - 1]
         *
         * If the above condition is satisfied, then:
         *
         *  Vs[n] = value_pack<Vs_Params[n]...> for each n in 0, 1, 2, ..., sizeof...(Vs) - 1
         *
         * Then, the result of this function is equivalent to:
         *
         *  value_pack<Vs_Params[0]..., Vs_Params[1]..., ..., Vs_Params[sizeof...(Vs) - 1]...>.
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  // this returns value_pack<0, 1, 2, 3, 4, 5>
         *  value_pack<value_pack<0, 1>, value_pack<2>, value_pack<3, 4, 5>>.flatten()
         *
         *  // this returns value_pack<0, 1, value_pack<2, 3>, value_pack<4>>
         *  value_pack<value_pack<0, 1>, value_pack<value_pack<2, 3>, value_pack<4>>>.flatten()
         */
        static constexpr auto flatten() noexcept -> auto
        requires (... && value_pack_type<decltype(Vs)>)
        {
            return (value_pack_t<>{} + ... + Vs);
        }

        /*
         * Calls fn on each value in Vs.
         *
         * fn must be a functor with the following signature:
         *  template <auto V> [return-type] operator()
         *
         * i.e. the following construct must be valid for all V in Vs:
         *  std::forward<Fn>(fn).template operator()<V>();
         *
         * This function is intended to be used with C++20's template lambda.
         *
         * For example:
         *  value_pack<'c', 'a', 't'>.for_each([]<auto V>{ std::cout << V; });
         */
        template <typename Fn> requires (... && concepts::value_functor<Fn, Vs>)
        static constexpr auto for_each(Fn&& fn) noexcept((... && concepts::nothrow_value_functor<Fn, Vs>))
        -> void
        {
            (std::forward<Fn>(fn).template operator()<Vs>(), ...);
        }

        /*
         * Calls fn on the non-type template parameter pack Vs.
         *
         * fn must be a functor with the following signature:
         *  template <auto... Vs> [return-type] operator()
         *
         * i.e. the following construct must be valid:
         *  std::forward<Fn>(fn).template operator()<Vs...>();
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  value_pack<'c', 'a', 't'>.apply([]<auto... Vs>{ return std::string({Vs...}); });
         *
         * The result of the invocation of fn is perfect forwarded.
         */
        template <concepts::value_functor<Vs...> Fn>
        static constexpr auto apply(Fn&& fn) noexcept(concepts::nothrow_value_functor<Fn, Vs...>)
        -> decltype(auto)
        {
            return std::forward<Fn>(fn).template operator()<Vs...>();
        }

        /*
         * Use fn to transform each value in Vs into a new compile-time value.
         *
         * fn must be a functor with the following signature:
         *  template <auto V> value_t<[result-value]> operator()
         *
         * i.e. the following construct must be valid for all V in Vs:
         *  constexpr auto result_value = decltype(std::forward<Fn>(fn).template operator()<V>())::value;
         *  constexpr auto v = value<result_value>;
         *
         * Let result_value[n] = decltype(std::forward<Fn>(fn).template operator()<Vs[n]>())::value
         * for Vs[0], Vs[1], Vs[2], Vs[sizeof...(Vs) - 1] in Vs
         *
         * Then, the result of calling this function is:
         *  value_pack<result_value[0], result_value[1], result_value[2], ... result_value[sizeof...(Vs) - 1]>
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  // this returns value_pack<2, 4, 6, 8, 10>
         *  value_pack<1, 2, 3, 4, 5>.transform([]<auto V>{ return value<V * 2>; });
         */
        template <typename Fn> requires (... && concepts::value_to_value_functor<Fn, auto_t, Vs>)
        static constexpr auto transform(Fn&& fn) noexcept -> auto
        {
            return value_pack_t<decltype(std::forward<Fn>(fn).template operator()<Vs>())::value...>{};
        }

        /*
         * Same as calling transform.
         */
        template <typename Fn>
        static constexpr auto map(Fn&& fn) noexcept -> auto
        {
            return transform(std::forward<Fn>(fn));
        }

    }; // struct value_pack_t

    /*
     * Concatenate the non-type template parameters of both operands together.
     */
    template <auto... Vs, auto... Ws>
    constexpr auto operator+(value_pack_t<Vs...>, value_pack_t<Ws...>) noexcept 
    -> value_pack_t<Vs..., Ws...> { return {}; }

    /*
     * Equals to value_pack_t<Vs...>{}.
     */
    template <auto... Vs>
    constexpr auto value_pack = value_pack_t<Vs...>{};

    /*
     * Equals to value_pack<0, 1, 2, ..., N - 1>.
     */
    template <std::size_t N>
    constexpr auto make_index_sequence = []<std::size_t... Is>(std::index_sequence<Is...>)
    {
        return value_pack<Is...>;
    } (std::make_index_sequence<N>{});

} // namespace lightray::mtp

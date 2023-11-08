#pragma once

#include <cstddef>
#include <utility>

#include "type.hpp"
#include "value.hpp"

namespace lightray::mtp
{
    template <std::size_t I, typename T>
    struct indexed_type_t : type_t<T>
    {
        static constexpr auto index = I;
    }; // struct indexed_type_t

    namespace detail
    {
        template <std::size_t I, typename T>
        constexpr auto get(indexed_type_t<I, T>) noexcept -> indexed_type_t<I, T> { return {}; }

        template <typename ISeq, typename... Ts>
        struct type_pack_indexer;

        template <std::size_t... Is, typename... Ts>
        struct type_pack_indexer<std::index_sequence<Is...>, Ts...> : indexed_type_t<Is, Ts>... {};

    } // namespace detail

    template <typename... Ts>
    struct type_pack_t;

    template <typename T>
    concept type_pack_type = requires (T&& t)
    {
        []<typename... Ts>(type_pack_t<Ts...> const&){} (t);

    }; // concept type_pack_type

    template <typename... Ts, typename... Us>
    constexpr auto operator+(type_pack_t<Ts...>, type_pack_t<Us...>) noexcept -> type_pack_t<Ts..., Us...>;

    /*
     * An empty meta type holding multiple types as its template parameters.
     * This type is cheaply and trivially (default|copy|move) (constructible|assignable),
     * and should preferrably be passed around by value.
     *
     * This type provides utility methods for operating on its template parameters Ts.
     */
    template <typename... Ts>
    struct type_pack_t : detail::type_pack_indexer<std::make_index_sequence<sizeof...(Ts)>, Ts...>
    {
        /*
         * Returns the number of types this pack contains (i.e. sizeof...(Ts)).
         */
        static constexpr auto size() noexcept -> std::size_t { return sizeof...(Ts); }

        /*
         * Returns true if this pack does not contain any type, otherwise false.
         */
        static constexpr auto empty() noexcept -> bool { return size() == 0; }

        /*
         * Returns type<T> where T is the I-th type in the template parameters Ts.
         */
        template <std::size_t I>
        static constexpr auto get() noexcept -> auto { return detail::get<I>(type_pack_t{}); }

        /*
         * Finds the first type in this pack which satisfies the predicate p.
         *
         * p must be a functor with the following signature:
         *  template <typename T> [return-type] operator()
         *
         * Additionally, p must return a value whose type have a nested constexpr static member data
         * 'value' which is convertible to bool.
         *
         * i.e. the following construct must be valid for all T in Ts:
         *  auto result = std::forward<Fn>(fn).template operator()<T>();
         *  if constexpr (decltype(result)::value) {...}
         *
         * Given the expressions above, this function returns type_t<T> where T is the first type in Ts
         * which satisfies the condition:
         *  decltype(result)::value
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  // this returns type_t<int>
         *  type_pack<float, int, double>.find_if([]<typename T>{ return std::is_integral<T>{}; });
         */
        template <typename Predicate>
        static constexpr auto find_if(Predicate&& p) noexcept -> auto
        {
            const auto impl = [&]<auto I>(auto self, value_t<I>)
            {
                if constexpr (I < size())
                {
                    using T = typename decltype(get<I>())::type;
                    if constexpr (decltype(std::forward<Predicate>(p).template operator()<T>())::value)
                        return type<T>;
                    else
                        return self(self, value<I + 1>);
                }
            };
            return impl(impl, value<0>);
        }

        /*
         * Performs filter operation on each type in Ts using predicate p.
         *
         * p must be a functor with the following signature:
         *  template <typename T> [return-type] operator()
         *
         * Additionally, p must return a value whose type have a nested constexpr static member data 'value' 
         * which is convertible to bool.
         *
         * i.e. the following construct must be valid for all T in Ts:
         *  auto result = std::forward<Fn>(fn).template operator()<T>();
         *  if constexpr (decltype(result)::value) {...}
         *
         * Given the expressions above, this function returns type_pack<Us...> where Us is a 
         * typename parameter pack and each type U in Us satisfies the following condition:
         *  decltype(result)::value
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  // this returns type_pack<float, double>
         *  type_pack<float, int, double, char>.filter([]<typename T>{ return std::is_floating_point<T>{}; });
         */
        template <typename Predicate>
        static constexpr auto filter(Predicate&& p) noexcept -> auto
        {
            const auto decide = [&]<typename T>(type_t<T> type)
            {
                if constexpr (decltype(std::forward<Predicate>(p).template operator()<T>())::value)
                    return type_pack_t<T>{};
                else
                    return type_pack_t<>{};
            };
            return (type_pack_t<>{} + ... + decide(type<Ts>));
        }

        /*
         * Performs a flattening operation on template parameters Ts.
         *
         *  For each T in Ts, T must be a type formed by the type template type_pack_t.
         *  Let Ts... = Ts[0], Ts[1], Ts[2], ..., Ts[sizeof...(Ts) - 1]
         *
         * If the above condition is satisfied, then:
         *
         *  Ts[n] = type_pack_t<Ts_Params[n]...> for each n in 0, 1, 2, ..., sizeof...(Ts) - 1
         *
         * Then, the result of this function is equivalent to:
         *
         *  type_pack<Ts_Params[0]..., Ts_Params[1]..., ..., Ts_Params[sizeof...(Ts) - 1]...>.
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  // this returns type_pack<void*, char, int, long, unsigned>.
         *  type_pack<type_pack_t<void*, char>, type_pack_t<int>, type_pack_t<long, unsigned>>.flatten()
         *
         *  // this returns type_pack<type_pack_t<int, float>, long, double, char*>
         *  type_pack<type_pack_t<type_pack_t<int, float>, long>, type_pack_t<double, char*>>.flatten()
         */
        static constexpr auto flatten() noexcept -> auto 
        requires (... && type_pack_type<Ts>)
        {
            return (type_pack_t<>{} + ... + Ts{});
        }

        /*
         * Calls fn on each type in Ts.
         * 
         * fn must be a functor with the following signature:
         *  template <typename T> [return-type] operator()
         *
         * i.e. the following construct must be valid for all T in Ts:
         *  std::forward<Fn>(fn).template operator()<T>();
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  type_pack<int, float, char>.for_each([]<typename T>{ std::cout << typeid(T).name() << '\n'; });
         */
        template <typename Fn>
        static constexpr auto for_each(Fn&& fn)
        noexcept(noexcept((std::forward<Fn>(fn).template operator()<Ts>(), ...)))
        -> void
        {
            (std::forward<Fn>(fn).template operator()<Ts>(), ...);
        }

        /*
         * Calls fn on the parameter pack Ts.
         *
         * fn must be a functor with the following signature:
         *  template <typename... Ts> [return-type] operator()
         *
         * i.e. the following construct must be valid:
         *  std::forward<Fn>(fn).template operator()<Ts...>();
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  type_pack<char*, int, float>.apply([]<typename... Ts>{ return std::tuple<Ts...>{}; });
         *
         * The result of the invocation of fn is perfect forwarded.
         */
        template <typename Fn>
        static constexpr auto apply(Fn&& fn)
        noexcept(noexcept(std::forward<Fn>(fn).template operator()<Ts...>()))
        -> decltype(auto)
        {
            return std::forward<Fn>(fn).template operator()<Ts...>();
        }

        /*
         * Use fn to transform each type in Ts into a new type.
         *
         * fn must be a functor with the following signature:
         *  template <typename T> type_t<[result-type]> operator()
         *
         * i.e. the following construct must be valid for all T in Ts:
         *  using result_type = typename decltype(std::forward<Fn>(fn).template operator()<T>())::type;
         *
         * Let result_type[n] = typename decltype(std::forward<Fn>(fn).template operator()<Ts[n]>())::type
         * for Ts[0], Ts[1], Ts[2], Ts[sizeof...(Ts) - 1] in Ts
         *
         * Then, the result of calling this function is:
         *  type_pack<result_type[0], result_type[1], result_type[2], ... result_type[sizeof...(Ts) - 1]>
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  // this returns type_pack<const int, const long, const char>
         *  type_pack<int, long, char>.transform([]<typename T>{ return type<const T>; });
         */
        template <typename Fn>
        static constexpr auto transform(Fn&& fn) noexcept -> auto
        {
            return type_pack_t<typename decltype(std::forward<Fn>(fn).template operator()<Ts>())::type...>{};
        }

        /*
         * Same as calling transform.
         */
        template <typename Fn>
        static constexpr auto map(Fn&& fn) noexcept -> auto 
        {
            return transform(std::forward<Fn>(fn));
        }

    }; // struct type_pack_t

    /*
     * Concatenate the type parameters of both operands together.
     */
    template <typename... Ts, typename... Us>
    constexpr auto operator+(type_pack_t<Ts...>, type_pack_t<Us...>) noexcept 
    -> type_pack_t<Ts..., Us...> { return {}; }

    /*
     * Equals to type_pack_t<Ts...>{}.
     */
    template <typename... Ts>
    constexpr auto type_pack = type_pack_t<Ts...>{};

} // namespace lightray::mtp

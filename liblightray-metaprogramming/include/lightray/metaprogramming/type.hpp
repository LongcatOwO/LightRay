#pragma once

#include <concepts>
#include <utility>

#include <lightray/metaprogramming/concepts/functor.hpp>

namespace lightray::mtp
{
    /*
     * An empty meta type holding a type as its template parameter.
     * This type is cheaply and trivially (default|copy|move) (constructible|assignable),
     * and should preferrably be passed around by value.
     *
     * This type provides utility methods for operating on its template parameter T.
     */
    template <typename T>
    struct type_t
    {
        /*
         * Aliases to T.
         */
        using type = T;

        /*
         * Constructs an instance of T with the given arguments perfect forwarded.
         */
        template <typename... Args>
        requires std::constructible_from<type, Args&&...>
        static constexpr auto construct(Args&&... args)
        noexcept(noexcept(type(std::forward<Args>(args)...)))
        -> auto { return type(std::forward<Args>(args)...); }

        /*
         * Same as calling construct.
         */
        template <typename... Args>
        constexpr auto operator()(Args&&... args) const
        noexcept(noexcept(construct(std::forward<Args>(args)...)))
        -> decltype(auto) { return construct(std::forward<Args>(args)...); }

        /*
         * Transform this type_t using fn.
         *
         * fn must be a functor with the following signature:
         *  template <typename T> [return-type] operator()
         *
         * i.e. the following construct must be valid:
         *  std::forward<Fn>(fn).template operator()<T>();
         *
         * This function is intended to be used with C++20's template lambda.
         * For example:
         *  type<int>.transform([]<typename T>{ return std::is_integral_v<T>; });
         *  type<std::vector<int>>.transform([]<typename T>{ return type<typename T::iterator>; });
         *
         * The result of the invocation of fn is perfect forwarded.
         */
        template <concepts::type_functor<type> Fn>
        static constexpr auto transform(Fn&& fn) noexcept(concepts::nothrow_type_functor<Fn, type>)
        -> decltype(auto) { return std::forward<Fn>(fn).template operator()<type>(); }

        /*
         * Same as transform.
         */
        template <typename Fn>
        static constexpr auto map(Fn&& fn)
        noexcept(noexcept(transform(std::forward<Fn>(fn))))
        -> decltype(auto) { return transform(std::forward<Fn>(fn)); }

    }; // struct type_t

    /*
     * Equals to type_t<T>{}.
     */
    template <typename T>
    constexpr auto type = type_t<T>{};

} // namespace lightray::mtp

#ifndef _LIGHTRAY_TRAITS_TYPE_HPP_
#define _LIGHTRAY_TRAITS_TYPE_HPP_


#include <utility>


namespace lightray::traits
{
    template <typename T>
    struct type_ { using type = T; };

    template <typename ...Ts>
    struct type_sequence {};

    template <typename T>
    struct is_type_sequence 
    { static constexpr bool value = false; };

    template <typename ...Ts>
    struct is_type_sequence<type_sequence<Ts...>> 
    { static constexpr bool value = true; };

    template <typename T>
    constexpr bool is_type_sequence_v = is_type_sequence<T>::value;


    template <typename ...Ts, typename ...Us>
    constexpr type_sequence<Ts..., Us...> operator+(type_sequence<Ts...>, type_sequence<Us...>) noexcept
    { return {}; }


    template <typename TypeSequence, auto Predicate, typename = void>
    struct type_sequence_filter {};

    template <typename ...Ts, auto Predicate, typename Dummy>
    struct type_sequence_filter<type_sequence<Ts...>, Predicate, Dummy>
    {
        using type = decltype([](auto ...types)
        {
            if constexpr (sizeof...(types) == 0) 
            {
                return type_sequence<>{};
            }
            else
            {
                constexpr auto recurse = 
                    []<typename ...Result, typename Head, typename ...Rest>
                    (auto &&self, type_sequence<Result...>, type_<Head>, type_<Rest>...)
                    {
                        constexpr auto result = []()
                        {
                            if constexpr (Predicate(type_<Head>{}))
                                return type_sequence<Result..., Head>{};
                            else
                                return type_sequence<Result...>{};
                        }();

                        if constexpr (sizeof...(Rest) == 0)
                            return result;
                        else
                            return self(self, result, type_<Rest>{}...);
                    };
                
                return recurse(recurse, type_sequence<>{}, types...);
            }
        }(type_<Ts>{}...));
    };

    template <typename TypeSequence, auto Predicate, typename Dummy = void>
    using type_sequence_filter_t = 
        typename type_sequence_filter<TypeSequence, Predicate, Dummy>::type;


    template <typename TypeSequence, template <typename ...> class Tmpl, typename = void>
    struct type_sequence_apply {};

    template <typename ...Ts, template <typename ...> class Tmpl, typename Dummy>
    struct type_sequence_apply<type_sequence<Ts...>, Tmpl, Dummy>
    {
        using type = Tmpl<Ts...>;
    };

    template <typename TypeSequence, template <typename ...> class Tmpl, typename Dummy = void>
    using type_sequence_apply_t = 
        typename type_sequence_apply<TypeSequence, Tmpl, Dummy>::type;


    template <typename ...Ts, typename Fn>
    constexpr void type_sequence_for_each(type_sequence<Ts...>, Fn &&fn)
    noexcept(noexcept((std::forward<Fn>(fn)(type_<Ts>{}), ...)))
    {
        (std::forward<Fn>(fn)(type_<Ts>{}), ...);
    }


    template <typename TypeSequence1, typename TypeSequence2, typename = void>
    struct type_sequence_concat {};

    template <typename ...Lhs, typename ...Rhs, typename Dummy>
    struct type_sequence_concat<type_sequence<Lhs...>, type_sequence<Rhs...>, Dummy>
    {
        using type = type_sequence<Lhs..., Rhs...>;
    };

    template <typename TypeSequence1, typename TypeSequence2, typename Dummy = void>
    using type_sequence_concat_t = typename type_sequence_concat<TypeSequence1, TypeSequence2, Dummy>::type;


    template <typename TypeSequence, typename = void>
    struct type_sequence_flatten {};

    template <typename ...TypeSequences, typename Dummy>
    requires (... && is_type_sequence_v<TypeSequences>)
    struct type_sequence_flatten<type_sequence<TypeSequences...>, Dummy>
    {
        using type = decltype((type_sequence<>{} + ... + TypeSequences{}));
    };

    template <typename TypeSequence, typename Dummy = void>
    using type_sequence_flatten_t = typename type_sequence_flatten<TypeSequence, Dummy>::type;


    template <typename ...Ts>
    struct fluent_type_sequence;
    
    template <typename TypeSequence, typename = void>
    struct type_sequence_fluent {};

    template <typename ...Ts, typename Dummy>
    struct type_sequence_fluent<type_sequence<Ts...>, Dummy>
    {
        using type = fluent_type_sequence<Ts...>;
    };

    template <typename TypeSequence, typename Dummy = void>
    using type_sequence_fluent_t = 
        typename type_sequence_fluent<TypeSequence, Dummy>::type;


    template <typename ...Ts>
    struct fluent_type_sequence
    {
        using type = type_sequence<Ts...>;
        
        template <auto Predicate, typename Dummy = void>
        using filter = type_sequence_fluent_t<
            type_sequence_filter_t<type, Predicate, Dummy>, Dummy
        >;

        template <template <typename ...> class Tmpl, typename Dummy = void>
        using apply = type_sequence_apply_t<type, Tmpl, Dummy>;

        template <typename TypeSequence, typename Dummy = void>
        using concat = type_sequence_fluent_t<
            type_sequence_concat_t<type, TypeSequence, Dummy>
        >;

        template <typename Dummy = void>
        using flatten = type_sequence_fluent_t<type_sequence_flatten_t<type, Dummy>>;
        

        template <typename Fn>
        static constexpr void for_each(Fn &&fn)
        noexcept(noexcept(type_sequence_for_each(type{}, std::forward<Fn>(fn))))
        {
            type_sequence_for_each(type{}, std::forward<Fn>(fn));
        }
    };
}


#endif

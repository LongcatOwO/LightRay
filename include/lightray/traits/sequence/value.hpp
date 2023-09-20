#ifndef _LIGHTRAY_TRAITS_SEQUENCE_VALUE_HPP_
#define _LIGHTRAY_TRAITS_SEQUENCE_VALUE_HPP_


#include <utility>

#include <lightray/traits/sequence/type.hpp>


namespace lightray::traits
{
    template <auto V>
    struct value_
    {
        static constexpr auto value = V;
        using type = decltype(V);
    };

    template <auto ...Vs>
    struct value_sequence {};

    template <typename T>
    struct is_value_sequence
    { static constexpr bool value = false; };
    
    template <auto ...Vs>
    struct is_value_sequence<value_sequence<Vs...>>
    { static constexpr bool value = true; };

    template <typename T>
    constexpr bool is_value_sequence_v = is_value_sequence<T>::value;


    template <auto ...Vs, auto ...Ws>
    constexpr value_sequence<Vs..., Ws...> operator+(value_sequence<Vs...>, value_sequence<Ws...>) noexcept
    { return {}; }


    template <typename ValueSequence, auto Predicate, typename = void>
    struct value_sequence_filter {};

    template <auto ...Vs, auto Predicate, typename Dummy>
    struct value_sequence_filter<value_sequence<Vs...>, Predicate, Dummy>
    {
        using type = decltype([]
            {
                constexpr auto decide = []<auto V>(value_<V> value)
                {
                    if constexpr (Predicate(value))
                        return value_sequence<V>{};
                    else
                        return value_sequence<>{};
                };

                return (value_sequence<>{} + ... + decide(value_<Vs>{}));
            }());
    };

    template <typename ValueSequence, auto Predicate, typename Dummy = void>
    using value_sequence_filter_t = 
        typename value_sequence_filter<ValueSequence, Predicate, Dummy>::type;


    template <typename ValueSequence, template <auto ...> class Tmpl, typename = void>
    struct value_sequence_apply {};

    template <auto ...Vs, template <auto ...> class Tmpl, typename Dummy>
    struct value_sequence_apply<value_sequence<Vs...>, Tmpl, Dummy>
    {
        using type = Tmpl<Vs...>;
    };

    template <typename ValueSequence, template <auto ...> class Tmpl, typename Dummy = void>
    using value_sequence_apply_t =
        typename value_sequence_apply<ValueSequence, Tmpl, Dummy>::type;


    template <auto ...Vs, typename Fn>
    constexpr void value_sequence_for_each(value_sequence<Vs...>, Fn &&fn)
    noexcept(noexcept((std::forward<Fn>(fn)(value_<Vs>{}), ...)))
    {
        (std::forward<Fn>(fn)(value_<Vs>{}), ...);
    }


    template <typename ValueSequence1, typename ValueSequence2, typename = void>
    struct value_sequence_concat {};

    template <auto ...Lhs, auto ...Rhs, typename Dummy>
    struct value_sequence_concat<value_sequence<Lhs...>, value_sequence<Rhs...>, Dummy>
    {
        using type = value_sequence<Lhs..., Rhs...>;
    };

    template <typename ValueSequence1, typename ValueSequence2, typename Dummy = void>
    using value_sequence_concat_t = 
        typename value_sequence_concat<ValueSequence1, ValueSequence2, Dummy>::type;


    template <typename ValueSequence, typename = void>
    struct value_to_type_sequence {};

    template <auto ...Vs, typename Dummy>
    struct value_to_type_sequence<value_sequence<Vs...>, Dummy>
    {
        using type = type_sequence<value_<Vs>...>;
    };

    template <typename ValueSequence, typename Dummy = void>
    using value_to_type_sequence_t =
        typename value_to_type_sequence<ValueSequence, Dummy>::type;


    template <typename TypeSequence, typename = void>
    struct type_to_value_sequence {};

    template <auto ...Vs, typename Dummy>
    struct type_to_value_sequence<type_sequence<value_<Vs>...>, Dummy>
    {
        using type = value_sequence<Vs...>;
    };

    template <typename TypeSequence, typename Dummy = void>
    using type_to_value_sequence_t = 
        typename type_to_value_sequence<TypeSequence, Dummy>::type;
}


#endif

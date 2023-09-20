#ifndef _LIGHTRAY_TRAITS_SEQUENCE_FLUENT_HPP_
#define _LIGHTRAY_TRAITS_SEQUENCE_FLUENT_HPP_


#include <lightray/traits/sequence/type.hpp>
#include <lightray/traits/sequence/value.hpp>

namespace lightray::traits
{
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




    template <auto ...Vs>
    struct fluent_value_sequence;

    template <typename ValueSequence, typename = void>
    struct value_sequence_fluent {};

    template <auto ...Vs, typename Dummy>
    struct value_sequence_fluent<value_sequence<Vs...>, Dummy>
    {
        using type = fluent_value_sequence<Vs...>;
    };

    template <typename ValueSequence, typename Dummy = void>
    using value_sequence_fluent_t = 
        typename value_sequence_fluent<ValueSequence, Dummy>::type;




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
        
        template <typename Dummy = void>
        using to_value = value_sequence_fluent_t<type_to_value_sequence_t<type, Dummy>, Dummy>;

        template <typename Fn>
        static constexpr void for_each(Fn &&fn)
        noexcept(noexcept(type_sequence_for_each(type{}, std::forward<Fn>(fn))))
        {
            type_sequence_for_each(type{}, std::forward<Fn>(fn));
        }
    };





    template <auto ...Vs>
    struct fluent_value_sequence
    {
        using type = value_sequence<Vs...>;

        template <auto Predicate, typename Dummy = void>
        using filter = value_sequence_fluent_t<
                value_sequence_filter_t<type, Predicate, Dummy>, Dummy
            >;

        template <template <auto ...> class Tmpl, typename Dummy = void>
        using apply = value_sequence_apply_t<type, Tmpl, Dummy>;

        template <typename ValueSequence, typename Dummy = void>
        using concat = value_sequence_fluent_t<
                value_sequence_concat_t<type, ValueSequence>, Dummy
            >;

        template <typename Dummy = void>
        using to_type = type_sequence_fluent_t<value_to_type_sequence_t<type, Dummy>, Dummy>;

        template <typename Fn>
        static constexpr void for_each(Fn &&fn)
        noexcept(noexcept(value_sequence_for_each(type{}, std::forward<Fn>(fn))))
        {
            value_sequence_for_each(type{}, std::forward<Fn>(fn));
        }
    };
}


#endif

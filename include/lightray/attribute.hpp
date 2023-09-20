#ifndef _LIGHTRAY_ATTRIBUTE_HPP_
#define _LIGHTRAY_ATTRIBUTE_HPP_


#include <concepts>
#include <type_traits>
#include <utility>

#include <lightray/coredef.hpp>
#include <lightray/traits.hpp>


namespace lightray
{
    template <category Category>
    struct attribute 
    {
        static constexpr category category = Category;
    };

    template <category Category>
    constexpr attribute<Category> check_is_attribute(attribute<Category> const &) 
    { 
        static_assert(
            traits::always_false<attribute<Category>>,
            "check_is_attribute cannot be used in evaluated context!"
        );
    }

    template <typename T>
    constexpr bool is_attribute = requires (T &&t)
    { 
        check_is_attribute(std::declval<T>()); 
    };

    template <typename T>
    concept attribute_type = is_attribute<T> && requires (T &&t)
    {
        { T::category } -> std::convertible_to<category>;
        requires T::category == decltype(check_is_attribute(t))::category;
    };

    template <attribute_type auto ...Attributes>
    struct attribute_sequence
    {
        using values = traits::value_sequence<Attributes...>;

        template <auto Predicate, typename Dummy = void>
        using filter = typename traits::fluent_value_sequence<Attributes...>
            ::template filter<[]<auto Attrib>(traits::value_<Attrib>)
            {
                return Predicate(Attrib);
            }, Dummy>::template apply<attribute_sequence, Dummy>;

    private:
        template <typename TypeSequence, typename = void>
        struct filter_by_type_impl {};

        template <typename ...Ts, typename Dummy>
        struct filter_by_type_impl<traits::type_sequence<Ts...>, Dummy>
        {
            using type = typename traits::fluent_value_sequence<Attributes...>
                ::template filter<[]<auto Attrib>(traits::value_<Attrib>)
                {
                    return (... || std::is_base_of_v<Ts, decltype(Attrib)>);
                }, Dummy>::template apply<attribute_sequence, Dummy>;
        };

        template <typename TypeSequence, typename = void>
        struct filter_by_type_exact_impl {};

        template <typename ...Ts, typename Dummy>
        struct filter_by_type_exact_impl<traits::type_sequence<Ts...>, Dummy>
        {
            using type = typename traits::fluent_value_sequence<Attributes...>
                ::template filter<[]<auto Attrib>(traits::value_<Attrib>)
                {
                    return (... || std::is_same_v<Ts, decltype(Attrib)>);
                }, Dummy>::template apply<attribute_sequence, Dummy>;
        };

    public:
        template <typename ...Ts>
        using filter_by_type = 
            typename filter_by_type_impl<traits::type_sequence<Ts...>>::type;

        template <typename ...Ts>
        using filter_by_type_exact = 
            typename filter_by_type_exact_impl<traits::type_sequence<Ts...>>::type;

        static constexpr bool is_valid_for(category c) noexcept
        {
            return (... && Attributes.category.is_superset_of(c));
        }
    };
}


#endif

#ifndef _LIGHTRAY_REFLECTION_MEMBER_TUPLE_HPP_
#define _LIGHTRAY_REFLECTION_MEMBER_TUPLE_HPP_


#include <concepts>
#include <cstddef>
#include <type_traits>


#include <lightray/coredef.hpp>
#include <lightray/fixed_string.hpp>
#include <lightray/traits.hpp>


namespace lightray
{
    template <typename Member, std::size_t Index>
    struct member_index_leaf { using member = Member; };

    template <typename Member, fixed_string Name>
    struct member_name_leaf { using member = Member; };

    template <typename IndexSequence, typename ...Members>
    struct member_tuple_impl {};

    template <std::size_t ...Is, typename ...Members>
    struct member_tuple_impl<std::index_sequence<Is...>, Members...> :
        member_index_leaf<Members, Is>..., 
        member_name_leaf<Members, Members::name>...
    {};

    template <std::size_t Index, typename Member>
    typename member_index_leaf<Member, Index>::member 
    member_tuple_get_leaf(member_index_leaf<Member, Index>)
    {
        static_assert(
            traits::always_false<Member>, 
            "member_tuple_get_leaf cannot be used in evaluated context!"
        );
    }

    template <fixed_string Name, typename Member>
    typename member_name_leaf<Member, Name>::member
    member_tuple_get_leaf(member_name_leaf<Member, Name>)
    {
        static_assert(
            traits::always_false<Member>,
            "member_tuple_get_leaf cannot be used in evaluated context!"
        );
    }

    template <typename ...Members>
    struct member_tuple
    {
    private:
        using impl = member_tuple_impl<std::make_index_sequence<sizeof...(Members)>, Members...>;

    public:
        template <auto Id>
        requires std::convertible_to<decltype(Id), std::size_t>
              || fixed_string_type<decltype(Id)>
        using get = decltype(member_tuple_get_leaf<Id>(impl{}));

        template <fixed_string Name>
        using get_by_name = get<Name>;

        template <auto Predicate, typename Dummy = void>
        using filter = typename traits::fluent_type_sequence<Members...>
            ::template filter<Predicate, Dummy>::template apply<member_tuple, Dummy>;

        template <category Filter, typename Dummy = void>
        using filter_by_category = typename filter<
                []<typename Member>(traits::type_<Member>)
                { return static_cast<bool>(Member::category & Filter); }, Dummy
            >::template apply<member_tuple, Dummy>;

        template <typename Fn>
        static constexpr void for_each(Fn &&fn)
        noexcept(noexcept(
            traits::fluent_type_sequence<Members...>::for_each(
                [fn_ptr = &fn]<typename Member>(traits::type_<Member>)
                { return std::forward<Fn>(*fn_ptr)(Member{}); }
            )
        ))
        {
            traits::fluent_type_sequence<Members...>::for_each(
                [&]<typename Member>(traits::type_<Member>)
                { return std::forward<Fn>(fn)(Member{}); }
            );
        }
    };
}


#endif

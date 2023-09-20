#ifndef _LIGHTRAY_REFLECTION_TYPE_INFO_HPP_
#define _LIGHTRAY_REFLECTION_TYPE_INFO_HPP_


#include <cstddef>
#include <type_traits>

#include <lightray/attribute.hpp>
#include <lightray/coredef.hpp>
#include <lightray/reflection/member_tuple.hpp>


namespace lightray
{
    template <reflected T, typename IndexSequence>
    struct get_member_metas_impl {};

    template <reflected T, std::size_t ...Is>
    struct get_member_metas_impl<T, std::index_sequence<Is...>>
    {
        using type = member_tuple<typename meta<T>::template member<Is>...>;
    };

    template <reflected T>
    using get_member_metas = typename get_member_metas_impl<
            T, std::make_index_sequence<meta<T>::member_count>
        >::type;




    template <reflected T, fixed_string Name>
    struct member_info_base : get_member_metas<T>::template get<Name> {};


    template <reflected T, fixed_string Name>
    struct member_info_add_attributes : virtual member_info_base<T, Name>
    { 
        using attributes = attribute_sequence<>; 

    private:
        using base = member_info_base<T, Name>;
        static_assert(attributes::is_valid_for(base::category));
    };

    template <reflected T, fixed_string Name>
    requires requires { typename member_info_base<T, Name>::attributes; }
    struct member_info_add_attributes<T, Name> : virtual member_info_base<T, Name> 
    {
    private:
        using base = member_info_base<T, Name>;
        static_assert(base::attributes::is_valid_for(base::category));
    };


    template <reflected T, fixed_string Name>
    struct member_info_add_bases : virtual member_info_base<T, Name>
    { using bases = traits::type_sequence<>; };

    template <reflected T, fixed_string Name>
    requires requires { typename member_info_base<T, Name>::bases; }
    struct member_info_add_bases<T, Name> : virtual member_info_base<T, Name> {};


    template <reflected T, fixed_string Name>
    struct member_info : 
        virtual member_info_base<T, Name>,
        member_info_add_attributes<T, Name> {};

    template <reflected T, fixed_string Name>
    requires (member_info_base<T, Name>::category == category::variable)
    struct member_info<T, Name> :
        virtual member_info_base<T, Name>,
        member_info_add_attributes<T, Name>,
        traits::pointer_invoker<T, member_info_base<T, Name>::pointer>
    {
    private:
        using base = member_info_base<T, Name>;

    public:
        using pointer_type = std::remove_const_t<decltype(base::pointer)>;
        static constexpr bool is_static = !std::is_member_pointer_v<pointer_type>;
        using type = typename std::conditional_t<
                is_static, 
                std::remove_pointer<pointer_type>,
                traits::remove_member_pointer<pointer_type>
            >::type;
    };

    template <reflected T, fixed_string Name>
    requires (member_info_base<T, Name>::category == category::variable_template)
    struct member_info<T, Name> : 
        virtual member_info_base<T, Name>,
        member_info_add_attributes<T, Name>
    {
    private:
        using base = member_info_base<T, Name>;

    public:
        static constexpr bool is_static = true;
    };




    template <reflected T>
    struct type_info_add_bases : virtual meta<T>
    { using bases = traits::type_sequence<>; };

    template <reflected T>
    requires requires { typename meta<T>::bases; }
    struct type_info_add_bases<T> : virtual meta<T> {};

    
    template <reflected T>
    struct type_info_add_attributes : virtual meta<T>
    { 
        using attributes = attribute_sequence<>; 
        static_assert(attributes::is_valid_for(meta<T>::category));
    };

    template <reflected T>
    requires requires { typename meta<T>::attributes; }
    struct type_info_add_attributes<T> : virtual meta<T> 
    {
        static_assert(meta<T>::attributes::is_valid_for(meta<T>::category));
    };




    template <reflected T>
    struct type_info : 
        virtual meta<T>,
        type_info_add_bases<T>,
        type_info_add_attributes<T>
    {
    private:
        template <reflected U>
        friend struct type_info;

        using self = type_info;

        template <typename IndexSequence, typename = void>
        struct get_member_sequence_impl {};

        template <std::size_t ...Is, typename Dummy>
        struct get_member_sequence_impl<std::index_sequence<Is...>, Dummy>
        {
            using type = typename traits::fluent_type_sequence<
                    member_info<T, self::template member<Is>::name>...
                >::type;
        };

        template <typename Dummy = void>
        using get_member_sequence = typename get_member_sequence_impl<
                std::make_index_sequence<self::member_count>, Dummy
            >::type;

        template <typename BaseSequence, typename = void>
        struct get_base_member_sequence_impl {};

        template <typename ...Bases, typename Dummy>
        struct get_base_member_sequence_impl<traits::type_sequence<Bases...>, Dummy>
        {
            static_assert(
                (... && reflected<Bases>),
                "All bases must be reflected!"
            );
        };

        template <reflected ...Bases, typename Dummy>
        struct get_base_member_sequence_impl<traits::type_sequence<Bases...>, Dummy>
        {
            using type = typename traits::fluent_type_sequence<
                    typename type_info<Bases>
                        ::template get_all_member_sequence<Dummy>...
                >::template flatten<Dummy>::type;
        };

        template <typename Dummy = void>
        using get_base_member_sequence = typename get_base_member_sequence_impl<
                typename self::bases, Dummy
            >::type;

        template <typename Dummy = void>
        using get_all_member_sequence = traits::type_sequence_concat_t<
                get_base_member_sequence<Dummy>, get_member_sequence<Dummy>, Dummy
            >;

    public:
        template <typename Dummy = void>
        using get_members = traits::type_sequence_apply_t<
                get_member_sequence<Dummy>, member_tuple, Dummy
            >;

        template <typename Dummy = void>
        using get_all_members = traits::type_sequence_apply_t<
                get_all_member_sequence<Dummy>, member_tuple, Dummy
            >;
    };
}


#endif

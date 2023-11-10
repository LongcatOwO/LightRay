#pragma once

#include <cstddef>
#include <utility>

#include <lightray/metaprogramming/traits/qualifier_traits.hpp>

#include "cast.hpp"
#include "type_pack.hpp"
#include "value_pack.hpp"


namespace lightray::mtp
{
    namespace detail
    {
        /*
         * Author: P. Lutchanont
         */
        template <auto Key, typename T>
        struct dict_tuple_leaf
        {
        private:
            using element_type = T;

            T _item;

        public:
            dict_tuple_leaf() = default;

            template <typename U>
            constexpr dict_tuple_leaf(U&& u) noexcept(noexcept(T(std::forward<U>(u))))
            : _item(std::forward<U>(u))
            {}

            constexpr auto get()       &  noexcept ->       T&  { return                 _item ; }
            constexpr auto get() const &  noexcept -> const T&  { return                 _item ; }
            constexpr auto get()       && noexcept ->       T&& { return cast<      T&&>(_item); }
            constexpr auto get() const && noexcept -> const T&& { return cast<const T&&>(_item); }
        }; // struct dict_tuple_leaf

        template <auto Key, typename T>
        constexpr auto dict_tuple_resolve_leaf_type(const dict_tuple_leaf<Key, T>& leaf) noexcept
        -> const dict_tuple_leaf<Key, T>& 
        { 
            return leaf; 
        }

        template <auto Key, typename Leaf>
        constexpr auto dict_tuple_get_leaf(Leaf&& leaf) noexcept -> decltype(auto)
        {
            using unqualified_leaf_type = std::remove_cvref_t<
                decltype(dict_tuple_resolve_leaf_type<Key>(leaf))
            >;
            return private_forward_cast<unqualified_leaf_type>(std::forward<Leaf>(leaf));
        }

    } // namespace detail

    /*
     * A tuple whose data can be accessed using arbitrary non-type template parameters as keys.
     *
     * Author: P. Lutchanont
     */
    template <typename Keys, typename ValueTypes>
    struct dict_tuple;

    template <auto... Keys, typename... Ts>
    requires (sizeof...(Keys) == sizeof...(Ts))
    struct dict_tuple<value_pack_t<Keys...>, type_pack_t<Ts...>> : private detail::dict_tuple_leaf<Keys, Ts>...
    {
    private:
        template <auto Key, typename Leaf>
        friend constexpr auto detail::dict_tuple_get_leaf(Leaf&& leaf) noexcept -> decltype(auto);

        template <auto Key>
        using leaf_type = typename std::remove_cvref_t<
            decltype(detail::dict_tuple_get_leaf<Key>(std::declval<dict_tuple>()))
        >;
    public:
        // The element type of the value at the given key.
        template <auto Key>
        using element_type = typename leaf_type<Key>::element_type;

        // Returns the number of elements this dict_tuple contains
        // size() == sizeof...(Ts) == sizeof...(Keys)
        static constexpr auto size() noexcept -> std::size_t { return sizeof...(Ts); }

        // Get the element reference at the given key.
        template <auto Key>
        constexpr auto get() & noexcept -> decltype(auto)
        {
            return detail::dict_tuple_get_leaf<Key>(*this).get();
        }

        // Get the element reference at the given key.
        template <auto Key>
        constexpr auto get() const & noexcept -> decltype(auto)
        {
            return detail::dict_tuple_get_leaf<Key>(*this).get();
        }

        // Get the element reference at the given key.
        template <auto Key>
        constexpr auto get() && noexcept -> decltype(auto)
        {
            return detail::dict_tuple_get_leaf<Key>(std::move(*this)).get();
        }

        // Get the element reference at the given key.
        template <auto Key>
        constexpr auto get() const && noexcept -> decltype(auto)
        {
            return detail::dict_tuple_get_leaf<Key>(std::move(*this)).get();
        }

        // Default constructs each element.
        dict_tuple() = default;

        /*
         * Initializes each element using each argument according to the order
         * of the keys of this dict tuple.
         *
         * If sizeof...(Args) is less than size(), then the remaining elements
         * are default constructed.
         *
         * TODO noexcept needs to be checked for default initialized elements also
         */
        template <typename... Args>
        constexpr dict_tuple(Args&&... args)
        :   dict_tuple(
                []<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    constexpr auto keys = value_pack<Keys...>;
                    return value_pack<(keys.template get<Is>())...>;
                } (std::make_index_sequence<sizeof...(Args)>{}),
                std::forward<Args>(args)...
            )
        {}

        /*
         * Initializes each element using each argument according to the order
         * of the given keys in the first parameter.
         *
         * The remaining elements with unspecified keys are default constructed.
         *
         * TODO noexcept needs to be checked for default initialized elements also
         */
        template <auto... KeyArgs, typename... Args>
        requires (sizeof...(KeyArgs) == sizeof...(Args))
        constexpr dict_tuple(value_pack_t<KeyArgs...>, Args&&... args)
        : leaf_type<KeyArgs>(std::forward<Args>(args))...
        {}

    }; // struct dict_tuple<value_pack_t<Keys...>, type_pack_t<Ts...>>

    // Deduce template parameters by value.
    template <auto... Keys, typename... Ts>
    dict_tuple(value_pack_t<Keys...>, Ts...) -> dict_tuple<value_pack_t<Keys...>, type_pack_t<Ts...>>;

} // namespace lightray::mtp

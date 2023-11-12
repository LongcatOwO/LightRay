#pragma once

#include <tuple>
#include <type_traits>

#include <lightray/metaprogramming/value.hpp>
#include <lightray/metaprogramming/value_pack.hpp>

#include "meta_category.hpp"

namespace lightray::refl
{
    template <meta_category Category>
    struct attribute;

    /*
     * A subtype of a templated variant of attribute.
     */
    template <typename T>
    concept attribute_type = requires (T t)
    {
        []<meta_category C>(const attribute<C>&){} (t);
    };

    /*
     * Inheriting from this class will allow the derived class to be used as attributes.
     *
     * IMPORTANT: This class does not have virtual destructor, and is intended to be lightweight.
     * Avoid dynamic allocations with this class.
     *
     * Author: P. Lutchanont
     */
    template <meta_category Category>
    struct attribute
    {
        static constexpr auto category() noexcept -> meta_category 
        { 
            return Category; 
        }
    }; // struct attribute

    template <typename AttribTuple>
    struct is_attributes
    {
        static constexpr bool value = false;

    }; // is_attributes;

    template <attribute_type... Ts>
    struct is_attributes<std::tuple<Ts...>>
    {
        static constexpr bool value = true;
    };

    template <typename AttribTuple>
    constexpr bool is_attributes_v = is_attributes<AttribTuple>::value;

    template <typename AttribTuple, meta_category Category>
    concept attributes_for_category =
        is_attributes_v<AttribTuple>
     && mtp::make_index_sequence<std::tuple_size_v<AttribTuple>>.apply([]<std::size_t... Indices>{
            return (... && has_flag(
                std::remove_cvref_t<std::tuple_element_t<Indices, AttribTuple>>::category(), Category
            ));
        });


} // namespace lightray::refl

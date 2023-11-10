#pragma once

#include <tuple>
#include <utility>

#include <lightray/metaprogramming/concepts/functor.hpp>

#include "type.hpp"
#include "value_pack.hpp"

namespace lightray::mtp
{
    template <typename Tuple, typename IndexPred>
    requires 
    (
        make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>
        .apply([]<std::size_t... Indices>{
            return (... && concepts::value_to_value_functor<IndexPred, bool, Indices>);
        })
    )
    constexpr auto tuple_filter(Tuple&& t, IndexPred&& p)
    ->  decltype(
            make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>
            .filter(std::forward<IndexPred>(p))
            .apply([]<std::size_t... Indices>{ 
                return type<std::tuple<std::tuple_element_t<Indices, std::remove_cvref_t<Tuple>>...>>; 
            })
        )::type
    {
        return make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>
            .filter(std::forward<IndexPred>(p))
            .apply([&]<std::size_t... Indices>{
                return std::tuple<std::tuple_element_t<Indices, std::remove_cvref_t<Tuple>>...> 
                {std::get<Indices>(std::forward<Tuple>(t))...};
            });
    }

    template <typename Tuple, typename IndexPred>
    requires
    (
        make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>
        .apply([]<std::size_t... Indices>{
            return (... && concepts::value_to_value_functor<IndexPred, bool, Indices>);
        })
    )
    constexpr auto tuple_find_if(Tuple&& t, IndexPred&& p)
    ->  decltype(
            make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>
            .template find_if<true>(std::forward<IndexPred>(p))
            .map([]<std::size_t Index>{ return std::tuple_element<Index, std::remove_cvref_t<Tuple>>{}; })
        )::type
    {
        return make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>
            .template find_if<true>(std::forward<IndexPred>(p))
            .map([&]<std::size_t Index>{ 
                return std::tuple_element_t<Index, std::remove_cvref_t<Tuple>>
                {std::get<Index>(std::forward<Tuple>(t))};
            });
    }

} // namespace lightray::mtp

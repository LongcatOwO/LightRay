#pragma once

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
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
    constexpr auto tuple_filter(Tuple&& t, IndexPred&& p) ->  decltype(auto)
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
            using index_pred_t = IndexPred;
            return (... && concepts::value_to_value_functor<index_pred_t, bool, Indices>);
        })
    )
    constexpr auto tuple_find_if(Tuple&& t, IndexPred&& p) -> decltype(auto)
    {
        return make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>
            .template find_if<true>(std::forward<IndexPred>(p))
            .map([&]<std::size_t Index>{ 
                return std::tuple_element_t<Index, std::remove_cvref_t<Tuple>>
                {std::get<Index>(std::forward<Tuple>(t))};
            });
    }

    struct std_hasher
    {
        template <typename T>
        auto operator()(const T& t) const noexcept -> auto
        {
            return std::hash<std::remove_volatile_t<T>>{}(t);
        }
    };

    constexpr void combine_hash(std::size_t& seed, std::size_t next_hash) noexcept
    {
        seed ^= next_hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <typename Pair, typename ElementHasher = std_hasher>
    struct pair_hasher;

    template <typename First, typename Second, typename ElementHasher>
    struct pair_hasher<std::pair<First, Second>, ElementHasher>
    {
        constexpr auto operator()(const std::pair<First, Second>& p) const noexcept
        -> std::size_t
        {
            std::size_t seed = 0;
            combine_hash(seed, ElementHasher{}(p.first));
            combine_hash(seed, ElementHasher{}(p.second));
            return seed;
        }
    }; // struct pair_hasher

    template <typename Tuple, typename ElementHasher = std_hasher>
    struct tuple_hasher;

    template <typename... Ts, typename ElementHasher>
    struct tuple_hasher<std::tuple<Ts...>, ElementHasher>
    {
        constexpr auto operator()(const std::tuple<Ts...>& tup) const noexcept
        -> std::size_t
        {
            std::size_t seed = 0;
            make_index_sequence<std::tuple_size_v<std::tuple<Ts...>>>.for_each([&]<auto I>{
                combine_hash(seed, ElementHasher{}(std::get<I>(tup)));
            });
            return seed;
        }

    }; // struct tuple_hasher

} // namespace lightray::mtp

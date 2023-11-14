#pragma once

#include <array>
#include <cstddef>

#include <lightray/metaprogramming/traits/unified_pointer_traits.hpp>
#include "value.hpp"

namespace lightray::mtp
{
    namespace detail
    {
#       pragma pack(push, 1)
        template <typename PaddedT, std::size_t NumPaddingBytes>
        struct offset_of_padded_object
        {
            std::array<std::byte, NumPaddingBytes> padding_bytes;
            PaddedT padded_obj;
        }; // struct offset_of_padded_object
#       pragma pack(pop)

        template <typename PaddedT>
        struct offset_of_padded_object<PaddedT, 0>
        {
            PaddedT padded_obj;
        };

        template <typename T, typename PaddedT, std::size_t NumPaddingBytes>
        union offset_of_storage_t
        {
            char dummy_init;
            offset_of_padded_object<PaddedT, NumPaddingBytes> padded_member;
            T obj;
        };

        template <typename T, typename PaddedT, std::size_t NumPaddingBytes>
        constexpr offset_of_storage_t<T, PaddedT, NumPaddingBytes> offset_of_storage = {0};

        template <typename C, typename T, typename DeclaringType, T DeclaringType::* MemberPtr>
        consteval auto constexpr_offset_of(value_t<MemberPtr>) noexcept -> std::ptrdiff_t
        {
            constexpr auto find_offset = [](auto&& self, auto low, auto high)
            {
                const auto mid = (high + low) / value<2>;
                constexpr auto& storage = detail::offset_of_storage<C, T, mid.value>;
                constexpr auto* real_addr = &(storage.obj.*MemberPtr);
                constexpr auto* padded_addr = &storage.padded_member.padded_obj;


                if constexpr (padded_addr == real_addr)
                    return mid;

                else if constexpr (padded_addr < real_addr)
                    return self(self, mid + value<1>, high);

                else if constexpr (padded_addr > real_addr)
                    return self(self, low, mid - value<1>);

            };

            return find_offset(find_offset, value<0>, value<sizeof(C)>).value;
        }
    } // namespace detail

    template <
        auto MemberPtr,
        typename ClassT = traits::unified_pointer_traits<decltype(MemberPtr)>::class_type
    >
    requires std::is_member_object_pointer_v<decltype(MemberPtr)>
    constexpr std::ptrdiff_t offset_of = detail::constexpr_offset_of<ClassT>(value<MemberPtr>);

    template <typename C, typename T>
    requires std::is_member_object_pointer_v<T C::*>
    auto runtime_offset_of(T C::* member_ptr) noexcept -> std::ptrdiff_t
    {
        const auto& storage = detail::offset_of_storage<C, T, 0>;
        return 
            reinterpret_cast<const volatile char*>(&(storage.obj.*member_ptr))
          - reinterpret_cast<const volatile char*>(&storage.padded_member.padded_obj);
    }

} // namespace lightray::mtp

#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

#include <lightray/metaprogramming/tags.hpp>
#include <lightray/metaprogramming/type_pack.hpp>
#include <lightray/metaprogramming/value_pack.hpp>

#include "member_info.hpp"
#include "meta_extraction.hpp"


namespace lightray::refl
{
    namespace detail
    {
        template <reflected T>
        constexpr auto type_info_meta_namespace_name() noexcept -> auto
        {
            static_assert(
                requires { meta_type_t<T>::namespace_name(); }, 
                "Cannot find namespace. This should never happen."
            );

            return meta_type_t<T>::namespace_name();
        }

        template <reflected T>
        constexpr auto type_info_declaring_type() noexcept -> auto
        {
            if constexpr (requires { meta_type_t<T>::declaring_type(); })
                return meta_type_t<T>::declaring_type();
            else
                return mtp::none;
        }

        template <reflected T>
        constexpr auto type_info_namespace_name() noexcept -> auto
        {
            using namespace mtp::splice::type;

            constexpr auto find_namespace = []<reflected U>(auto self, mtp::type_t<U>)
            {
                if constexpr (std::is_same_v<decltype(type_info_declaring_type<U>()), mtp::none_t>)
                    return type_info_meta_namespace_name<U>();

                else if constexpr (reflected<decl_t<type_info_declaring_type<U>()>>)
                    return self(self, mtp::type<decl_t<type_info_declaring_type<U>()>>);

                else
                    return mtp::none;
            };

            return find_namespace(find_namespace, mtp::type<T>);
        }

        template <reflected T>
        constexpr auto type_info_type() noexcept -> auto
        {
            if constexpr (requires { meta_type_t<T>::type(); })
                return meta_type_t<T>::type();

            else
                return mtp::none;
        }

        template <reflected T>
        constexpr auto type_info_name() noexcept -> auto
        {
            return meta_type_t<T>::name();
        }

        template <reflected T>
        constexpr auto type_info_category() noexcept -> meta_category
        {
            return meta_type_t<T>::category();
        }

        template <reflected T>
        constexpr auto type_info_member_count() noexcept -> std::size_t
        {
            return meta_type_t<T>::member_count();
        }

        template <reflected T>
        constexpr auto type_info_members() noexcept -> auto
        {
            return mtp::make_index_sequence<type_info_member_count<T>()>.apply([]<std::size_t... Indices>{
                return mtp::value_pack<member_info<T, Indices>...>;
            });
        }

        template <reflected T>
        constexpr auto type_info_bases() noexcept -> auto
        {
            if constexpr (requires { meta_type_t<T>::bases(); })
                return meta_type_t<T>::bases();
            
            else
                return mtp::type_pack<>;
        }

        template <reflected T>
        constexpr auto type_info_attributes() noexcept -> auto
        {
            if constexpr (requires { meta_type_t<T>::attributes(); })
                return meta_type_t<T>::attributes();

            else
                return std::tuple{};
        }

        template <reflected T, typename TypeInfo, typename Derived>
        constexpr auto type_info_proxy_type() noexcept -> auto
        {
            if constexpr (requires { meta_type_t<T>::template proxy_type<Derived, TypeInfo>(); })
                return meta_type_t<T>::template proxy_type<Derived, TypeInfo>();

            else
                return mtp::none;
        }

        template <reflected T, typename TypeInfo, typename Derived>
        constexpr auto type_info_interface_proxy_type() noexcept -> auto
        {
            if constexpr (requires { meta_type_t<T>::template interface_proxy_type<Derived, TypeInfo>(); })
                return meta_type_t<T>::template interface_proxy_type<Derived, TypeInfo>();

            else
                return mtp::none;
        }

        template <reflected T>
        constexpr auto type_info_id_accessor_type() noexcept -> auto
        {
            if constexpr (requires { meta_type_t<T>::id_accessor_type(); })
                return meta_type_t<T>::id_accessor_type();

            else
                return mtp::none;
        }

    } // namespace detail

    template <reflected T>
    struct type_info_t
    {
        static constexpr auto declaring_type() noexcept -> auto
        {
            return detail::type_info_declaring_type<T>();
        }

        static constexpr auto namespace_name() noexcept -> auto
        {
            return detail::type_info_namespace_name<T>();
        }

        static constexpr auto type() noexcept -> auto
        {
            return detail::type_info_type<T>();
        }

        static constexpr auto name() noexcept -> auto
        {
            return detail::type_info_name<T>();
        }

        static constexpr auto category() noexcept -> meta_category
        {
            return detail::type_info_category<T>();
        }

        static constexpr auto member_count() noexcept -> std::size_t
        {
            return detail::type_info_member_count<T>();
        }

        static constexpr auto members() noexcept -> auto
        {
            return detail::type_info_members<T>();
        }

        static constexpr auto bases() noexcept -> auto
        {
            return detail::type_info_bases<T>();
        }

        static constexpr auto attributes() noexcept -> auto
        {
            return detail::type_info_attributes<T>();
        }

        template <typename Derived>
        static constexpr auto proxy_type() noexcept -> auto
        {
            return detail::type_info_proxy_type<T, type_info_t, Derived>();
        }

        template <typename Derived>
        static constexpr auto interface_proxy_type() noexcept -> auto
        {
            return detail::type_info_interface_proxy_type<T, type_info_t, Derived>();
        }

        static constexpr auto id_accessor_type() noexcept -> auto
        {
            return detail::type_info_id_accessor_type<T>();
        }

    }; // struct type_info_t

    template <reflected T>
    constexpr auto type_info_ = type_info_t<T>{};

} // namespace lightray::refl

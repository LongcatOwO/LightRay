#pragma once

#include <concepts>
#include <cstddef>
#include <tuple>
#include <utility>

#include <lightray/metaprogramming/tags.hpp>
#include <lightray/metaprogramming/type.hpp>
#include <lightray/metaprogramming/value.hpp>
#include <lightray/metaprogramming/functors/from_trait.hpp>
#include <lightray/metaprogramming/traits/unified_pointer_traits.hpp>

#include "meta_category.hpp"
#include "meta_extraction.hpp"


namespace lightray::refl
{
    namespace detail
    {
        template <reflected T, std::size_t Index>
        using member_meta_type_t = typename meta_type_t<T>::template member<Index>;

        template <reflected T, std::size_t Index>
        constexpr auto member_info_index() noexcept -> std::size_t
        {
            return member_meta_type_t<T, Index>::index();
        }

        template <reflected T, std::size_t Index>
        constexpr auto member_info_declaring_type() noexcept -> auto
        {
            return member_meta_type_t<T, Index>::declaring_type();
        }

        template <reflected T, std::size_t Index>
        constexpr auto member_info_name() noexcept -> auto
        {
            return member_meta_type_t<T, Index>::name();
        }

        template <reflected T, std::size_t Index>
        constexpr auto member_info_category() noexcept -> meta_category
        {
            return member_meta_type_t<T, Index>::category();
        }

        template <reflected T, std::size_t Index>
        constexpr auto member_info_attributes() noexcept -> auto
        {
            if constexpr (requires { member_meta_type_t<T, Index>::attributes(); })
                return member_meta_type_t<T, Index>::attributes();

            else
                return std::tuple{};
        }

        template <reflected T, std::size_t Index, typename... TArgs>
        constexpr auto member_info_pointer() noexcept -> auto
        {
            using mc = meta_category;

            static_assert(
                has_flag(mc::variable | mc::type | mc::function, member_info_category<T, Index>()), 
                "category not implemented!"
            );

            if constexpr (member_info_category<T, Index>() == mc::variable)
                return member_meta_type_t<T, Index>::template pointer<TArgs...>();

            else if constexpr (member_info_category<T, Index>() == mc::type)
                return mtp::none;

            else if constexpr (member_info_category<T, Index>() == mc::function)
                return mtp::none;
        }

        template <reflected T, std::size_t Index, typename... TArgs>
        constexpr auto member_info_pointer_type() noexcept -> auto
        {
            using ptr_type = decltype(member_info_pointer<T, Index, TArgs...>());

            if constexpr (std::same_as<mtp::none_t, ptr_type>)
                return mtp::none;

            else
                return mtp::type<ptr_type>;
        }

        template <reflected T, std::size_t Index>
        constexpr auto member_info_is_static() noexcept -> auto
        {
            using mc = meta_category;
            using namespace mtp::functors;

            static_assert(
                has_flag(mc::variable | mc::type | mc::function, member_info_category<T, Index>()),
                "category not implemented!"
            );

            if constexpr (member_info_category<T, Index>() == mc::type)
                return mtp::true_;

            else if constexpr (member_info_category<T, Index>() == mc::variable)
                return mtp::value<!requires { member_meta_type_t<T, Index>::pointer(); }>;

            else if constexpr (member_info_category<T, Index>() == mc::function)
                return mtp::none;
        }

        template <reflected T, std::size_t Index, typename... TArgs>
        constexpr auto member_info_type() noexcept -> auto
        {
            using mc = meta_category;
            using namespace mtp::traits;
            using namespace mtp::splice::type;

            static_assert(
                has_flag(mc::variable | mc::type | mc::function, member_info_category<T, Index>()),
                "category not implemented!"
            );

            if constexpr (member_info_category<T, Index>() == mc::variable)
                return mtp::type<
                    typename unified_pointer_traits<decl_t<member_info_pointer_type<T, Index, TArgs...>()>>
                    ::element_type
                >;

            else if constexpr (member_info_category<T, Index>() == mc::type)
                return member_meta_type_t<T, Index>::template type<TArgs...>();

            else if constexpr (member_info_category<T, Index>() == mc::function)
                return mtp::none;
        }

        template <reflected T, std::size_t Index, typename... TArgs, typename Self, typename... Args>
        constexpr auto member_info_invoke(Self&& self, Args&&... args)
        noexcept(noexcept(
            [&self = self, &...args = args]()
            {
                using mc = meta_category;

                static_assert(
                    has_flag(mc::type | mc::variable | mc::function, member_info_category<T, Index>()), 
                    "category not implemented!"
                );

                if constexpr (member_info_category<T, Index>() == mc::type)
                {
                    return member_info_type<T, Index, TArgs...>()(std::forward<Args>(args)...);
                }
                else if constexpr (member_info_category<T, Index>() == mc::variable)
                {
                    if constexpr (member_info_is_static<T, Index>())
                        return *member_info_pointer<T, Index, TArgs...>();

                    else
                        return std::forward<Self>(self).*member_info_pointer<T, Index, TArgs...>();
                }
                else if constexpr (member_info_category<T, Index>() == mc::function)
                {
                    return member_meta_type_t<T, Index>
                        ::template invoke<TArgs...>(std::forward<Self>(self), std::forward<Args>(args)...);
                }
            } ()
        ))
        -> decltype(auto)
        requires
        (
            (
                (member_info_category<T, Index>() == meta_category::type)
             && std::constructible_from<
                    mtp::splice::type::decl_t<member_info_type<T, Index, TArgs...>()>,
                    Args&&...
                >
            )
         || (
                (member_info_category<T, Index>() == meta_category::variable)
             && requires { member_info_pointer<T, Index, TArgs...>(); }
            )
         || (member_info_category<T, Index>() == meta_category::function)
         || (!has_flag(
                meta_category::type
              | meta_category::variable
              | meta_category::function, member_info_category<T, Index>()
            ))
        )
        {
            using mc = meta_category;

            static_assert(
                has_flag(mc::type | mc::variable | mc::function, member_info_category<T, Index>()), 
                "category not implemented!"
            );

            if constexpr (member_info_category<T, Index>() == mc::type)
            {
                return member_info_type<T, Index, TArgs...>()(std::forward<Args>(args)...);
            }
            else if constexpr (member_info_category<T, Index>() == mc::variable)
            {
                if constexpr (member_info_is_static<T, Index>())
                    return *member_info_pointer<T, Index, TArgs...>();

                else
                    return std::forward<Self>(self).*member_info_pointer<T, Index, TArgs...>();
            }
            else if constexpr (member_info_category<T, Index>() == mc::function)
            {
                return member_meta_type_t<T, Index>
                    ::template invoke<TArgs...>(std::forward<Self>(self), std::forward<Args>(args)...);
            }
        }

        template <reflected T, std::size_t Index, typename MemberInfo, typename Derived>
        constexpr auto member_info_proxy_type() noexcept -> auto
        {
            if constexpr (requires {
                member_meta_type_t<T, Index>::template proxy_type<Derived, MemberInfo>(); 
            })
                return member_meta_type_t<T, Index>::template proxy_type<Derived, MemberInfo>();

            else
                return mtp::none;
        }

        template <reflected T, std::size_t Index, typename MemberInfo, typename Derived>
        constexpr auto member_info_interface_proxy_type() noexcept -> auto
        {
            if constexpr (requires {
                member_meta_type_t<T, Index>::template interface_proxy_type<Derived, MemberInfo>();
            })
                return member_meta_type_t<T, Index>::template interface_proxy_type<Derived, MemberInfo>();

            else
                return mtp::none;
        }

        template <reflected T, std::size_t Index, typename MemberInfo>
        constexpr auto member_info_id_accessor_type() noexcept -> auto
        {
            if constexpr (requires { member_meta_type_t<T, Index>::id_accessor_type(); })
                return member_meta_type_t<T, Index>::id_accessor_type();

            else
                return mtp::none;
        }

    } // namespace detail

    template <reflected T, std::size_t Index>
    struct member_info_t
    {
        static constexpr auto index() noexcept -> std::size_t
        {
            return detail::member_info_index<T, Index>();
        }

        static constexpr auto declaring_type() noexcept -> auto
        {
            return detail::member_info_declaring_type<T, Index>();
        }

        static constexpr auto name() noexcept -> auto
        {
            return detail::member_info_name<T, Index>();
        }

        static constexpr auto category() noexcept -> meta_category
        {
            return detail::member_info_category<T, Index>();
        }

        static constexpr auto attributes() noexcept -> auto
        {
            return detail::member_info_attributes<T, Index>();
        }

        template <typename... TArgs>
        static constexpr auto pointer() noexcept -> auto
        {
            return detail::member_info_pointer<T, Index, TArgs...>();
        }

        template <typename... TArgs>
        static constexpr auto pointer_type() noexcept -> auto
        {
            return detail::member_info_pointer_type<T, Index, TArgs...>();
        }

        static constexpr auto is_static() noexcept -> auto
        {
            return detail::member_info_is_static<T, Index>();
        }

        template <typename... TArgs>
        static constexpr auto type() noexcept -> auto
        {
            return detail::member_info_type<T, Index, TArgs...>();
        }

        template <typename... TArgs, typename Self, typename... Args>
        static constexpr auto invoke(Self&& self, Args&&... args)
        noexcept(noexcept(
            detail::member_info_invoke<T, Index, TArgs...>(
                std::forward<Self>(self), std::forward<Args>(args)...
            )
        ))
        -> decltype(auto)
        {
            return detail::member_info_invoke<T, Index, TArgs...>(
                std::forward<Self>(self), std::forward<Args>(args)...
            );
        }

        template <typename Derived>
        static constexpr auto proxy_type() noexcept -> auto
        {
            return detail::member_info_proxy_type<T, Index, member_info_t, Derived>();
        }

        template <typename Derived>
        static constexpr auto interface_proxy_type() noexcept -> auto
        {
            return detail::member_info_interface_proxy_type<T, Index, member_info_t, Derived>();
        }
        
        static constexpr auto id_accessor_type() noexcept -> auto
        {
            return detail::member_info_id_accessor_type<T, Index, member_info_t>();
        }

    }; // member_info_t

    template <reflected T, std::size_t Index>
    constexpr auto member_info = member_info_t<T, Index>{};

} // namespace lightray::refl

#pragma once

#include <utility>

#include <lightray/metaprogramming/inherit_from.hpp>
#include <lightray/metaprogramming/type.hpp>

#include "type_info.hpp"

namespace lightray::refl
{
    template <reflected T>
    struct builder :
        mtp::splice::type::decl_t<type_info_<T>.members().apply([]<auto... Members>{
            using mtp::splice::type::decl_t;
            return mtp::type<mtp::inherit_from<decl_t<Members.template proxy_type<builder<T>>()>...>>;
        })>
    {
    private:
        T _data;

    public:
        template <typename MemberInfo, typename TAssigned>
        constexpr builder& on_proxy_invoked(MemberInfo info, TAssigned&& assigned)
        noexcept(noexcept(info.invoke(_data) = std::forward<TAssigned>(assigned)))
        {
            info.invoke(_data) = std::forward<TAssigned>(assigned);
            return *this;
        }

        constexpr const T& get() const noexcept
        {
            return _data;
        }

        constexpr T&& move_get() noexcept
        {
            return std::move(_data);
        }

    }; // struct builder

} // namespace lightray::refl

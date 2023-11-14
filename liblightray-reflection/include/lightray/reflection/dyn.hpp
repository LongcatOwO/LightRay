#pragma once

#include <cassert>
#include <cstddef>
#include <memory>
#include <tuple>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

#include <lightray/metaprogramming/cast.hpp>
#include <lightray/metaprogramming/dict_tuple.hpp>
#include <lightray/metaprogramming/fixed_string.hpp>
#include <lightray/metaprogramming/function_pointer.hpp>
#include <lightray/metaprogramming/inherit_from.hpp>
#include <lightray/metaprogramming/on_destructed.hpp>
#include <lightray/metaprogramming/overload.hpp>
#include <lightray/metaprogramming/owning.hpp>
#include <lightray/metaprogramming/tuple_util.hpp>
#include <lightray/metaprogramming/type.hpp>
#include <lightray/metaprogramming/value.hpp>
#include <lightray/metaprogramming/void_ref_ptr.hpp>
#include <lightray/metaprogramming/traits/function_traits.hpp>
#include <lightray/metaprogramming/traits/qualifier_traits.hpp>

#include "type_info.hpp"


namespace lightray::refl
{
    template <reflected Prototype, bool Cloneable>
    struct dyn;

    namespace detail
    {
        constexpr auto dyn_copy_constructor_func_id() noexcept -> auto
        {
            return mtp::fixed_string("__copy_constructor__");
        }

        constexpr auto dyn_destructor_func_id() noexcept -> auto
        {
            return mtp::fixed_string("__destructor__");
        }

        constexpr auto dyn_type_info_func_id() noexcept -> auto
        {
            return mtp::fixed_string("__type_info__");
        }

        template <typename TargetType, typename IdAccessor, typename FuncSignature>
        constexpr auto dyn_make_function_pointers() noexcept -> auto
        {
            using func_traits = mtp::traits::function_traits<FuncSignature>;
            using qual_traits = typename func_traits::qualifier_traits;
            using return_t = typename func_traits::return_type;
            constexpr auto arg_count = func_traits::argument_count;

            constexpr auto make_func_ptr = []<typename SelfPtr>(mtp::type_t<SelfPtr>) {
                return mtp::make_index_sequence<arg_count>.apply([]<auto... Is>{
                    // used for deducing type
                    if constexpr (std::is_void_v<TargetType>)
                        return mtp::function_pointer<
                            return_t (*)(SelfPtr, typename func_traits::template argument_type<Is>... args)
                        >{};
                    else 
                        return mtp::function_pointer{
                            +[](SelfPtr self, typename func_traits::template argument_type<Is>... args)
                            -> return_t 
                            {
                                return IdAccessor::invoke(
                                    self.template as<TargetType>(), std::forward<decltype(args)>(args)...
                                ); 
                            }
                        };
                });
            };

            if constexpr (!qual_traits::is_reference)
            {
                using lself_ptr_t = mtp::void_ref_ptr<
                    mtp::traits::qualifier_traits<typename qual_traits::template apply<int>&>
                >;
                using rself_ptr_t = mtp::void_ref_ptr<
                    mtp::traits::qualifier_traits<typename qual_traits::template apply<int>&&>
                >;

                return std::tuple{
                    make_func_ptr(mtp::type<lself_ptr_t>), 
                    make_func_ptr(mtp::type<rself_ptr_t>)
                };
            }
            else
            {
                using self_ptr_t = mtp::void_ref_ptr<qual_traits>;
                
                return std::tuple{make_func_ptr(mtp::type<self_ptr_t>)};
            }
        }

        template <typename TargetType>
        constexpr auto dyn_make_copy_constructor_function_pointer() noexcept -> auto
        {
            using self_ptr_t = mtp::void_ref_ptr<mtp::traits::const_lvalue_traits>;
            if constexpr (std::is_void_v<TargetType>)
                return mtp::function_pointer<mtp::owning<void*> (*)(self_ptr_t)>{};
            else
                return mtp::function_pointer{+[](self_ptr_t self) -> mtp::owning<void*>{
                    return new auto(self.as<TargetType>());
                }};
        }

        template <typename TargetType>
        constexpr auto dyn_make_destructor_function_pointer() noexcept -> auto
        {
            using self_ptr_t = mtp::void_ref_ptr<mtp::traits::const_lvalue_traits>;
            if constexpr (std::is_void_v<TargetType>)
                return mtp::function_pointer<void (*)(self_ptr_t)>{};
            else
                return mtp::function_pointer{+[](self_ptr_t self){ delete &self.as<TargetType>(); }};
        }

        template <typename TargetType>
        constexpr auto dyn_make_type_info_function_pointer() noexcept -> auto
        {
            if constexpr (std::is_void_v<TargetType>)
                return mtp::function_pointer<const std::type_info& (*)()>{};
            else
                return mtp::function_pointer{+[]() -> const std::type_info& { return typeid(TargetType); }};
        }

        template <typename TargetType, reflected Prototype, bool Cloneable, mtp::fixed_string Name>
        constexpr auto dyn_make_overload() noexcept -> auto
        {
            using namespace mtp::splice::type;

            constexpr auto member = type_info_<Prototype>.members()
                .find_if([]<auto M>{ return mtp::value<M.name() == Name>; });

            using intf_proxy_t = decl_t<member.template interface_proxy_type<dyn<Prototype, Cloneable>>()>;
            
            using id_accessor_t = decl_t<member.id_accessor_type()>;

            return mtp::make_index_sequence<intf_proxy_t::function_count()>.apply([]<auto... Is>{
                constexpr auto func_ptr_tuple = std::tuple_cat(
                    dyn_make_function_pointers<
                        TargetType,
                        id_accessor_t,
                        decl_t<intf_proxy_t::template function_type<Is>()>
                    >()...
                );
                constexpr auto func_ptr_count = std::tuple_size_v<std::remove_const_t<decltype(func_ptr_tuple)>>;
                return mtp::make_index_sequence<func_ptr_count>.apply([func_ptr_tuple=func_ptr_tuple]<auto... FnIs>{
                    return mtp::overload{std::get<FnIs>(func_ptr_tuple)...};
                });
            });
        }

        template <typename TargetType, reflected Prototype, bool Cloneable>
        constexpr auto dyn_make_vtable() noexcept -> auto
        {
            using namespace mtp::fixed_string_literals;

            constexpr auto members = type_info_<Prototype>.members();

            if constexpr (Cloneable)
                return members.apply([]<auto... Members>{
                    return mtp::dict_tuple(
                        mtp::value_pack<
                            Members.name()...,
                            dyn_copy_constructor_func_id(),
                            dyn_destructor_func_id(),
                            dyn_type_info_func_id()
                        >,
                        dyn_make_overload<TargetType, Prototype, Cloneable, Members.name()>()...,
                        dyn_make_copy_constructor_function_pointer<TargetType>(),
                        dyn_make_destructor_function_pointer<TargetType>(),
                        dyn_make_type_info_function_pointer<TargetType>()
                    );
                });
            else
                return members.apply([]<auto... Members>{
                    return mtp::dict_tuple(
                        mtp::value_pack<
                            Members.name()...,
                            dyn_destructor_func_id(),
                            dyn_type_info_func_id()
                        >,
                        dyn_make_overload<TargetType, Prototype, Cloneable, Members.name()>()...,
                        dyn_make_destructor_function_pointer<TargetType>(),
                        dyn_make_type_info_function_pointer<TargetType>()
                    );
                });
        }

        template <reflected Prototype, bool Cloneable>
        using dyn_vtable_t = decltype(dyn_make_vtable<void, Prototype, Cloneable>());

        template <typename TargetType, reflected Prototype, bool Cloneable>
        constexpr dyn_vtable_t<Prototype, Cloneable> dyn_vtable = dyn_make_vtable<TargetType, Prototype, Cloneable>();


        template <reflected ToPrototype, bool ToCloneable, reflected FromPrototype, bool FromCloneable>
        constexpr auto dyn_convert_vtable(const dyn_vtable_t<FromPrototype, FromCloneable>& from) noexcept -> auto
        {
            dyn_vtable_t<ToPrototype, ToCloneable> to;
            constexpr auto names = to.keys();
            constexpr auto is_special_name = [](const auto& name) {
                return
                    name == dyn_copy_constructor_func_id()
                 || name == dyn_destructor_func_id();
            };

            names.filter([]<auto Name>{ return mtp::value<is_special_name(Name)>; })
            .for_each([&]<auto Name>{
                to.template get<Name>() = from.template get<Name>();
            });

            names.filter([]<auto Name>{ return mtp::value<!is_special_name(Name)>; })
            .for_each([&]<auto Name>{
                auto& to_overload = to.template get<Name>();
                const auto& from_overload = from.template get<Name>();

                [&from_overload]<typename... FuncPtrs>(mtp::overload<FuncPtrs...>& to_overload)
                {
                    ((mtp::ldefault_cast<FuncPtrs>(to_overload) = mtp::ldefault_cast<FuncPtrs>(from_overload))
                     ,...);
                } (to_overload);
            });

            return to;
        }

        using dyn_dynamic_vtable_key_t = std::pair<const std::type_info&, const std::type_info&>;

        template <reflected Prototype, bool Cloneable>
        struct dyn_dynamic_vtable_t;

        template <reflected Prototype, bool Cloneable>
        using dyn_dynamic_vtable_map_t = std::unordered_map<
            dyn_dynamic_vtable_key_t,
            std::weak_ptr<const dyn_dynamic_vtable_t<Prototype, Cloneable>>,
            mtp::pair_hasher<dyn_dynamic_vtable_key_t>
        >;


        template <reflected Prototype, bool Cloneable>
        struct dyn_dynamic_vtable_remover
        {
        private:
            dyn_dynamic_vtable_map_t<Prototype, Cloneable>* _vtable_map;
            dyn_dynamic_vtable_key_t _key;

        public:
            dyn_dynamic_vtable_remover(
                dyn_dynamic_vtable_map_t<Prototype, Cloneable>* vtable_map,
                dyn_dynamic_vtable_key_t key
            ) noexcept
            :   _vtable_map{vtable_map},
                _key{key}
            {}

            constexpr auto operator()() const noexcept -> void
            {
                _vtable_map->erase(_key);
            }
        };

        template <reflected Prototype, bool Cloneable>
        struct dyn_dynamic_vtable_t : dyn_vtable_t<Prototype, Cloneable>
        {
        private:
            mtp::on_destructed<dyn_dynamic_vtable_remover<Prototype, Cloneable>> _remover;

        public:
            template <reflected OtherPrototype>
            dyn_dynamic_vtable_t(
                const dyn_vtable_t<OtherPrototype, Cloneable>& other_vtable,
                dyn_dynamic_vtable_map_t<Prototype, Cloneable>* vtable_map,
                dyn_dynamic_vtable_key_t key
            ) noexcept
            :   dyn_vtable_t<Prototype, Cloneable>{dyn_convert_vtable<Prototype, Cloneable>(other_vtable)},
                _remover{vtable_map, std::move(key)}
            {}
        };

        template <reflected Prototype, bool Cloneable>
        constinit dyn_dynamic_vtable_map_t<Prototype, Cloneable> dyn_dynamic_vtable_map{};

        
        template <reflected ToPrototype, bool ToCloneable, reflected FromPrototype, bool FromCloneable>
        constexpr auto dyn_get_dynamic_vtable(const dyn_vtable_t<FromPrototype, FromCloneable>* vtable) noexcept
        -> std::shared_ptr<const dyn_dynamic_vtable_t<ToPrototype, ToCloneable>>
        {
            if (!vtable) return nullptr;

            dyn_dynamic_vtable_key_t key = {
                typeid(FromPrototype),
                vtable->template get<dyn_type_info_func_id()>()()
            };
            auto& map = dyn_dynamic_vtable_map<ToPrototype, ToCloneable>;
            if (auto search = map.find(key); search != map.end())
            {
                std::weak_ptr<const dyn_dynamic_vtable_t<ToPrototype, ToCloneable>> ptr = search->second;
                assert(ptr.use_count() != 0);
                return ptr.lock();
            }
            else
            {
                auto ptr = std::make_shared<const dyn_dynamic_vtable_t<ToPrototype, ToCloneable>>(
                    dyn_convert_vtable<ToPrototype>(*vtable)
                );
                map.emplace(key, ptr);
                return ptr;
            }
        }

    } // namespace detail

    template <reflected Prototype, bool Cloneable = false>
    struct dyn
    :   mtp::splice::type::decl_t<
            type_info_<Prototype>.members().apply([]<auto... Members>{
                return mtp::type<mtp::inherit_from<
                    mtp::splice::type::decl_t<
                        Members.template interface_proxy_type<dyn<Prototype, Cloneable>>()
                    >...
                >>;
            })
        >
    {
    private:
        template <reflected, bool>
        friend struct dyn;

        std::variant<
            const detail::dyn_vtable_t<Prototype, Cloneable>*,
            std::shared_ptr<const detail::dyn_dynamic_vtable_t<Prototype, Cloneable>>
        > _vtable;

        mtp::owning<void*> _obj;

        constexpr mtp::owning<void*> _copy_constructor() const requires Cloneable
        {
            if (!_obj) return nullptr;
            return std::visit([&](auto& vtable) {
                return vtable->template get<detail::dyn_copy_constructor_func_id()>()(
                    mtp::void_ref_ptr<mtp::traits::const_lvalue_traits>{_obj}
                );
            }, _vtable);
        }

        constexpr void _destructor() const
        {
            if (!_obj) return;
            std::visit([&](auto& vtable) {
                return vtable->template get<detail::dyn_destructor_func_id()>()(
                    mtp::void_ref_ptr<mtp::traits::const_lvalue_traits>{_obj}
                );
            }, _vtable);
        }

    public:
        constexpr dyn() noexcept : _vtable{nullptr}, _obj{nullptr} {}
        constexpr dyn(std::nullptr_t) noexcept : dyn{} {}

        template <typename T>
        constexpr dyn(std::unique_ptr<T> impl) noexcept
        :   _vtable{&detail::dyn_vtable<T, Prototype, Cloneable>},
            _obj(impl.release())
        {}

        constexpr dyn(const dyn& other) requires Cloneable
        :   _vtable{other._vtable},
            _obj{other._copy_constructor()}
        {}

        constexpr dyn(dyn&& other) noexcept
        :   _vtable{std::exchange(other._vtable, nullptr)},
            _obj{std::exchange(other._obj, nullptr)}
        {}

        template <reflected OtherPrototype>
        constexpr dyn(const dyn<OtherPrototype, true>& other)
        :   _vtable{detail::dyn_get_dynamic_vtable<Prototype, Cloneable>(other._vtable)},
            _obj{other._copy_constructor()}
        {}

        template <reflected OtherPrototype, bool OtherCloneable> requires (!Cloneable || OtherCloneable)
        constexpr dyn(dyn<OtherPrototype, OtherCloneable>&& other) noexcept
        :   _vtable{detail::dyn_get_dynamic_vtable<Prototype, Cloneable>(std::exchange(other._vtable, nullptr))},
            _obj{std::exchange(other._obj, nullptr)}
        {}

        constexpr auto operator=(const dyn& other) -> dyn& requires Cloneable
        {
            _destructor();
            _vtable = other._vtable;
            _obj = other._copy_constructor();
            return *this;
        }

        constexpr auto operator=(dyn&& other) noexcept -> dyn&
        {
            _destructor();
            _vtable = std::exchange(other._vtable, nullptr);
            _obj = std::exchange(other._obj, nullptr);
            return *this;
        }

        template <reflected OtherPrototype>
        constexpr auto operator=(const dyn<OtherPrototype, true>& other) -> dyn&
        {
            _destructor();
            _vtable = detail::dyn_get_dynamic_vtable<Prototype, Cloneable>(other._vtable);
            _obj = other._copy_constructor();
            return *this;
        }

        template <reflected OtherPrototype, bool OtherCloneable> requires (!Cloneable || OtherCloneable)
        constexpr auto operator=(dyn<OtherPrototype, OtherCloneable>&& other) noexcept -> dyn&
        {
            _destructor();
            _vtable = detail::dyn_get_dynamic_vtable<Prototype, Cloneable>(std::exchange(other._vtable, nullptr));
            _obj = std::exchange(other._obj, nullptr);
            return *this;
        }

        constexpr ~dyn() { _destructor(); }

        constexpr auto on_proxy_invoked(auto info, auto&&... args)       & -> decltype(auto)
        {
            return std::visit([&](auto& vtable) {
                return vtable->template get<info.name()>()(
                    mtp::void_ref_ptr<mtp::traits::lvalue_traits>{_obj},
                    std::forward<decltype(args)>(args)...
                );
            }, _vtable);
        }

        constexpr auto on_proxy_invoked(auto info, auto&&... args) const & -> decltype(auto)
        {
            return std::visit([&](auto& vtable) {
                return vtable->template get<info.name()>()(
                    mtp::void_ref_ptr<mtp::traits::const_lvalue_traits>{_obj},
                    std::forward<decltype(args)>(args)...
                );
            }, _vtable);
        }

        constexpr auto on_proxy_invoked(auto info, auto&&... args)       && -> decltype(auto)
        {
            return std::visit([&](auto& vtable) {
                return vtable->template get<info.name()>()(
                    mtp::void_ref_ptr<mtp::traits::rvalue_traits>{_obj},
                    std::forward<decltype(args)>(args)...
                );
            }, _vtable);
        }

        constexpr auto on_proxy_invoked(auto info, auto&&... args) const && -> decltype(auto)
        {
            return std::visit([&](auto& vtable) {
                return vtable->template get<info.name()>()(
                    mtp::void_ref_ptr<mtp::traits::const_rvalue_traits>{_obj},
                    std::forward<decltype(args)>(args)...
                );
            }, _vtable);
        }

    }; // struct dyn

} // namespace lightray::refl

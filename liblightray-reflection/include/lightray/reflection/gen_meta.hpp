#ifndef _LIGHTRAY_REFLECTION_GEN_META_HPP_
#define _LIGHTRAY_REFLECTION_GEN_META_HPP_

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include <lightray/metaprogramming/cast.hpp>
#include <lightray/metaprogramming/fixed_string.hpp>
#include <lightray/metaprogramming/tags.hpp>
#include <lightray/metaprogramming/type.hpp>
#include <lightray/metaprogramming/type_pack.hpp>
#include <lightray/metaprogramming/concepts/common.hpp>

#include <lightray/preprocessor/common.hpp>
#include <lightray/preprocessor/logical/if.hpp>
#include <lightray/preprocessor/seq/for_each.hpp>
#include <lightray/preprocessor/seq/size.hpp>
#include <lightray/preprocessor/tup/elem.hpp>
#include <lightray/preprocessor/tup/for_each.hpp>
#include <lightray/preprocessor/tup/rest_n.hpp>
#include <lightray/preprocessor/tup/size.hpp>
#include <lightray/preprocessor/var/for_each.hpp>
#include <lightray/preprocessor/var/size.hpp>

#include <lightray/reflection/attribute.hpp>
#include <lightray/reflection/meta_category.hpp>
#include <lightray/reflection/meta_extraction.hpp>


/**
 *
 *  Guide to Scalable Macro:
 *
 *      Whenever a new internal macro is defined (that is, intended to be used by another macro),
 *      if the macro expands to a nested macro whose id could potentially be in the current disabling context,
 *      make the new macro as following:
 *
 *          #define NEW_MACRO(...) LIGHTRAY_PP_DEFER(NEW_MACRO_)(__VA_ARGS__)
 *          #define NEW_MACRO_(...) ... - actual implementation here
 *
 *      The reason for doing this is to allow the macro expansion to be deferred, allowing the stacks
 *      of disabling context to get wiped off, before being reactivated again by an expanding macro.
 *      Doing so will allow macro recursion to be possible, since the names of the nested macros
 *      won't get blue-painted by the discarded disabling context anymore.
 *
 *      If a nested macro expansion is needed (such as for the purpose of reactivating the macros from 
 *      the above example). The macro wishing to perform the expansion MUST define its own expansion macro,
 *      preferrably named the same as its id suffixed with '_EXPAND', and used ONLY that expanding macro
 *      inside its own definition to perform expansion wherever is needed. 
 *      For example:
 *
 *          #define MACRO_EXPAND(...) __VA_ARGS__
 *          #define MACRO(...) MACRO_EXPAND(NESTED_MACRO(__VA_ARGS__))
 *
 *      This is to avoid the same expansion macro being used recursively, resulting in it getting
 *      blue-painted, disabling the expansion.
 *      
 *      A macro can be made "invocable", by having a common prefix with other groups of invocable macros.
 *      This will allow the user to pass in the suffix of the macro they wished to be invoked, along
 *      with their parameters. Then, the macro suffix passed in can be invoked using concatenation.
 *
 *      For example:
 *
 *          #define COMMON_PREFIX_FUNC1(...) "This is func1 parameters: " LIGHTRAY_PP_STRINGIZE(__VA_ARGS__)
 *          #define COMMON_PREFIX_FUNC2(...) "This is func2 parameters: " LIGHTRAY_PP_STRINGIZE(__VA_ARGS__)
 *          #define INVOKE_FUNC(f_func) LIGHTRAY_PP_CAT(COMMON_PREFIX_, f_func)
 *
 *          ---------- on call site ----------
 *          INVOKE_FUNC(FUNC1(abc)) -----expands to-----> "This is func1 parameters: " "abc"
 *          INVOKE_FUNC(FUNC2(def)) -----expands to-----> "This is func2 parameters: " "def"
 *
 *      However, it is quite common that COMMON_PREFIX_FUNC1 or COMMON_PREFIX_FUNC2 will need to also use
 *      LIGHTRAY_PP_CAT (for concatenation), since it is a commonly needed macro, but because those two
 *      invocable macros are expanded inside the disabling context of LIGHTRAY_PP_CAT, if LIGHTRAY_PP_CAT
 *      appears in their expansion, its id will get blue-painted.
 *
 *      As a result we need to defer their expansion, allowing the disabling context to go away, and then
 *      reactivate it using an expansion macro.
 *
 *      Following all the advice above, this would be the preferred implementation of our last example:
 *
 *          #define COMMON_PREFIX_FUNC1(...) LIGHTRAY_PP_DEFER(COMMON_PREFIX_FUNC1_)(__VA_ARGS__)
 *          #define COMMON_PREFIX_FUNC2(...) LIGHTRAY_PP_DEFER(COMMON_PREFIX_FUNC2_)(__VA_ARGS__)
 *
 *          #define COMMON_PREFIX_FUNC1_(...) "This is func1 parameters: " LIGHTRAY_PP_STRINGIZE(__VA_ARGS__)
 *          #define COMMON_PREFIX_FUNC2_(...) "This is func2 parameters: " LIGHTRAY_PP_STRINGIZE(__VA_ARGS__)
 *
 *          // added for demonstration purpose
 *          #define COMMON_PREFIX_FUNC3(...) LIGHTRAY_PP_DEFER(COMMON_PREFIX_FUNC3_)(__VA_ARGS__)
 *          #define COMMON_PREFIX_FUNC3_(arg) LIGHTRAY_PP_CAT(012, arg)
 *          
 *          #define INVOKE_FUNC(f_func) INVOKE_FUNC_EXPAND(LIGHTRAY_PP_CAT(COMMON_PREFIX_, f_func))
 *          #define INVOKE_FUNC_EXPAND(...) __VA_ARGS__
 *
 *      With the new refined implementation, even if our COMMON_PREFIX_FUNC3 need to use LIGHTRAY_PP_CAT,
 *      it can still use it just fine:
 *
 *          INVOKE_FUNC(FUNC3(345)) -----expands to-----> 012345
 *
 *      With the above technique, it would be nice to name the macro parameter in such a way that
 *      the user will know that they have to put in a 'function call', while also telling the implementors
 *      themselves that this parameter is a 'function call' that is meant to be invoked.
 *
 *      Therefore, such parameter should be prefixed with 'f_' for 'function'.
 *
 *      Similiarly, to distinguish between different parameter types, other prefix conventions are also
 *      made as following:
 *
 *          'v_' for 'value', i.e. regular parameters.
 *
 *          'd_' for 'discard', i.e. an unused parameter
 *
 *          'tup_' for 'tuple', i.e. (a, b, c).
 *              Useful for packing multiple parameters into one parameter.
 *              Such as: MACRO(a, (b, c, d), e)
 *
 *          'seq_' for 'sequence', i.e. (a)(b)(c)(d)
 *              Also useful for packing multiple parameters into one parameter.
 *              Such as: MACRO(a, (b)(c)(d), e)
 *              This can also be used to by pass macro arguments limit for FOR_EACH expression.
 *              Not sure if there is any macro expansion depth limitation though
 *              (apart from the implementation limit of LightRay.Preprocessor).
 *
 *  Author: P. Lutchanont
**/


// TODO: add GitHub issue about empty base class optimization for all classes


// func_sig = (return-type)((ArgT0)(arg0), (ArgT1)(arg1), (ArgT2)(arg2), ...)(cv-ref-qualifiers)
#define LIGHTRAY_REFL_FUNCSIG_ARGS_FOR_EACH(macro, seq_func_sig) \
    LIGHTRAY_PP_TUP_FOR_EACH( \
        LIGHTRAY_REFL_FUNCSIG_ARGS_FOR_EACH_EACH, macro, LIGHTRAY_PP_SEQ_ELEM(1, seq_func_sig) \
    )
#define LIGHTRAY_REFL_FUNCSIG_ARGS_FOR_EACH_EACH(macro, v_i, seq_arg) \
    LIGHTRAY_PP_IF(v_i, LIGHTRAY_PP_COMMA, LIGHTRAY_PP_EMPTY)() \
    macro( \
        LIGHTRAY_PP_EXPAND LIGHTRAY_PP_SEQ_ELEM(0, seq_arg), \
        LIGHTRAY_PP_EXPAND LIGHTRAY_PP_SEQ_ELEM(1, seq_arg) \
    )

#define LIGHTRAY_REFL_FUNCSIG_RETURN(seq_func_sig) \
    LIGHTRAY_REFL_FUNCSIG_RETURN_I(LIGHTRAY_PP_EXPAND LIGHTRAY_PP_SEQ_ELEM(0, seq_func_sig))
#define LIGHTRAY_REFL_FUNCSIG_RETURN_I(ret) ret

#define LIGHTRAY_REFL_FUNCSIG_ARGS_DECL(seq_func_sig) \
    LIGHTRAY_REFL_FUNCSIG_ARGS_FOR_EACH(LIGHTRAY_REFL_FUNCSIG_ARG_DECL, seq_func_sig)
#define LIGHTRAY_REFL_FUNCSIG_ARG_DECL(type, var) type var

#define LIGHTRAY_REFL_FUNCSIG_QUALS(seq_func_sig) \
    LIGHTRAY_REFL_FUNCSIG_QUALS_I(LIGHTRAY_PP_EXPAND LIGHTRAY_PP_SEQ_ELEM(2, seq_func_sig))
#define LIGHTRAY_REFL_FUNCSIG_QUALS_I(quals) quals

#define LIGHTRAY_REFL_FUNCSIG_TYPE(seq_func_sig) \
    LIGHTRAY_REFL_FUNCSIG_RETURN(seq_func_sig) \
    (LIGHTRAY_REFL_FUNCSIG_ARGS_DECL(seq_func_sig)) \
    LIGHTRAY_REFL_FUNCSIG_QUALS(seq_func_sig)

#define LIGHTRAY_REFL_FUNCSIG_ARGS_FORWARD(seq_func_sig) \
    LIGHTRAY_REFL_FUNCSIG_ARGS_FOR_EACH(LIGHTRAY_REFL_FUNCSIG_ARG_FORWARD, seq_func_sig)
#define LIGHTRAY_REFL_FUNCSIG_ARG_FORWARD(type, var) std::forward<type>(var)

#define LIGHTRAY_REFL_FUNCSIG_ARGS_COUNT(seq_func_sig) \
    LIGHTRAY_PP_TUP_SIZE(LIGHTRAY_PP_SEQ_ELEM(1, seq_func_sig))


// Declare the enclosing namespace
#define LIGHTRAY_REFL_INVOKE_namespace(...) LIGHTRAY_PP_DEFER(LIGHTRAY_REFL_INVOKE_namespace_)(__VA_ARGS__)
#define LIGHTRAY_REFL_INVOKE_namespace_(v_namespace) \
    static constexpr auto namespace_name() noexcept -> auto \
    { return ::lightray::mtp::fixed_string{LIGHTRAY_PP_STRINGIZE(v_namespace)}; }

// Declare the enclosing class
#define LIGHTRAY_REFL_INVOKE_outer_class(...) LIGHTRAY_PP_DEFER(LIGHTRAY_REFL_INVOKE_outer_class_)(__VA_ARGS__)
#define LIGHTRAY_REFL_INVOKE_outer_class_(v_class) \
    static constexpr auto declaring_type() noexcept -> auto \
    { return ::lightray::mtp::type<v_class>; }

// Optional TYPE metadata: declare bases of the type
#define LIGHTRAY_REFL_BASES(v_id, ...) \
    static constexpr auto bases() noexcept -> auto \
    { return ::lightray::mtp::type_pack<__VA_ARGS__>; } \
    static_assert \
    ( \
        ::lightray::mtp::concepts::derived_from< \
            ::lightray::mtp::splice::type::decl_t<type()> __VA_OPT__(,) \
            __VA_ARGS__ \
        >, \
        "Invalid base specified." \
    );
#define LIGHTRAY_REFL_ARGV_bases(...) __VA_ARGS__
#define LIGHTRAY_REFL_MACRO_bases(...) LIGHTRAY_REFL_BASES

// Optional ALL metadata: declare attributes of the entity
#define LIGHTRAY_REFL_ATTRIBUTES(v_id, ...) \
    static constexpr auto attributes() noexcept -> auto \
    { return std::tuple{__VA_ARGS__}; } \
    static_assert \
    ( \
        ::lightray::refl::attributes_for_category<decltype(attributes()), category()>, \
        "Invalid attribute type for the category" \
    );
#define LIGHTRAY_REFL_ARGV_attributes(...) __VA_ARGS__
#define LIGHTRAY_REFL_MACRO_attributes(...) LIGHTRAY_REFL_ATTRIBUTES


// Optional ALL metadata: declare proxy of this entity
#define LIGHTRAY_REFL_PROXY(v_id) \
    template <typename Derived, typename Info> struct proxy \
    { \
        template <typename... Params, typename... Args> \
        constexpr decltype(auto) v_id(Args&&... args) \
        noexcept(noexcept( \
            [&...args = args] \
            { \
                if constexpr (sizeof...(Params) == 0) \
                    return static_cast<Derived*>(this) \
                        ->on_proxy_invoked(Info{}, std::forward<Args>(args)...); \
                else \
                    return static_cast<Derived*>(this) \
                        ->template on_proxy_invoked<Params...>(Info{}, std::forward<Args>(args)...); \
            } () \
        )) \
        { \
            if constexpr (sizeof...(Params) == 0) \
                return static_cast<Derived*>(this) \
                    ->on_proxy_invoked(Info{}, std::forward<Args>(args)...); \
            else \
                return static_cast<Derived*>(this) \
                    ->template on_proxy_invoked<Params...>(Info{}, std::forward<Args>(args)...); \
        } \
        \
        template <typename... Params, typename... Args> \
        constexpr decltype(auto) v_id(Args&&... args) const \
        noexcept(noexcept( \
            [&...args = args] \
            { \
                if constexpr (sizeof...(Params) == 0) \
                    return static_cast<Derived*>(this) \
                        ->on_proxy_invoked(Info{}, std::forward<Args>(args)...); \
                else \
                    return static_cast<Derived*>(this) \
                        ->template on_proxy_invoked<Params...>(Info{}, std::forward<Args>(args)...); \
            } () \
        )) \
        { \
            if constexpr (sizeof...(Params) == 0) \
                return static_cast<Derived*>(this) \
                    -> on_proxy_invoked(Info{}, std::forward<Args>(args)...); \
            else \
                return static_cast<Derived*>(this) \
                    ->template on_proxy_invoked<Params...>(Info{}, std::forward<Args>(args)...); \
        } \
    }; \
    template <typename Derived, typename Info> \
    static constexpr auto proxy_type() noexcept -> auto \
    { return ::lightray::mtp::type<proxy<Derived, Info>>; }

#define LIGHTRAY_REFL_ARGV_proxy
#define LIGHTRAY_REFL_MACRO_proxy LIGHTRAY_REFL_PROXY

// Optional ALL metadata: declare proxy of this entity with defined function signature.
#define LIGHTRAY_REFL_INTERFACE_PROXY(v_id, ...) \
    template <typename Derived, typename Info> struct interface_proxy \
    { \
        static constexpr auto function_count() noexcept -> std::size_t \
        { return LIGHTRAY_PP_VAR_SIZE(__VA_ARGS__); } \
        \
        template <std::size_t I> static constexpr auto function_type() noexcept -> auto {} \
        \
        LIGHTRAY_PP_VAR_FOR_EACH(LIGHTRAY_REFL_INTERFACE_PROXY_EACH, v_id, __VA_ARGS__) \
    }; \
    \
    template <typename Derived, typename Info> \
    static constexpr auto proxy_type() noexcept -> auto \
    { return ::lightray::mtp::type<interface_proxy<Derived, Info>>; }

#define LIGHTRAY_REFL_INTERFACE_PROXY_EACH(v_id, v_i, seq_func_sig) \
    template <> constexpr auto function_type<v_i>() noexcept -> auto \
    { return ::lightray::mtp::type<LIGHTRAY_REFL_FUNCSIG_TYPE(seq_func_sig)>; } \
    \
    constexpr LIGHTRAY_REFL_FUNCSIG_RETURN(seq_func_sig) v_id \
    (LIGHTRAY_REFL_FUNCSIG_ARGS_DECL(seq_func_sig)) LIGHTRAY_REFL_FUNCSIG_QUALS(seq_func_sig) \
    { \
        return ::lightray::mtp::ldefault_cast<Derived LIGHTRAY_REFL_FUNCSIG_QUALS(seq_func_sig)>(*this) \
            .on_proxy_invoked( \
                Info{} \
                LIGHTRAY_PP_IF( \
                    LIGHTRAY_REFL_FUNCSIG_ARGS_COUNT(seq_func_sig), \
                    LIGHTRAY_PP_COMMA, \
                    LIGHTRAY_PP_EMPTY \
                )() \
                LIGHTRAY_REFL_FUNCSIG_ARGS_FORWARD(seq_func_sig) \
            ); \
    }

#define LIGHTRAY_REFL_ARGV_interface_proxy(...) __VA_ARGS__
#define LIGHTRAY_REFL_MACRO_interface_proxy(...) LIGHTRAY_REFL_INTERFACE_PROXY


// Optional ALL metadata: declare an id accessor for the same id as this entity.
#define LIGHTRAY_REFL_ID_ACCESSOR(v_id, ...) \
    struct id_accessor \
    { \
        template <typename T> \
        static constexpr auto access() noexcept -> decltype(auto) \
        { return T::v_id; } \
        \
        template <typename T> \
        static constexpr auto access(T&& obj) noexcept -> decltype(auto) \
        { return std::forward<T>(obj).v_name; } \
        \
        template <typename T, bool ForceTemplate = false, typename... TArgs, typename... Args> \
        static constexpr auto invoke(Args&&... args) -> decltype(auto) \
        { \
            if constexpr ( \
                !ForceTemplate && sizeof...(TArgs) == 0 \
             && requires { T::v_id(std::forward<Args>(args)...); } \
            ) \
                return T::v_id(std::forward<Args>(args)...); \
            else \
                return T::template v_id<TArgs...>(std::forward<Args>(args)...); \
        } \
        \
        template <bool ForceTemplate = false, typename... TArgs, typename T, typename... Args> \
        static constexpr auto invoke(T&& obj, Args&&... args) -> decltype(auto) \
        { \
            if constexpr ( \
                !ForceTemplate && sizeof...(TArgs) == 0 \
             && requires { std::forward<T>(obj).v_id(std::forward<Args>(args)...); } \
            ) \
                return std::forward<T>(obj).v_id(std::forward<Args>(args)...); \
            else \
                return std::forward<T>(obj).template v_id<TArgs...>(std::forward<Args>(args)...); \
        } \
    }; \
    \
    static constexpr auto id_accessor_type() noexcept -> auto \
    { return ::lightray::mtp::type<id_accessor>; }

#define LIGHTRAY_REFL_ARGV_id_accessor
#define LIGHTRAY_REFL_MACRO_id_accessor LIGHTRAY_REFL_ID_ACCESSOR



#define LIGHTRAY_REFL_DATA(v_id, tup_f_data) \
    LIGHTRAY_REFL_DATA_EXPAND(LIGHTRAY_PP_TUP_FOR_EACH(LIGHTRAY_REFL_DATA_EACH, v_id, tup_f_data))

#define LIGHTRAY_REFL_DATA_EXPAND(...) __VA_ARGS__
#define LIGHTRAY_REFL_DATA_EACH(v_id, v_i, f_data) \
    LIGHTRAY_REFL_DATA_EACH_I( \
        LIGHTRAY_PP_CAT(LIGHTRAY_REFL_MACRO_, f_data), \
        v_id, \
        LIGHTRAY_PP_CAT(LIGHTRAY_REFL_ARGV_, f_data) \
    )
#define LIGHTRAY_REFL_DATA_EACH_I(macro, v_id, ...) \
    LIGHTRAY_PP_DEFER(macro)(v_id, __VA_ARGS__)




#define LIGHTRAY_REFL_MEMBER_TYPE(v_id) \
    static constexpr auto category() noexcept -> ::lightray::refl::meta_category \
    { return ::lightray::refl::meta_category::type; } \
    \
    template <typename... TArgs> \
    static constexpr auto type() noexcept -> auto \
    { \
        if constexpr (sizeof...(TArgs) == 0 && requires { typename DeclaringType::v_id; }) \
            return ::lightray::mtp::type<typename DeclaringType::v_id>; \
        else \
            return ::lightray::mtp::type<typename DeclaringType::template v_id<TArgs...>>; \
    }
#define LIGHTRAY_REFL_MACRO_MEMBER_type LIGHTRAY_REFL_MEMBER_TYPE


#define LIGHTRAY_REFL_MEMBER_VAR(v_id) \
    static constexpr auto category() noexcept -> ::lightray::refl::meta_category \
    { return ::lightray::refl::meta_category::variable; } \
    \
    template <typename... TArgs> \
    static constexpr auto pointer() noexcept -> auto \
    { \
        if constexpr (sizeof...(TArgs) == 0 && requires { &DeclaringType::v_id; }) \
            return &DeclaringType::v_id; \
        else \
            return &DeclaringType::template v_id<TArgs...>; \
    }
#define LIGHTRAY_REFL_MACRO_MEMBER_var LIGHTRAY_REFL_MEMBER_VAR


#define LIGHTRAY_REFL_MEMBER_FUNC(v_id) \
    static constexpr auto category() noexcept -> ::lightray::refl::meta_category \
    { return ::lightray::refl::meta_category::function; } \
    \
    template <bool AsTemplate = false, typename... TArgs, typename Self, typename... Args> \
    static constexpr auto invoke(Self&& self, Args&&... args) \
    noexcept(noexcept( \
        [&self = self, &...args = args]() -> decltype(auto) \
        { \
            if constexpr (!std::is_same_v<::lightray::mtp::none_t, std::remove_cvref_t<Self>>) \
            { \
                if constexpr (!AsTemplate) \
                    return std::forward<Self>(self) \
                        .DeclaringType::v_id(std::forward<Args>(args)...); \
                else \
                    return std::forward<Self>(self) \
                        .DeclaringType::template v_id<TArgs...>(std::forward<Args>(args)...); \
            } \
            else \
            { \
                if constexpr (!AsTemplate) \
                    return DeclaringType::v_id(std::forward<Args>(args)...); \
                else \
                    return DeclaringType::template v_id<TArgs...>(std::forward<Args>(args)...); \
            } \
        } () \
    )) \
    -> decltype(auto) \
    { \
        if constexpr (!std::is_same_v<::lightray::mtp::none_t, std::remove_cvref_t<Self>>) \
        { \
            if constexpr (!AsTemplate) \
                return std::forward<Self>(self) \
                    .DeclaringType::v_id(std::forward<Args>(args)...); \
            else \
                return std::forward<Self>(self) \
                    .DeclaringType::template v_id<TArgs...>(std::forward<Args>(args)...); \
        } \
        else \
        { \
            if constexpr (!AsTemplate) \
                return DeclaringType::v_id(std::forward<Args>(args)...); \
            else \
                return DeclaringType::template v_id<TArgs...>(std::forward<Args>(args)...); \
        } \
    }
    
#define LIGHTRAY_REFL_MACRO_MEMBER_func LIGHTRAY_REFL_MEMBER_FUNC


/*
 * tup_members = (member-type, member-id, tup_f_data)
 */
#define LIGHTRAY_REFL_MEMBERS(seq_tup_members) \
    LIGHTRAY_REFL_MEMBERS_EXPAND(LIGHTRAY_PP_SEQ_FOR_EACH(LIGHTRAY_REFL_MEMBERS_EACH, data, seq_tup_members))

#define LIGHTRAY_REFL_MEMBERS_EXPAND(...) __VA_ARGS__
#define LIGHTRAY_REFL_MEMBERS_EACH(d_data, v_index, tup_member) \
    LIGHTRAY_PP_DEFER(LIGHTRAY_REFL_MEMBERS_EACH_I)(d_data, v_index, tup_member)

#define LIGHTRAY_REFL_MEMBERS_EACH_I(d_data, v_index, tup_member) \
    LIGHTRAY_REFL_MEMBERS_EACH_II( \
        LIGHTRAY_PP_CAT(LIGHTRAY_REFL_MACRO_MEMBER_, LIGHTRAY_PP_TUP_ELEM(0, tup_member)), \
        v_index, \
        LIGHTRAY_PP_TUP_ELEM(1, tup_member), \
        LIGHTRAY_PP_TUP_ELEM(2, tup_member) \
    )

#define LIGHTRAY_REFL_MEMBERS_EACH_II(member_macro, v_index, v_id, tup_f_data) \
    template <typename DeclaringType> struct member<v_index, DeclaringType> \
    { \
        static constexpr auto index() noexcept -> std::size_t \
        { return v_index; } \
        \
        static constexpr auto declaring_type() noexcept -> auto \
        { return ::lightray::mtp::type<DeclaringType>; } \
        \
        static constexpr auto name() noexcept -> auto \
        { return ::lightray::mtp::fixed_string(LIGHTRAY_PP_STRINGIZE(v_id)); } \
        \
        member_macro(v_id) \
        LIGHTRAY_REFL_DATA(v_id, tup_f_data) \
    };


#define LIGHTRAY_REFL_TYPE(f_namespace_or_outer_class, v_type, tup_f_data, seq_tup_members) \
    struct _lightray_meta_ \
    { \
        LIGHTRAY_REFL_TYPE_EXPAND(LIGHTRAY_PP_CAT(LIGHTRAY_REFL_INVOKE_, f_namespace_or_outer_class)) \
        static constexpr auto type() noexcept -> auto \
        { return ::lightray::mtp::type<v_type>; } \
        \
        static constexpr auto name() noexcept -> auto \
        { return ::lightray::mtp::fixed_string(LIGHTRAY_PP_STRINGIZE(v_type)); } \
        \
        static constexpr auto category() noexcept -> ::lightray::refl::meta_category \
        { return ::lightray::refl::meta_category::type; } \
        \
        static constexpr auto member_count() noexcept -> ::std::size_t \
        { return LIGHTRAY_PP_SEQ_SIZE(seq_tup_members); } \
        \
        template <::std::size_t Index, typename DeclaringType = v_type> struct member; \
        \
        LIGHTRAY_REFL_DATA(v_type, tup_f_data) \
        LIGHTRAY_REFL_MEMBERS(seq_tup_members) \
    }; \
    \
    friend _lightray_meta_ operator<<(::lightray::refl::extract_meta_t, v_type const &);
#define LIGHTRAY_REFL_TYPE_EXPAND(...) __VA_ARGS__


#endif

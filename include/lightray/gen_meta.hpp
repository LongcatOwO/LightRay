/*
 *  TODO: implement EXTRA() functionality which allows more metadata, 
 *  such as attributes.
 *
 *  TODO: make a documentation
 *
 *  MACRO NAMING CONVENTION:
 *      Internal macros are prepended with underscore '_'.
 *      lowercase letters allow for special macro function names that can be invoked 
 *      using LIGHTRAY_REFL_INVOKE. Helper functions should have its main function as prefix.
 *      
 *      Macro arguments should be prefixed with 'p_', except for special macro functions, 
 *      which should be prefixed with 'f_' instead. Parameters that would be discarded 
 *      should be prefixed with 'd_'.
 *
 *
 *  PARAMETER DECLARATION:
 *  Use the following form to declare paramters: param((type1, var1), (type2, var2), (type3, var3), ...)
 *  
 *  Optionally, each sub-tuple can also be declared as (type, var, ...[literal three-dots]) to 
 *  If the user want the variable names to be automatically generated, 
 *  use param_auto_name(...) instead.
 *  signify variadic argument.
 *
 *  BEGIN REFLECTION SCOPE:
 *  Use LIGHTRAY_REFL_BEGIN(f_category) to begin reflection scope.
 *  f_category can be one of the following: type, type_tmpl, enum
 *
 *  Here are the call signatures:
 *      type(type_to_reflect, ...)
 *      type_tmpl(tmpl_to_reflect, tmpl_param_decl, ...) 
 *          where tmpl_param_decl is a PARAMETER_DECLARATION.
 *      func(func_to_reflect, tmpl_overload(param_decl, ...), ...) 
 *          where param_decl is a PARAMETER_DECLARATION
 *      func(func_to_reflect, overload(static(), instance(), static_tmpl(tmpl_param_decl), instance_tmpl(tmpl_param_decl)))
 *
 *  The variadic arguments provide extra optional functions that can be used to add extra metadata,
 *  such as namespace, attributes, etc.
 *
 *  TYPE METADATA CONTENT:
 *      member type 'type' which refers to the type being reflected.
 *
 *      member variable 'name' which refers to the name of the type being reflected.
 *
 *      member type 'member<std::size_t Index>' which contains 
 *      member metadata.
 *
 *      member variable 'member_count'
 *
 *  MEMBER METADATA CONTENT:
 *      member type 'declaring_type' which refers to the type which declares the member
 *      member variable 'name' which refers to the name of the member being reflected.
 *      member variable 'category' which refers to the category of the member being reflected. The available categories are: type, variable, function
 *
 *  MEMBER TYPE METADATA CONTENT:
 *      member type 'type' which refers to the member type being reflected.
 *
 *  MEMBER VARIABLE METADATA CONTENT:
 *      member variable 'pointer' which is the member pointer of the member being reflected.
 *  MEMBER FUNCTION METADATA CONTENT:
 *      member function 'invoke' which calls the member function being reflected.
 */



#ifndef _LIGHTRAY_GEN_META_HPP_
#define _LIGHTRAY_GEN_META_HPP_


#include <cstddef>
#include <type_traits>
#include <utility>

#include <lightray/preprocessor/common.hpp>
#include <lightray/preprocessor/variadic.hpp>
#include <lightray/preprocessor/seq.hpp>

#include <lightray/coredef.hpp>
#include <lightray/fixed_string.hpp>
#include <lightray/traits.hpp>




#define _LIGHTRAY_REFL_EXTRACT_MANGLE(...) __VA_ARGS__
#define _LIGHTRAY_REFL_EXTRACT_NAMESPACE(...) __VA_ARGS__

#define _LIGHTRAY_REFL_META_TYPE(p_type, p_mangle) \
LIGHTRAY_PP_CONCAT( \
    lightray_meta, \
    LIGHTRAY_PP_CONCAT( \
        p_mangle, \
        p_type \
    ) \
)


#define _LIGHTRAY_REFL_D_BASES(...) LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_BASES)(__VA_ARGS__)
#define _LIGHTRAY_REFL_BASES(...) \
    using bases = ::lightray::traits::type_sequence<__VA_ARGS__>;

#define _LIGHTRAY_REFL_D_ATTRIBUTES(...) LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_ATTRIBUTES)(__VA_ARGS__)
#define _LIGHTRAY_REFL_ATTRIBUTES(...) \
    using attributes = ::lightray::attribute_sequence<__VA_ARGS__>;

#define _LIGHTRAY_REFL_D_EXTRA(...) LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_EXTRA)(__VA_ARGS__)
#define _LIGHTRAY_REFL_EXTRA(...) \
    _LIGHTRAY_REFL_EXTRA_EXPAND(_LIGHTRAY_REFL_EXTRA_EXPAND( \
        LIGHTRAY_PP_VARIADIC_FOR_EACH(_LIGHTRAY_REFL_EXTRA_EACH, none, LIGHTRAY_PP_EMPTY, __VA_ARGS__) \
    ))

#define _LIGHTRAY_REFL_EXTRA_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_REFL_D_EXTRA_EACH(d_data, d_i, f_func) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_EXTRA_EACH)(d_data, d_i, f_func)

#define _LIGHTRAY_REFL_EXTRA_EACH(d_data, d_i, f_func) \
    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_, f_func)


#define _LIGHTRAY_REFL_D_DECL_PARAM(...) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_PARAM)(__VA_ARGS__)

#define _LIGHTRAY_REFL_DECL_PARAM(...) \
    LIGHTRAY_PP_VARIADIC_FOR_EACH( \
        _LIGHTRAY_REFL_DECL_PARAM_EACH, none, LIGHTRAY_PP_COMMA, __VA_ARGS__ \
    )

#define _LIGHTRAY_REFL_DECL_PARAM_EACH(d_data, d_i, p_tuple) \
    _LIGHTRAY_REFL_DECL_PARAM_EACH_SHUFFLE p_tuple

#define _LIGHTRAY_REFL_DECL_PARAM_EACH_SHUFFLE(p_type, p_var, ...) \
    p_type __VA_ARGS__ p_var


#define _LIGHTRAY_REFL_D_APPLY_PARAM(...) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_APPLY_PARAM)(__VA_ARGS__)

#define _LIGHTRAY_REFL_APPLY_PARAM(...) \
    LIGHTRAY_PP_VARIADIC_FOR_EACH( \
        _LIGHTRAY_REFL_APPLY_PARAM_EACH, none, LIGHTRAY_PP_COMMA, __VA_ARGS__ \
    )

#define _LIGHTRAY_REFL_APPLY_PARAM_EACH(d_data, d_i, p_tuple) \
    _LIGHTRAY_REFL_APPLY_PARAM_EACH_SHUFFLE p_tuple

#define _LIGHTRAY_REFL_APPLY_PARAM_EACH_SHUFFLE(p_type, p_var, ...) \
    p_var __VA_ARGS__




#define _LIGHTRAY_REFL_D_DECL_PARAM_UNNAMED(...) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_PARAM_UNNAMED)(__VA_ARGS__)

#define _LIGHTRAY_REFL_DECL_PARAM_UNNAMED(...) \
    LIGHTRAY_PP_VARIADIC_FOR_EACH( \
        _LIGHTRAY_REFL_DECL_PARAM_UNNAMED_EACH, none, LIGHTRAY_PP_COMMA, __VA_ARGS__ \
    )

#define _LIGHTRAY_REFL_DECL_PARAM_UNNAMED_EACH_EXPAND(...) __VA_ARGS__
#define _LIGHTRAY_REFL_DECL_PARAM_UNNAMED_EACH(d_data, p_i, p_tuple) \
    _LIGHTRAY_REFL_DECL_PARAM_UNNAMED_EACH_EXPAND( \
        LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_PARAM_UNNAMED_EACH_SHUFFLE)( \
            p_i, _LIGHTRAY_REFL_DECL_PARAM_UNNAMED_EACH_EXPAND p_tuple \
        ) \
    )

#define _LIGHTRAY_REFL_DECL_PARAM_UNNAMED_EACH_SHUFFLE(p_i, p_type, ...) \
    p_type __VA_ARGS__ LIGHTRAY_PP_CONCAT(LightrayUnnamed, p_i)


#define _LIGHTRAY_REFL_D_APPLY_PARAM_UNNAMED(...) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_APPLY_PARAM_UNNAMED)(__VA_ARGS__)

#define _LIGHTRAY_REFL_APPLY_PARAM_UNNAMED(...) \
    LIGHTRAY_PP_VARIADIC_FOR_EACH( \
        _LIGHTRAY_REFL_APPLY_PARAM_UNNAMED_EACH, none, LIGHTRAY_PP_COMMA, __VA_ARGS__ \
    )

#define _LIGHTRAY_REFL_APPLY_PARAM_UNNAMED_EACH_EXPAND(...) __VA_ARGS__
#define _LIGHTRAY_REFL_APPLY_PARAM_UNNAMED_EACH(d_data, p_i, p_tuple) \
    _LIGHTRAY_REFL_APPLY_PARAM_UNNAMED_EACH_EXPAND( \
        LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_APPLY_PARAM_UNNAMED_EACH_SHUFFLE)( \
            p_i, _LIGHTRAY_REFL_APPLY_PARAM_UNNAMED_EACH_EXPAND p_tuple\
        ) \
    )

#define _LIGHTRAY_REFL_APPLY_PARAM_UNNAMED_EACH_SHUFFLE(p_i, p_type, ...) \
    LIGHTRAY_PP_CONCAT(LightrayUnnamed, p_i) __VA_ARGS__




#define LIGHTRAY_REFL_DECL_FRIEND(p_type) \
    LIGHTRAY_REFL_DECL_FRIEND_MANGLE(p_type, MANGLE())

#define LIGHTRAY_REFL_DECL_FRIEND_MANGLE(p_type, f_mangle) \
    friend struct _LIGHTRAY_REFL_META_TYPE( \
        p_type, \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_EXTRACT_, f_mangle) \
    ); \


#define LIGHTRAY_REFL_DECL_FRIEND_TMPL(p_type, f_param) \
    LIGHTRAY_REFL_DECL_FRIEND_TMPL_MANGLE(p_type, f_param, MANGLE())

#define _LIGHTRAY_REFL_DECL_FRIEND_TMPL_MANGLE_EXPAND(...) __VA_ARGS__
#define LIGHTRAY_REFL_DECL_FRIEND_TMPL_MANGLE(p_type, f_param, f_mangle) \
    template <_LIGHTRAY_REFL_DECL_FRIEND_TMPL_MANGLE_EXPAND( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
    )> \
    friend struct _LIGHTRAY_REFL_META_TYPE( \
        p_type, \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_EXTRACT_, f_mangle) \
    ); \
    

#define _LIGHTRAY_REFL_D_TYPE(p_type, f_namespace, f_extra, f_members) \
    LIGHTRAY_PP_DEFER(LIGHTRAY_REFL_TYPE)(p_type, f_namespace, f_extra, f_members)

#define LIGHTRAY_REFL_TYPE(p_type, f_namespace, f_extra, f_members) \
    LIGHTRAY_REFL_TYPE_MANGLE(p_type, f_namespace, MANGLE(), f_extra, f_members)

#define _LIGHTRAY_REFL_D_TYPE_MANGLE(p_type, f_namespace, f_mangle, f_extra, f_members) \
    LIGHTRAY_PP_DEFER(LIGHTRAY_REFL_TYPE_MANGLE)(p_type, f_namespace, f_mangle, f_extra, f_members)

#define LIGHTRAY_REFL_TYPE_MANGLE(p_type, f_namespace, f_mangle, f_extra, f_members) \
    _LIGHTRAY_REFL_TYPE_MANGLE_EXTRACTED( \
        p_type, \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_EXTRACT_, f_namespace), \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_EXTRACT_, f_mangle), \
        f_extra, \
        f_members \
    )


#define _LIGHTRAY_REFL_D_TYPE_TMPL(p_type, f_param, f_namespace, f_extra, f_members) \
    LIGHTRAY_PP_DEFER(LIGHTRAY_REFL_TYPE_TMPL)(p_type, f_param, f_namespace, f_extra, f_members)

#define LIGHTRAY_REFL_TYPE_TMPL(p_type, f_param, f_namespace, f_extra, f_members) \
    LIGHTRAY_REFL_TYPE_TMPL_MANGLE(p_type, f_param, f_namespace, MANGLE(), f_extra, f_members)

#define _LIGHTRAY_REFL_D_TYPE_TMPL_MANGLE(p_type, f_param, f_namespace, f_mangle, f_extra, f_members) \
    LIGHTRAY_PP_DEFER(LIGHTRAY_REFL_TYPE_TMPL_MANGLE)(p_type, f_namespace, f_mangle, f_extra, f_members)

#define LIGHTRAY_REFL_TYPE_TMPL_MANGLE(p_type, f_param, f_namespace, f_mangle, f_extra, f_members) \
    _LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED( \
        p_type, \
        f_param, \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_EXTRACT_, f_namespace), \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_EXTRACT_, f_mangle), \
        f_extra, \
        f_members \
    )




#define _LIGHTRAY_REFL_TYPE_MANGLE_EXTRACTED_EXPAND(...) __VA_ARGS__
#define _LIGHTRAY_REFL_TYPE_MANGLE_EXTRACTED(p_type, p_namespace, p_mangle, f_extra, f_members) \
\
struct _LIGHTRAY_REFL_META_TYPE(p_type, p_mangle); \
\
_LIGHTRAY_REFL_META_TYPE(p_type, p_mangle) lightray_meta_get(::p_namespace::p_type const &); \
\
struct _LIGHTRAY_REFL_META_TYPE(p_type, p_mangle) \
{ \
    using type = ::p_namespace::p_type; \
    static constexpr ::lightray::fixed_string name = LIGHTRAY_PP_STRINGIZE(p_type); \
    static constexpr ::lightray::fixed_string fully_qualified_name = \
        LIGHTRAY_PP_STRINGIZE(::p_namespace::p_type); \
    static constexpr ::lightray::category category = ::lightray::category::type; \
    _LIGHTRAY_REFL_TYPE_MANGLE_EXTRACTED_EXPAND( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_, f_extra) \
    ) \
    static constexpr std::size_t member_count = \
        _LIGHTRAY_REFL_TYPE_MANGLE_EXTRACTED_EXPAND( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_COUNT_, f_members); \
        ); \
    template <std::size_t Index, typename = void> struct member; \
    _LIGHTRAY_REFL_TYPE_MANGLE_EXTRACTED_EXPAND( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_members) \
    ) \
};




#define _LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND(...) __VA_ARGS__
#define _LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED(p_type, f_param, p_namespace, p_mangle, f_extra, f_members) \
\
template <_LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
)> \
struct _LIGHTRAY_REFL_META_TYPE(p_type, p_mangle); \
\
template <_LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
)> \
_LIGHTRAY_REFL_META_TYPE(p_type, p_mangle)<_LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
)> \
lightray_meta_get(::p_namespace::p_type<_LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
)> const &); \
\
template <_LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
)> \
struct _LIGHTRAY_REFL_META_TYPE(p_type, p_mangle) \
{ \
    using type = ::p_namespace::p_type<_LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
    )>; \
    static constexpr ::lightray::fixed_string name = \
        LIGHTRAY_PP_STRINGIZE(p_type< \
            _LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
            ) \
        ); \
    \
    static constexpr ::lightray::fixed_string fully_qualified_name = \
        LIGHTRAY_PP_STRINGIZE(::p_namespace::p_type< \
            _LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
            ) \
        >); \
    static constexpr ::lightray::category category = ::lightray::category::type_template; \
    _LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_, f_extra) \
    ) \
    static constexpr std::size_t member_count = \
        _LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_COUNT_, f_members) \
        ); \
    template <std::size_t Index, typename = void> struct member; \
    _LIGHTRAY_REFL_TYPE_TMPL_MANGLE_EXTRACTED_EXPAND( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_members) \
    ) \
};







#define _LIGHTRAY_REFL_D_COUNT_MEMBERS(p_member_seq) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_COUNT_MEMBERS)(p_member_seq)

#define _LIGHTRAY_REFL_COUNT_MEMBERS(p_member_seq) LIGHTRAY_PP_SEQ_SIZE(p_member_seq)

#define _LIGHTRAY_REFL_D_DECL_MEMBERS(p_member_seq) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_MEMBERS)(p_member_seq)

#define _LIGHTRAY_REFL_DECL_MEMBERS_EXPAND(...) __VA_ARGS__
#define _LIGHTRAY_REFL_DECL_MEMBERS(p_member_seq) \
    _LIGHTRAY_REFL_DECL_MEMBERS_EXPAND( \
        LIGHTRAY_PP_SEQ_FOR_EACH( \
            _LIGHTRAY_REFL_D_DECL_MEMBERS_EACH, none, LIGHTRAY_PP_EMPTY, p_member_seq \
        ) \
    )

#define _LIGHTRAY_REFL_D_DECL_MEMBERS_EACH(d_data, p_index, p_member_tuple) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_MEMBERS_EACH)(d_data, p_index, p_member_tuple)

#define _LIGHTRAY_REFL_DECL_MEMBERS_EACH(d_data, p_index, p_member_tuple) \
    _LIGHTRAY_REFL_DECL_MEMBER_BEGIN(p_index) \
        _LIGHTRAY_REFL_DECL_MEMBERS_EACH_HELPER p_member_tuple \
    _LIGHTRAY_REFL_DECL_MEMBER_END()


#define _LIGHTRAY_REFL_DECL_MEMBERS_EACH_HELPER_EXPAND(...) __VA_ARGS__
#define _LIGHTRAY_REFL_DECL_MEMBERS_EACH_HELPER(p_member_decl, ...) \
    _LIGHTRAY_REFL_DECL_MEMBERS_EACH_HELPER_EXPAND(_LIGHTRAY_REFL_DECL_MEMBERS_EACH_HELPER_EXPAND( \
        LIGHTRAY_PP_DEFER(LIGHTRAY_PP_CALL)( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_DECL_MEMBER_HELPER_, p_member_decl), \
            __VA_ARGS__ \
        ) \
    ))


#define _LIGHTRAY_REFL_DECL_MEMBER_BEGIN(p_index) \
    template <typename Dummy> \
    struct member<p_index, Dummy> \
    { \
        using declaring_type = type;

#define _LIGHTRAY_REFL_DECL_MEMBER_END() \
    };


#define _LIGHTRAY_REFL_DECL_MEMBER_HELPER_TYPEDEF       _LIGHTRAY_REFL_D_DECL_MEMBER_TYPEDEF, 
#define _LIGHTRAY_REFL_DECL_MEMBER_HELPER_TYPEDEF_TMPL  _LIGHTRAY_REFL_D_DECL_MEMBER_TYPEDEF_TMPL, 
#define _LIGHTRAY_REFL_DECL_MEMBER_HELPER_VAR           _LIGHTRAY_REFL_D_DECL_MEMBER_VAR, 
#define _LIGHTRAY_REFL_DECL_MEMBER_HELPER_VAR_TMPL      _LIGHTRAY_REFL_D_DECL_MEMBER_VAR_TMPL, 
#define _LIGHTRAY_REFL_DECL_MEMBER_HELPER_FUNC          _LIGHTRAY_REFL_D_DECL_MEMBER_FUNC, 


#define _LIGHTRAY_REFL_D_DECL_MEMBER_TYPEDEF(p_member, f_extra) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_MEMBER_TYPEDEF)(p_member, f_extra)

#define _LIGHTRAY_REFL_DECL_MEMBER_TYPEDEF(p_member, f_extra) \
        static constexpr ::lightray::fixed_string name = LIGHTRAY_PP_STRINGIZE(p_member); \
        static constexpr ::lightray::category category = ::lightray::category::type_definition; \
        using type = typename declaring_type::p_member;




#define _LIGHTRAY_REFL_D_DECL_MEMBER_TYPEDEF_TMPL(p_member, f_param, f_extra) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_MEMBER_TYPEDEF_TMPL)(p_member, f_param, f_extra)

#define _LIGHTRAY_REFL_DECL_MEMBER_TYPEDEF_TMPL_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_REFL_DECL_MEMBER_TYPEDEF_TMPL(p_member, f_param, f_extra) \
        static constexpr ::lightray::fixed_string name = LIGHTRAY_PP_STRINGIZE(p_member); \
        static constexpr ::lightray::category category = ::lightray::category::type_definition_template; \
        template <_LIGHTRAY_REFL_DECL_MEMBER_TYPEDEF_TMPL_EXPAND( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
        )>\
        using type = typename declaring_type::template p_member< \
            _LIGHTRAY_REFL_DECL_MEMBER_TYPEDEF_TMPL_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
            ) \
        >;




#define _LIGHTRAY_REFL_D_DECL_MEMBER_VAR(p_member, f_extra) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_MEMBER_VAR)(p_member, f_extra)

#define _LIGHTRAY_REFL_DECL_MEMBER_VAR(p_member, f_extra) \
        static constexpr ::lightray::fixed_string name = LIGHTRAY_PP_STRINGIZE(p_member); \
        static constexpr ::lightray::category category = ::lightray::category::variable; \
        static constexpr auto pointer = &declaring_type::p_member;




#define _LIGHTRAY_REFL_D_DECL_MEMBER_VAR_TMPL(p_member, f_param, f_extra) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL)(p_member, f_param, f_extra)

#define _LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL(p_member, f_param, f_extra) \
        static constexpr ::lightray::fixed_string name = LIGHTRAY_PP_STRINGIZE(p_member); \
        static constexpr ::lightray::category category = ::lightray::category::variable_template; \
        \
        template <_LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
        )>  \
        static constexpr auto pointer = &declaring_type::template p_member< \
            _LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
            ) \
        >; \
        \
        template <_LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
        )> \
        using pointer_type = std::remove_const_t<decltype(pointer< \
            _LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
            ) \
        >)>; \
        \
        template <_LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
        )>\
        using type = std::remove_pointer_t<pointer_type< \
                _LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
                    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
                ) \
            >>; \
        \
        template <_LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
        )> \
        static constexpr decltype(auto) invoke(std::nullptr_t) noexcept \
        { \
            return *pointer<_LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
                    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
                )>; \
        } \
        template <_LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
            LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
        ), typename Self> \
        requires std::is_base_of_v<declaring_type, std::remove_cvref_t<Self>> \
        static constexpr decltype(auto) invoke(Self &&self) noexcept \
        { \
            return *pointer<_LIGHTRAY_REFL_DECL_MEMBER_VAR_TMPL_EXPAND( \
                    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
                )>; \
        }




#define _LIGHTRAY_REFL_D_DECL_MEMBER_FUNC(p_member, f_overload, f_extra) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_DECL_MEMBER_FUNC)(p_member, f_overload, f_extra)

#define _LIGHTRAY_REFL_DECL_MEMBER_FUNC_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_REFL_DECL_MEMBER_FUNC(p_member, f_overload, f_extra) \
        static constexpr ::lightray::fixed_string name = LIGHTRAY_PP_STRINGIZE(p_member); \
        static constexpr ::lightray::category category = ::lightray::category::function; \
        _LIGHTRAY_REFL_DECL_MEMBER_FUNC_EXPAND(LIGHTRAY_PP_CALL( \
            _LIGHTRAY_REFL_DECL_MEMBER_FUNC_EXPAND( \
                LIGHTRAY_PP_DEFER(LIGHTRAY_PP_VARIADIC_HEAD)( \
                    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_HELPER_, f_overload) \
                ) \
            ), \
            p_member, \
            _LIGHTRAY_REFL_DECL_MEMBER_FUNC_EXPAND( \
                LIGHTRAY_PP_DEFER(LIGHTRAY_PP_VARIADIC_REST)( \
                    LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_HELPER_, f_overload) \
                ) \
            ) \
        ))





#define _LIGHTRAY_REFL_HELPER_OVERLOAD(...) _LIGHTRAY_REFL_D_OVERLOAD, __VA_ARGS__
#define _LIGHTRAY_REFL_D_OVERLOAD(...) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_OVERLOAD)(__VA_ARGS__)


#define _LIGHTRAY_REFL_OVERLOAD_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_REFL_OVERLOAD(p_member, ...) \
    _LIGHTRAY_REFL_OVERLOAD_EXPAND(LIGHTRAY_PP_VARIADIC_FOR_EACH( \
        _LIGHTRAY_REFL_OVERLOAD_EACH, p_member, LIGHTRAY_PP_EMPTY, __VA_ARGS__ \
    ))

#define _LIGHTRAY_REFL_OVERLOAD_EACH_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_REFL_OVERLOAD_EACH(p_member, d_i, f_overload_type) \
    _LIGHTRAY_REFL_OVERLOAD_EACH_EXPAND(LIGHTRAY_PP_DEFER(LIGHTRAY_PP_CALL)( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_OVERLOAD_HELPER_, f_overload_type), p_member \
    ))

#define _LIGHTRAY_REFL_OVERLOAD_HELPER_STATIC() \
    _LIGHTRAY_REFL_D_OVERLOAD_STATIC
#define _LIGHTRAY_REFL_OVERLOAD_HELPER_NON_STATIC() \
    _LIGHTRAY_REFL_D_OVERLOAD_NON_STATIC
#define _LIGHTRAY_REFL_OVERLOAD_HELPER_STATIC_TMPL(f_param) \
    _LIGHTRAY_REFL_D_OVERLOAD_STATIC_TMPL, f_param
#define _LIGHTRAY_REFL_OVERLOAD_HELPER_NON_STATIC_TMPL(f_param) \
    _LIGHTRAY_REFL_D_OVERLOAD_NON_STATIC_TMPL, f_param


#define _LIGHTRAY_REFL_D_OVERLOAD_STATIC(p_member) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_OVERLOAD_STATIC)(p_member)

#define _LIGHTRAY_REFL_OVERLOAD_STATIC(p_member) \
    template <typename ...Args> \
    static constexpr decltype(auto) invoke(std::nullptr_t, Args &&...args) \
    noexcept(noexcept(declaring_type::p_member(std::forward<Args>(args)...))) \
    { return declaring_type::p_member(std::forward<Args>(args)...); }


#define _LIGHTRAY_REFL_D_OVERLOAD_NON_STATIC(p_member) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_OVERLOAD_NON_STATIC)(p_member)

#define _LIGHTRAY_REFL_OVERLOAD_NON_STATIC(p_member) \
    template <typename Self, typename ...Args> \
    requires std::is_base_of_v<declaring_type, std::remove_cvref_t<Self>> \
    static constexpr decltype(auto) invoke(Self &&self, Args &&...args) \
    noexcept(noexcept( \
        std::forward<Self>(self).declaring_type::p_member(std::forward<Args>(args)...) \
    )) \
    { return std::forward<Self>(self).declaring_type::p_member(std::forward<Args>(args)...); }


#define _LIGHTRAY_REFL_OVERLOAD_STATIC_TMPL_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_REFL_D_OVERLOAD_STATIC_TMPL(f_param, p_member) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_OVERLOAD_STATIC_TMPL)(f_param, p_member)

#define _LIGHTRAY_REFL_OVERLOAD_STATIC_TMPL(f_param, p_member) \
    template <_LIGHTRAY_REFL_OVERLOAD_STATIC_TMPL_EXPAND( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
    ), typename ...Args> \
    static constexpr decltype(auto) invoke(std::nullptr_t, Args &&...args) \
    noexcept(noexcept( \
        declaring_type::template p_member< \
            _LIGHTRAY_REFL_OVERLOAD_STATIC_TMPL_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
            ) \
        >(std::forward<Args>(args)...) \
    )) \
    { \
        return declaring_type::template p_member< \
            _LIGHTRAY_REFL_OVERLOAD_STATIC_TMPL_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
            ) \
        >(std::forward<Args>(args)...); \
    }


#define _LIGHTRAY_REFL_OVERLOAD_NON_STATIC_TMPL_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_REFL_D_OVERLOAD_NON_STATIC_TMPL(f_param, p_member) \
    LIGHTRAY_PP_DEFER(_LIGHTRAY_REFL_OVERLOAD_NON_STATIC_TMPL)(f_param, p_member)

#define _LIGHTRAY_REFL_OVERLOAD_NON_STATIC_TMPL(f_param, p_member) \
    template <_LIGHTRAY_REFL_OVERLOAD_NON_STATIC_TMPL_EXPAND( \
        LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_DECL_, f_param) \
    ), typename Self, typename ...Args> \
    requires std::is_base_of_v<declaring_type, std::remove_cvref_t<Self>> \
    static constexpr decltype(auto) invoke(Self &&self, Args &&...args) \
    noexcept(noexcept( \
        std::forward<Self>(self).declaring_type::template p_member< \
            _LIGHTRAY_REFL_OVERLOAD_NON_STATIC_TMPL_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
            ) \
        >(std::forward<Args>(args)...) \
    )) \
    { \
        return std::forward<Self>(self).declaring_type::template p_member< \
            _LIGHTRAY_REFL_OVERLOAD_NON_STATIC_TMPL_EXPAND( \
                LIGHTRAY_PP_CONCAT(_LIGHTRAY_REFL_D_APPLY_, f_param) \
            ) \
        >(std::forward<Args>(args)...); \
    }




#endif

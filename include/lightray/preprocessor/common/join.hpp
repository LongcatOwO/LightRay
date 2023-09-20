#ifndef _LIGHTRAY_PREPROCESSOR_COMMON_JOIN_HPP_
#define _LIGHTRAY_PREPROCESSOR_COMMON_JOIN_HPP_


#include <lightray/preprocessor/common/concat.hpp>
#include <lightray/preprocessor/common/defer.hpp>
#include <lightray/preprocessor/common/repeat.hpp>
#include <lightray/preprocessor/common/symbols.hpp>
#include <lightray/preprocessor/variadic/for_each.hpp>
#include <lightray/preprocessor/variadic/size.hpp>


#define _LIGHTRAY_PP_D_JOIN(delim_func, ...) \
    LIGHTRAY_PP_DEFER(LIGHTRAY_PP_JOIN)(delim_func, __VA_ARGS__)

#define LIGHTRAY_PP_JOIN(delim_func, ...) \
    __VA_OPT__(_LIGHTRAY_PP_JOIN_EXPAND(_LIGHTRAY_PP_JOIN(delim_func, __VA_ARGS__)))

#define _LIGHTRAY_PP_JOIN_EXPAND(...) __VA_ARGS__

#define _LIGHTRAY_PP_JOIN(delim_func, head, ...) \
    _LIGHTRAY_PP_JOIN_LHS(__VA_ARGS__) head _LIGHTRAY_PP_JOIN_RHS(delim_func, __VA_ARGS__)

#define _LIGHTRAY_PP_JOIN_LHS(...) \
    LIGHTRAY_PP_REPEAT(_LIGHTRAY_PP_JOIN_LHS_EACH, none, LIGHTRAY_PP_EMPTY, LIGHTRAY_PP_VARIADIC_SIZE(__VA_ARGS__)) \
    LIGHTRAY_PP_REPEAT(_LIGHTRAY_PP_JOIN_LHS_EACH, none, LIGHTRAY_PP_EMPTY, LIGHTRAY_PP_VARIADIC_SIZE(__VA_ARGS__))

#define _LIGHTRAY_PP_JOIN_LHS_EACH(data, i) LIGHTRAY_PP_DEFER(LIGHTRAY_PP_CONCAT)(


#define _LIGHTRAY_PP_JOIN_RHS(delim_func, ...) \
    LIGHTRAY_PP_VARIADIC_FOR_EACH(_LIGHTRAY_PP_JOIN_RHS_EACH, delim_func, LIGHTRAY_PP_EMPTY, __VA_ARGS__)
    
#define _LIGHTRAY_PP_JOIN_RHS_EACH(delim_func, i, elem) \
    , LIGHTRAY_PP_DEFER(delim_func)()), elem)



#endif

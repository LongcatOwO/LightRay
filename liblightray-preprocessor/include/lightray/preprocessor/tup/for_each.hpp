#ifndef _LIGHTRAY_PREPROCESSOR_TUP_FOR_EACH_HPP_
#define _LIGHTRAY_PREPROCESSOR_TUP_FOR_EACH_HPP_

#include <lightray/preprocessor/var/for_each.hpp>

#define LIGHTRAY_PP_TUP_FOR_EACH(macro, data, tup) \
    LIGHTRAY_PP_VAR_FOR_EACH(macro, data, LIGHTRAY_PP_TUP_FOR_EACH_EXPAND tup)
#define LIGHTRAY_PP_TUP_FOR_EACH_EXPAND(...) __VA_ARGS__

#endif

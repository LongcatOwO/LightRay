#ifndef _LIGHTRAY_PREPROCESSOR_TUP_ELEM_HPP_
#define _LIGHTRAY_PREPROCESSOR_TUP_ELEM_HPP_

#include <lightray/preprocessor/var/elem.hpp>

#define LIGHTRAY_PP_TUP_ELEM(n, tup) LIGHTRAY_PP_VAR_ELEM(n, LIGHTRAY_PP_TUP_ELEM_EXPAND tup)
#define LIGHTRAY_PP_TUP_ELEM_EXPAND(...) __VA_ARGS__

#endif

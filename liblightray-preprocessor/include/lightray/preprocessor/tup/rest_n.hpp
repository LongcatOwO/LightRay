#ifndef _LIGHTRAY_PREPROCESSOR_TUP_REST_N_HPP_
#define _LIGHTRAY_PREPROCESSOR_TUP_REST_N_HPP_

#include <lightray/preprocessor/var/rest_n.hpp>

#define LIGHTRAY_PP_TUP_REST_N(n, tup) (LIGHTRAY_PP_VAR_REST_N(n, LIGHTRAY_PP_TUP_REST_N_EXPAND tup))
#define LIGHTRAY_PP_TUP_REST_N_EXPAND(...) __VA_ARGS__

#endif

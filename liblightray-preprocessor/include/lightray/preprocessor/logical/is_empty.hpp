#ifndef _LIGHTRAY_PREPROCESSOR_LOGICAL_IS_EMPTY_HPP_
#define _LIGHTRAY_PREPROCESSOR_LOGICAL_IS_EMPTY_HPP_

#include <lightray/preprocessor/common.hpp>

/*
 * Expands to 1 if the argument is empty, otherwise expands to 0.
 */
#define LIGHTRAY_PP_IS_EMPTY(...) LIGHTRAY_PP_CAT(LIGHTRAY_PP_IS_EMPTY_I, __VA_OPT__(I))()
#define LIGHTRAY_PP_IS_EMPTY_I() 1
#define LIGHTRAY_PP_IS_EMPTY_II() 0

#endif

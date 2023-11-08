#ifndef _LIGHTRAY_PREPROCESSOR_LOGICAL_IF_HPP_
#define _LIGHTRAY_PREPROCESSOR_LOGICAL_IF_HPP_

#include <lightray/preprocessor/common.hpp>

#include "bool.hpp"

/*
 * Converts cond to bool. Then, 
 *  if cond is 1, expands to t
 *  if cond is 0, expands to f
 *
 * Author: P. Lutchanont
 */
#define LIGHTRAY_PP_IF(cond, t, f) LIGHTRAY_PP_CAT(LIGHTRAY_PP_IF_, LIGHTRAY_PP_BOOL(cond))(t, f)
#define LIGHTRAY_PP_IF_0(t, f) f
#define LIGHTRAY_PP_IF_1(t, f) t

#endif

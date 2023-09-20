#ifndef _LIGHTRAY_PREPROCESSOR_COMMON_DEFER_HPP_
#define _LIGHTRAY_PREPROCESSOR_COMMON_DEFER_HPP_

#include <lightray/preprocessor/common/empty.hpp>

#define LIGHTRAY_PP_DEFER(...) __VA_ARGS__ LIGHTRAY_PP_EMPTY()

#endif

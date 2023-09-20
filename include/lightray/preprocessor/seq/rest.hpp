#ifndef _LIGHTRAY_PREPROCESSOR_SEQ_REST_HPP_
#define _LIGHTRAY_PREPROCESSOR_SEQ_REST_HPP_


#include <lightray/preprocessor/common/defer.hpp>


#define LIGHTRAY_PP_SEQ_REST(seq) _LIGHTRAY_PP_SEQ_REST_EXPAND( \
        LIGHTRAY_PP_DEFER(_LIGHTRAY_PP_SEQ_REST_HELPER_0)( \
            _LIGHTRAY_PP_SEQ_REST_HELPER_1 seq \
        ) \
    ) 

#define _LIGHTRAY_PP_D_SEQ_REST(seq) LIGHTRAY_PP_DEFER(LIGHTRAY_PP_SEQ_REST)(seq)


#define _LIGHTRAY_PP_SEQ_REST_EXPAND(...) __VA_ARGS__
#define _LIGHTRAY_PP_SEQ_REST_HELPER_0(head, ...) __VA_ARGS__
#define _LIGHTRAY_PP_SEQ_REST_HELPER_1(...) (__VA_ARGS__),


#endif

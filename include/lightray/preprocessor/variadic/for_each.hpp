#ifndef _LIGHTRAY_PREPROCESSOR_VARIADIC_FOR_EACH_HPP_
#define _LIGHTRAY_PREPROCESSOR_VARIADIC_FOR_EACH_HPP_


#include <lightray/preprocessor/arithmetic/increment.hpp>
#include <lightray/preprocessor/common/concat.hpp>
#include <lightray/preprocessor/common/defer.hpp>
#include <lightray/preprocessor/common/eval.hpp>
#include <lightray/preprocessor/variadic/size.hpp>


#define LIGHTRAY_PP_D_VARIADIC_FOR_EACH(macro, data, sep_func, ...) \
    LIGHTRAY_PP_DEFER(LIGHTRAY_PP_VARIADIC_FOR_EACH)(macro, data, sep_func, __VA_ARGS__)


#define LIGHTRAY_PP_VARIADIC_FOR_EACH(macro, data, sep_func, ...) \
    LIGHTRAY_PP_CONCAT(_LIGHTRAY_PP_VARIADIC_FOR_EACH_, LIGHTRAY_PP_VARIADIC_SIZE(__VA_ARGS__))(macro, data, sep_func, 0, __VA_ARGS__)


#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_0(macro, data, sep_func, i, ...)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_1(macro, data, sep_func, i, elem, ...)  macro(data, i, elem)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_2(macro, data, sep_func, i, elem, ...)  macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_1 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_3(macro, data, sep_func, i, elem, ...)  macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_2 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_4(macro, data, sep_func, i, elem, ...)  macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_3 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_5(macro, data, sep_func, i, elem, ...)  macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_4 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_6(macro, data, sep_func, i, elem, ...)  macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_5 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_7(macro, data, sep_func, i, elem, ...)  macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_6 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_8(macro, data, sep_func, i, elem, ...)  macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_7 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_9(macro, data, sep_func, i, elem, ...)  macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_8 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_10(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_9 (macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_11(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_10(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_12(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_11(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_13(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_12(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_14(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_13(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_15(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_14(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_16(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_15(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_17(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_16(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_18(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_17(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_19(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_18(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_20(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_19(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_21(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_20(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_22(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_21(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_23(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_22(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_24(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_23(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_25(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_24(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_26(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_25(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_27(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_26(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_28(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_27(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_29(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_28(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_30(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_29(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_31(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_30(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_32(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_31(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_33(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_32(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_34(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_33(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_35(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_34(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_36(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_35(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_37(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_36(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_38(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_37(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_39(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_38(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_40(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_39(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_41(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_40(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_42(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_41(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_43(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_42(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_44(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_43(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_45(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_44(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_46(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_45(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_47(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_46(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_48(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_47(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_49(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_48(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_50(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_49(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_51(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_50(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_52(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_51(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_53(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_52(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_54(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_53(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_55(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_54(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_56(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_55(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_57(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_56(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_58(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_57(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_59(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_58(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_60(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_59(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_61(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_60(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_62(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_61(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_63(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_62(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)
#define _LIGHTRAY_PP_VARIADIC_FOR_EACH_64(macro, data, sep_func, i, elem, ...) macro(data, i, elem) sep_func() _LIGHTRAY_PP_VARIADIC_FOR_EACH_63(macro, data, sep_func, LIGHTRAY_PP_INCREMENT(i), __VA_ARGS__)



#endif

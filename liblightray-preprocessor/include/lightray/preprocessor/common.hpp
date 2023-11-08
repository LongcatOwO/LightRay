#ifndef _LIGHTRAY_PREPROCESSOR_COMMON_HPP_
#define _LIGHTRAY_PREPROCESSOR_COMMON_HPP_

/*
 * Concatenate both the arguments together.
 *
 * Author: P. Lutchanont
 */
#define LIGHTRAY_PP_CAT(a, b) LIGHTRAY_PP_CAT_(a, b)
#define LIGHTRAY_PP_CAT_(a, b) a##b

/*
 * Stringize the arguments.
 *
 * Author: P. Lutchanont
 */
#define LIGHTRAY_PP_STRINGIZE(...) LIGHTRAY_PP_STRINGIZE_(__VA_ARGS__)
#define LIGHTRAY_PP_STRINGIZE_(...) #__VA_ARGS__

/*
 * Expands to nothing.
 *
 * Author: P. Lutchanont
 */
#define LIGHTRAY_PP_EMPTY(...)

/*
 * Defers the macro evaluation of its argument and the token after the defer call.
 * For example:
 *
 *  LIGHTRAY_PP_DEFER(MACRO)(arg0, arg1, arg2)
 *
 * Useful for implementing recursive macros by delaying the expansion,
 * preventing the macro's identifier from being 'blue-painted'
 *
 * Author: P. Lutchanont
 */
#define LIGHTRAY_PP_DEFER(...) __VA_ARGS__ LIGHTRAY_PP_EMPTY()

/*
 * Evaluates to its own arguments. Use cases are:
 *  - consuming bracket
 *  - trigger the evaluation of deferred macro calls from LIGHTRAY_PP_DEFER.
 *
 * Author: P. Lutchanont
 */
#define LIGHTRAY_PP_EXPAND(...) __VA_ARGS__

/*
 * Expands to literal comma ','.
 * Useful for passing comma as macro argument.
 *
 * Author: P. Lutchanont
 */
#define LIGHTRAY_PP_COMMA() ,

#endif

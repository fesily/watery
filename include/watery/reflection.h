#pragma once
#ifndef REFLECTION_H
#define REFLECTION_H
#include "reflection_macro.h"
//----- reflection object -------
#define WATERY_REFLECTION(STRUCT_NAME, ...) \
IGUANA_MAKE_META_DATA(STRUCT_NAME, GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

//----- reflection template object ------
#define WATERY_REFLECTION_TEMPLATE(STRUCT_NAME,			\
							TYPENAME_N, ...)		\
IGUANA_MAKE_META_DATA_TEMPLATE1(STRUCT_NAME,								\
								MAKE_ARG_TYPENAME(TYPENAME_N),				\
								GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
//----- reflection partial template object ------
#define WATERY_REFLECTION_TEMPLATE_PARTIAL(STRUCT_NAME,				\
									STRUCT_TYPENAME,			\
									TYPENAME_N, ...)			\
IGUANA_MAKE_META_DATA_TEMPLATE(STRUCT_NAME,												\
							   MACRO_FORWARD(STRUCT_TYPENAME),							\
							   IGUANA_TEMPLATE_LIST(MAKE_ARG_TYPENAME(TYPENAME_N)),		\
							   GET_ARG_COUNT(__VA_ARGS__),__VA_ARGS__)

//----- reflection full partial template object ------
#define WATERY_REFLECTION_TEMPLATE_FULL(STRUCT_NAME,								\
								STRUCT_TYPENAME,...)						\
IGUANA_MAKE_META_DATA_TEMPLATE(STRUCT_NAME,									\
								MACRO_FORWARD(STRUCT_TYPENAME), ,			\
								GET_ARG_COUNT(__VA_ARGS__),__VA_ARGS__)
#endif // REFLECTION_H

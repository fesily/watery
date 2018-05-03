#pragma once
#ifndef REFLECTION_H
#define REFLECTION_H
#include <watery\reflection_macro.h>
/**
 * \brief reflection object,
 *		if need support overload function,
 *		please use WATERY_REFLECTION
 *			ex:WATERY_SIMAPLE_REFLECTION(CLASS,FUNCTION1,FUNCTION2,OBJECT1,OBJECT2)
 * \param STRUCT_NAME name
 */
#define WATERY_SIMAPLE_REFLECTION(STRUCT_NAME, ...) \
WATERY_MAKE_SIMAPLE_META_DATA(STRUCT_NAME,WATERY_EMPTY_BASE_TYPE(), WATERY_GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)


 /**
 * \brief reflection object , every object need().
 *			This support overload function!
 *			ex: WATERY_REFLECTION(CLASS,(OBJECT1),(FUNCTION1),(OBJECT2),(void(),OVERLOAD),(void(int),OVERLOAD))
 * \param STRUCT_NAME name
 */
#define WATERY_WITH_BASE_REFLECTION(STRUCT_NAME,BASE_TYPE, ...) \
WATERY_REFLEX_DEFINE_IMPL(STRUCT_NAME,BASE_TYPE, WATERY_GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define WATERY_REFLECTION(STRUCT_NAME, ...) \
WATERY_REFLEX_DEFINE_IMPL(STRUCT_NAME,WATERY_EMPTY_BASE_TYPE(),WATERY_GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

/**
 * \brief reflection enum object
 * \param STRUCT_NAME name
 */
#define WATERY_ENUM_REFLECTION(ENUM_NAME, ...) \
WATERY_MAKE_ENUM_META_DATA(ENUM_NAME, WATERY_GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
/**
 * \brief reflection template object
 * \param STRUCT_NAME name
 * \param TYPENAME_N number of typename
 */
#define WATERY_REFLECTION_TEMPLATE(STRUCT_NAME,								\
							TYPENAME_N, ...)								\
IGUANA_MAKE_META_DATA_TEMPLATE1(STRUCT_NAME,								\
								MAKE_ARG_TYPENAME(TYPENAME_N),				\
								WATERY_GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)
/*
 * example:
 *  template<typename T1,typename T2> Test{T1 a; void b(T2){}};
 *  WATERY_REFLECTION_TEMPLATE_PARTIAL(Test,MACRO_FORWARD(int,T1),1,a,b)
 */
/**
 * \brief reflection partial template object
 * \param STRUCT_NAME name
 * \param STRUCT_TYPENAME partial typename (example: Test<int,T1>, here use : int,T1),typename must be T1,T2,T3,T4....
 * \param TYPENAME_N number of typename
 */
#define WATERY_REFLECTION_TEMPLATE_PARTIAL(STRUCT_NAME,			\
									STRUCT_TYPENAME,			\
									TYPENAME_N, ...)			\
IGUANA_MAKE_META_DATA_TEMPLATE0(TEMPLATE_MACRO,											\
								TEMPLATE_MACRO,											\
								STRUCT_NAME,											\
								MACRO_FORWARD(STRUCT_TYPENAME),							\
							   IGUANA_TEMPLATE_LIST(MAKE_ARG_TYPENAME(TYPENAME_N)),		\
							   WATERY_GET_ARG_COUNT(__VA_ARGS__),__VA_ARGS__)

/**
 * \brief reflection full partial template object
 * \param STRUCT_NAME name
 * \param STRUCT_TYPENAME full typename name (example: Test<int,double>,here use : int,double)
 */
#define WATERY_REFLECTION_TEMPLATE_FULL(STRUCT_NAME,						\
								STRUCT_TYPENAME,...)						\
IGUANA_MAKE_META_DATA_TEMPLATE0(TEMPLATE_MACRO_FULL,						\
								TEMPLATE_MACRO_NULL,						\
								STRUCT_NAME,								\
								MACRO_FORWARD(STRUCT_TYPENAME), ,			\
								WATERY_GET_ARG_COUNT(__VA_ARGS__),__VA_ARGS__)
#endif // REFLECTION_H

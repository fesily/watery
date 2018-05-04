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
#endif // REFLECTION_H

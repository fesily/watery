#pragma once
#ifndef REFLECTION_STRUCT_H
#define REFLECTION_STRUCT_H
#include <type_traits>
#include <array>
#if 0
template<typename T>
struct iguana_reflect_members : std::false_type
{

};

#define IGUANA_MAKE_META_DATA_IMPL(N,						\
									STRUCT_NAME,			\
									STRUCT_TYPE,			\
									TYPENAME_LIST,			\
									ARR_NAME, ...)			\
template<TYPENAME_LIST>										\
struct iguana_reflect_members<STRUCT_TYPE> : std::true_type{\
using struct_type = STRUCT_TYPE;							\
static auto apply_impl()									\
			-> decltype(std::make_tuple(__VA_ARGS__)) {		\
		return std::make_tuple(__VA_ARGS__);				\
}															\
static const size_t all_member_size = N;					\
static const char* name() { return ADD_VIEW(STRUCT_NAME); }	\
static std::array<const char*, N> arr()						\
{ return { ARR_NAME }; }									\
}; 
#else
namespace watery
{
	struct null_reflect : std::false_type{};
}
watery::null_reflect ____watery_reflect_invoker(...);

#define IGUANA_MAKE_META_DATA_IMPL(TEMPLATE_MARCO0,TEMPLATE_MARCO1,\
									N,						\
									STRUCT_NAME,			\
									STRUCT_TYPE,			\
									ARR_NAME, ...)			\
namespace reflect_details{									\
template<typename... T>										\
struct STRUCT_NAME##iguana_reflect_members;					\
TEMPLATE_MARCO0												\
struct STRUCT_NAME##iguana_reflect_members<STRUCT_TYPE>		\
							: std::true_type{				\
using struct_type = STRUCT_TYPE;							\
static auto apply_impl()									\
			-> decltype(std::make_tuple(__VA_ARGS__)) {		\
		return std::make_tuple(__VA_ARGS__);				\
}															\
static const size_t all_member_size = N;					\
static const char* name() { return ADD_VIEW(STRUCT_NAME); }	\
static std::array<const char*, N> arr()						\
{ return { ARR_NAME }; }									\
};															\
}															\
TEMPLATE_MARCO1												\
reflect_details::STRUCT_NAME##iguana_reflect_members<STRUCT_TYPE> ____watery_reflect_invoker(STRUCT_TYPE);

#endif
#endif // REFLECTION_STRUCT_H

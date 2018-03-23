#pragma once
#ifndef REFLECTION_STRUCT_H
#define REFLECTION_STRUCT_H
#include <type_traits>
#include <array>
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
#endif // REFLECTION_STRUCT_H

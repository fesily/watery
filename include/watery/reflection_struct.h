#pragma once
#ifndef REFLECTION_STRUCT_H
#define REFLECTION_STRUCT_H
#include <type_traits>
#include <array>

namespace watery
{
	template<typename T, T value>
	struct reflex_enum : std::integral_constant<T,value>
	{
		static_assert(std::is_enum_v<T>);
		using enum_type = T;
	};
	template<typename T, T value>
	struct reflex_member_object;
	template<typename Obj, typename T, T(Obj::*ptr)>
	struct reflex_member_object<T(Obj::*), ptr> :std::integral_constant<T(Obj::*), ptr>
	{
		using object_type = Obj;
		using value_type = T;
	};
	template<typename T, T value>
	struct reflex_member_function;

#define _REFLEX_INFO(CALL_OPT, CV_OPT, REF_OPT, NOEXCEPT_OPT)\
	template<typename Obj, typename Ret,typename ...Args, Ret(CALL_OPT Obj::*ptr)(Args...) CV_OPT REF_OPT NOEXCEPT_OPT>\
	struct reflex_member_function<Ret(CALL_OPT Obj::*)(Args...) CV_OPT REF_OPT NOEXCEPT_OPT, ptr> \
		:std::integral_constant<Ret(CALL_OPT Obj::*)(Args...) CV_OPT REF_OPT NOEXCEPT_OPT, ptr>\
	{\
		using object_type = Obj;\
		using result_type = Ret;\
		using param_type = std::tuple<Args...>;\
	};
#ifdef _MSC_VER
	_MEMBER_CALL_CV_REF_NOEXCEPT(_REFLEX_INFO)
#endif
#undef _REFLEX_INFO
#define _REFLEX_INFO(CV_REF_NOEXCEPT_OPT)\
	template<typename Obj, typename Ret,typename ...Args, Ret(Obj::*ptr)(Args...,...) CV_REF_NOEXCEPT_OPT>\
	struct reflex_member_function<Ret(Obj::*)(Args...,...) CV_REF_NOEXCEPT_OPT, ptr> \
		:std::integral_constant<Ret(Obj::*)(Args...,...) CV_REF_NOEXCEPT_OPT, ptr>\
	{\
		using object_type = Obj;\
		using result_type = Ret;\
		using param_type = std::tuple<Args...>;\
	};
		_CLASS_DEFINE_CV_REF_NOEXCEPT(_REFLEX_INFO)
#undef _REFLEX_INFO

	template<typename Obj, typename ... Functions>
	struct reflex_overload_function
	{
		using object_type = Obj;
		using overload_type = std::tuple<Functions...>;
	};

	template < template <typename...> class U, typename T >
	struct is_template_instant_of : std::false_type {};

	template < template <typename...> class U, typename... args >
	struct is_template_instant_of< U, U<args...> > : std::true_type {};

	template<typename T>
	struct is_reflex_member_function :std::false_type {};

	template<typename T,T value>
	struct is_reflex_member_function<reflex_member_function<T, value>> :std::true_type {};


	template<typename T>
	struct is_reflex_member_object :std::false_type {};

	template<typename T, T value>
	struct is_reflex_member_object<reflex_member_object<T, value>> :std::true_type {};



	template<typename T>
	struct is_reflex_overload_function : is_template_instant_of < reflex_overload_function, T >
	{};

	struct null_reflect : std::false_type{};
	template<typename T>
	struct reflex_sfinae
	{
		template<T value>
		using type =
			std::conditional_t<std::is_enum_v<T>, reflex_enum<T, value>,
						std::conditional_t<std::is_member_object_pointer_v<T>,reflex_member_object<T,value>, 
							reflex_member_function<T, value>>>;
	};
	template<typename T, T value>
	using reflex_info = typename reflex_sfinae<T>::template type<value>;
}
watery::null_reflect ____watery_reflect_invoker(...);

#define WATERY_REFLEX_NAMESPACE_BEGIN_1(NAME) namespace reflect_details{ struct NAME##_iguana_reflect_members
#define WATERY_REFLEX_NAMESPACE_END_1 }
#define WATERY_REFLEX_NAMESPACE_1(NAME) reflect_details::NAME##_iguana_reflect_members 
#define WATERY_REFLEX_NAMESPACE_BEGIN_0(NAME) struct __reflecx__
#define WATERY_REFLEX_NAMESPACE_END_0
#define WATERY_REFLEX_NAMESPACE_0(NAME) friend __reflecx__
#define WATERY_REFLEX_NAMESPACE_BEGIN(n,NAME) WATERY_CAT(WATERY_REFLEX_NAMESPACE_BEGIN_,n)(NAME)
#define WATERY_REFLEX_NAMESPACE_END(n) WATERY_CAT(WATERY_REFLEX_NAMESPACE_END_,n)
#define WATERY_REFLEX_NAMESPACE(n,NAME) WATERY_CAT(WATERY_REFLEX_NAMESPACE_,n)(NAME)

#define WATERY_EMPTY_BASE_TYPE() void

#define WATERY_META_DATA(IN_CLASS,NAME,TYPE,BASE,ARR_NAME,ARR_REFLX)\
WATERY_REFLEX_NAMESPACE_BEGIN(IN_CLASS,NAME)				\
							: std::true_type{				\
using base_type = BASE;										\
using struct_type = TYPE;									\
using reflex_type = std::tuple<ARR_REFLX>;					\
static const size_t all_member_size = std::tuple_size_v<reflex_type>;	\
constexpr static WATERY_STRING name() { return WATERY_ADD_VIEW(NAME); }			\
constexpr static std::array<WATERY_STRING, all_member_size> arr()			\
{ return { ARR_NAME }; }									\
};															\
WATERY_REFLEX_NAMESPACE_END(IN_CLASS)						\
WATERY_REFLEX_NAMESPACE(IN_CLASS,NAME) ____watery_reflect_invoker(TYPE);


#define IGUANA_MAKE_META_DATA_IMPL(TEMPLATE_MACRO0,TEMPLATE_MACRO1,\
									N,						\
									STRUCT_NAME,			\
									STRUCT_TYPE,			\
									ARR_NAME, ...)			\
namespace reflect_details{									\
template<typename... T>										\
struct STRUCT_NAME##_iguana_reflect_members;				\
TEMPLATE_MACRO0												\
struct STRUCT_NAME##_iguana_reflect_members<STRUCT_TYPE>	\
							: std::true_type{				\
using struct_type = STRUCT_TYPE;							\
using reflex_type = std::tuple<__VA_ARGS__>;				\
static const size_t all_member_size = N;					\
constexpr static WATERY_STRING name() { return WATERY_ADD_VIEW(STRUCT_NAME); }	\
constexpr static std::array<WATERY_STRING, N> arr()						\
{ return { ARR_NAME }; }									\
};															\
}															\
TEMPLATE_MACRO1												\
reflect_details::STRUCT_NAME##_iguana_reflect_members<STRUCT_TYPE> ____watery_reflect_invoker(STRUCT_TYPE);

#endif // REFLECTION_STRUCT_H

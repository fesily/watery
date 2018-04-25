#pragma once
#ifndef REFLECTION_STRUCT_H
#define REFLECTION_STRUCT_H
#include <type_traits>
#include <array>

namespace watery
{
	namespace details
	{
	template <typename Tuple, size_t Index, typename F>
	constexpr size_t for_each_finder_tuple_meta_impl(F&& f)
	{
		if (f(std::tuple_element_t<Index, Tuple>{}, std::integral_constant<size_t, Index>{}))
		{
			return Index;
		}
		else {
			if constexpr(Index > 0)
			{
				return for_each_finder_tuple_meta_impl<Tuple, Index - 1>(std::forward<F>(f));
			}
			else
			{
				return -1;
			}
		}
	}
	template <typename Tuple, typename F>
	constexpr size_t for_each_finder_tuple_meta(F&& f)
	{
		constexpr auto index = std::tuple_size_v<Tuple> -1;
		static_assert(index >= 0);
		return for_each_finder_tuple_meta_impl<Tuple, index>(std::forward<F>(f));
	}
	}
	struct null_reflect : std::false_type{};
	template<typename T, T value>
	struct reflex_info : std::integral_constant<T, value>
	{
		static_assert(std::is_enum_v<T>);
		using enum_type = T;
	};
	template<typename Obj,typename T,T (Obj::*ptr)>
	struct reflex_info<T(Obj::*), ptr> :std::integral_constant<T(Obj::*), ptr>
	{
		using object_type = Obj;
		using value_type = T;
	};
	template<typename Tuple,typename ...Args>
	struct is_overload_match : std::false_type{};
	template<typename ...Args,typename ...Args1>
	struct is_overload_match<std::tuple<Args...>, Args1...> : std::is_invocable<void(Args...), Args1...> {};
	template<typename Obj,typename Tuple>
	struct reflex_function_overload;
	template<typename Overload,typename Obj, typename ...Args>
	auto invoke(Obj&& obj, Args&&... args)
	{
		using overload_t = typename Overload::overloads_type;
		using object_t = typename Overload::object_type;
		static_assert(std::is_same_v<std::decay_t<Obj>, object_t>);
		constexpr auto index = details::for_each_finder_tuple_meta<overload_t>(
			[](auto fn, auto index) constexpr {
				return is_overload_match<decltype(fn)::param_type,Args...>::value;
		});
		static_assert(index != -1, "args is not match");
		using type = std::tuple_element_t<index, overload_t>;
		return std::invoke(type::value, std::forward<Obj>(obj), std::forward<Args>(args)...);
	}
	template<typename Obj, typename ... Functions>
	struct reflex_function_overload<Obj, std::tuple<Functions...>>
	{
		using object_type = Obj; 
		using overloads_type = std::tuple<Functions...>;
	};

#define _REFLEX_INFO(CALL_OPT, CV_OPT, REF_OPT, NOEXCEPT_OPT)\
	template<typename Obj, typename Ret,typename ...Args, Ret(CALL_OPT Obj::*ptr)(Args...) CV_OPT REF_OPT NOEXCEPT_OPT>\
	struct reflex_info<Ret(CALL_OPT Obj::*)(Args...) CV_OPT REF_OPT NOEXCEPT_OPT, ptr> \
		:std::integral_constant<Ret(CALL_OPT Obj::*)(Args...) CV_OPT REF_OPT NOEXCEPT_OPT, ptr>\
	{\
		using object_type = Obj;\
		using result_type = Ret;\
		using param_type = std::tuple<Args...>;\
	};

_MEMBER_CALL_CV_REF_NOEXCEPT(_REFLEX_INFO)
#undef _REFLEX_INFO
#define _REFLEX_INFO(CV_REF_NOEXCEPT_OPT)\
	template<typename Obj, typename Ret,typename ...Args, Ret(Obj::*ptr)(Args...,...) CV_REF_NOEXCEPT_OPT>\
	struct reflex_info<Ret(Obj::*)(Args...,...) CV_REF_NOEXCEPT_OPT, ptr> \
		:std::integral_constant<Ret(Obj::*)(Args...,...) CV_REF_NOEXCEPT_OPT, ptr>\
	{\
		using object_type = Obj;\
		using result_type = Ret;\
		using param_type = std::tuple<Args...>;\
	};
_CLASS_DEFINE_CV_REF_NOEXCEPT(_REFLEX_INFO)
#undef _REFLEX_INFO
}
watery::null_reflect ____watery_reflect_invoker(...);

#define IGUANA_MAKE_META_DATA_IMPL(TEMPLATE_MARCO0,TEMPLATE_MARCO1,\
									N,						\
									STRUCT_NAME,			\
									STRUCT_TYPE,			\
									ARR_NAME, ...)			\
namespace reflect_details{									\
template<typename... T>										\
struct STRUCT_NAME##_iguana_reflect_members;					\
TEMPLATE_MARCO0												\
struct STRUCT_NAME##_iguana_reflect_members<STRUCT_TYPE>		\
							: std::true_type{				\
using struct_type = STRUCT_TYPE;							\
using reflex_type = std::tuple<__VA_ARGS__>;				\
static const size_t all_member_size = N;					\
constexpr static const char* name() { return ADD_VIEW(STRUCT_NAME); }	\
constexpr static std::array<const char*, N> arr()						\
{ return { ARR_NAME }; }									\
};															\
}															\
TEMPLATE_MARCO1												\
reflect_details::STRUCT_NAME##_iguana_reflect_members<STRUCT_TYPE> ____watery_reflect_invoker(STRUCT_TYPE);

#endif // REFLECTION_STRUCT_H

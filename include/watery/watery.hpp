#pragma once
#ifndef WATERY_HPP
#define WATERY_HPP
#include <type_traits>
#include <tuple>
#include <watery\reflection.hpp>

template<typename T> struct iguana_reflect_members;
namespace watery
{
template<typename T>
using is_member_object_pointer = is_reflex_member_object<T>;
template<typename T>
using is_member_function_pointer = is_reflex_member_function<T>;

template<typename T>
constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
template<typename T>
constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

namespace details
{
template<class T, size_t S2> struct push_index_sequence;
template<size_t... I, size_t J>
struct push_index_sequence<std::index_sequence<I...>, J>
{
	using type = std::index_sequence<I..., J>;
};

template<typename Tuple, template <typename...> typename Fn, typename List, size_t index>
struct get_member_type_index_impl
{
	using element_t = std::tuple_element_t<index, Tuple>;
	using type = std::conditional_t < Fn<element_t>::value, typename push_index_sequence<List, index>::type, List >;
};
template<typename Tuple, typename List, template <typename...> typename Fn, size_t... indexs>
struct get_member_type_index;
template<typename Tuple, typename List, template <typename...> typename Fn, size_t index>
struct get_member_type_index<Tuple, List, Fn, index>
{
	using type = typename get_member_type_index_impl<Tuple, Fn, List, index>::type;
};
template<typename Tuple, typename List, template <typename...> typename Fn, size_t index, size_t... indexs>
struct get_member_type_index<Tuple, List, Fn, index, indexs...>
{
private:
	using new_list = typename get_member_type_index_impl<Tuple, Fn, List, index>::type;
public:
	using type = typename get_member_type_index<Tuple, new_list, Fn, indexs...>::type;
};

template<typename Tuple, typename List, template <typename...> typename Fn, typename T>
struct calc_member_x_index_impl;
template<typename Tuple, typename List, template <typename...> typename Fn, size_t ... indexs>
struct calc_member_x_index_impl <Tuple, List, Fn, std::index_sequence<indexs...>>
{
	using type = typename get_member_type_index<Tuple, List, Fn, indexs...>::type;
};
}
template<typename Tuple, template <typename...> typename Fn>
struct calc_member_x_index
{
	using type = typename details::calc_member_x_index_impl<Tuple, std::index_sequence<>, Fn, std::make_index_sequence<std::tuple_size_v<Tuple>>>::type;
};

template<typename MemberTuple>
struct member_field_index {
	using type = typename calc_member_x_index<MemberTuple, is_member_object_pointer>::type;
};
template<typename MemberTuple>
struct member_function_index {
	using type = typename calc_member_x_index<MemberTuple, is_member_function_pointer>::type;
};

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
template <typename Tuple, typename T, typename F, size_t... Idx>
constexpr void for_each_impl(T&& t, F&& f, std::index_sequence<Idx...>)
{
	(f((t.*std::tuple_element_t<Idx, Tuple>::value), std::integral_constant<size_t, Idx>{}), ...);
}
template <typename Tuple, typename F, size_t... Idx>
constexpr void for_each_ptr_impl(F&& f, std::index_sequence<Idx...>)
{
	(f(std::tuple_element_t<Idx, Tuple>::value, std::integral_constant<size_t, Idx>{}), ...);
}
template <typename Tuple, typename F, size_t... Idx>
constexpr void for_each_overload_tuple_impl(F&& f, std::index_sequence<Idx...>)
{
	(for_each_ptr_impl<typename std::tuple_element_t<Idx, Tuple>::overload_type>(f, std::make_index_sequence<std::tuple_size_v<typename std::tuple_element_t<Idx, Tuple>::overload_type>>{}), ...);
}
template <typename Tuple, typename F, size_t... Idx>
constexpr void for_each_meta_impl(F&& f, std::index_sequence<Idx...>)
{
	(std::forward<F>(f)(std::tuple_element_t<Idx, Tuple>{}), ...);
}

template <typename Tuple, typename F, size_t... Idx>
constexpr void for_each_meta_index_impl(F&& f, std::index_sequence<Idx...>)
{
	(std::forward<F>(f)(std::tuple_element_t<Idx, Tuple>{},std::integral_constant<size_t,Idx>{}), ...);
}
template <typename Tuple, typename F, size_t... Idx>
constexpr void for_each_overload_tuple_meta_impl(F&& f, std::index_sequence<Idx...>)
{
	(for_each_meta_impl<typename std::tuple_element_t<Idx, Tuple>::overload_type>(f, std::make_index_sequence<std::tuple_size_v<typename std::tuple_element_t<Idx, Tuple>::overload_type>>{}), ...);
}

}
template<typename T>
struct iguana_reflect_info
{
	using type = decltype(____watery_reflect_invoker(std::declval<T>()));
};
template<typename T>
using reflex_type_t = typename iguana_reflect_info<std::decay_t<T>>::type;

template<typename T>
struct is_reflection : decltype(____watery_reflect_invoker(std::declval<T>()))
{
};
template<typename T>
constexpr bool is_reflection_v = is_reflection<T>::value;

template<typename T, typename F>
constexpr void for_each_function(F&& f);
template<typename T, typename F>
constexpr void for_each_field(T&& t, F&& f);
template<typename T, typename F>
constexpr void for_each_field_ptr(F&& f);
template<typename T, typename F>
constexpr void for_each_all(F&& f);
template<typename T, typename F>
constexpr void for_each_function_meta(F&& f);
template<typename T, typename F>
constexpr void for_each_field_meta(F&& f);
template<typename T, typename F>
constexpr void for_each_all_meta(F&& f);

namespace details
{
	template<typename T, size_t Index>
	constexpr std::enable_if_t<is_reflection_v<T>, WATERY_STRING> get_reflex_name()
	{
		static_assert(is_reflection<T>::value, "");
		using M = reflex_type_t<T>;
		return M::arr()[Index];
	}
	template<typename T, size_t Index>
	constexpr std::enable_if_t<!is_reflection_v<T>, WATERY_STRING> get_reflex_name()
	{
		static_assert(is_reflection<typename T::object_type>::value, "");
		using M = reflex_type_t<typename T::object_type>;
		return M::arr()[Index];
	};
}

template<typename T>
constexpr size_t get_reflex_index()
{
	using object = typename T::object_type;
	using reflex_type = typename reflex_type_t<object>::reflex_type;
	return details::for_each_finder_tuple_meta<reflex_type>([](auto meta, auto index) constexpr {
		return std::is_same_v<decltype(meta), T>;
	});
}
template<typename T>
constexpr size_t get_reflex_index(T finder)
{
	static_assert(std::is_enum_v<T>);
	using reflex_type = typename reflex_type_t<T>::reflex_type;
	return details::for_each_finder_tuple_meta<reflex_type>([finder](auto meta, auto index) constexpr {
		return decltype(meta)::value == finder;
	});
}
template<typename T>
constexpr size_t get_reflex_overload_function_index()
{
	using object = typename T::object_type;
	using reflex_type = typename reflex_type_t<object>::reflex_type;
	return details::for_each_finder_tuple_meta<reflex_type>([](auto meta, auto index) constexpr {
		if constexpr(is_reflex_overload_function<decltype(meta)>::value)
		{
			return details::for_each_finder_tuple_meta<decltype(meta)>([](auto meta, auto index) constexpr {
				return std::is_same_v<decltype(meta), T>;
			}) != -1;
		}
		return false;
	});
}
template<typename T, size_t index>
constexpr WATERY_STRING get_name()
{
	return details::get_reflex_name<T, index>();
}
template<typename T, typename Index>
constexpr WATERY_STRING get_name(const T&,const Index&)
{
	return details::get_reflex_name<T, Index::value>();
}
template<typename T, T ptr>
constexpr WATERY_STRING get_name(const std::integral_constant<T,ptr>&)
{
	constexpr size_t Index = get_reflex_index<reflex_info<T, ptr>>();
	static_assert(Index != -1);
	return details::get_reflex_name<reflex_info<T, ptr>, Index>();
}
template<typename Obj, typename ...Args>
constexpr WATERY_STRING get_name(const reflex_overload_function<Obj, Args...>&)
{
	using overload_t = typename reflex_overload_function<Obj, Args...>::overload_type;
	constexpr size_t Index = get_reflex_index<std::tuple_element_t<0, overload_t>>();
	return details::get_reflex_name<Obj, Index>();
}
template<typename T>
constexpr auto get_name(T value)->std::enable_if_t<std::is_enum_v<T>, WATERY_STRING>
{
	static_assert(is_reflection_v<T>);
	using type = reflex_type_t<T>;
	auto index = get_reflex_index(value);
	if (index == -1)
	{
		return {};
	}
	return type::arr()[index];
}
template<typename T>
constexpr WATERY_STRING get_name()
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	return M::name();
}

template<typename T>
constexpr std::enable_if_t<is_reflection_v<T>,bool> has_base_type()
{
	return !std::is_void_v<typename T::base_type>;
}

template<typename T>
constexpr std::enable_if_t<!is_reflection_v<T>, bool> has_base_type()
{
	static_assert(is_reflection<typename T::object_type>::value);
	//If build error ,because T is not reflection!
	using reflex_t = reflex_type_t<typename T::object_type>;
	return !std::is_void_v<typename reflex_t::base_type>;
}
template<typename T,typename =void>
struct reflex_base_type
{
	static_assert(is_reflection<typename T::object_type>::value);
	using type = typename reflex_type_t<typename T::object_type>::base_type;
};
template<typename T>
struct reflex_base_type<T, std::void_t<typename T::base_type>>
{
	static_assert(is_reflection_v<typename T::base_type>);
	using type = typename T::base_type;
};

template<typename Obj,typename Ret,typename ...Args, typename T, typename... Args1>
constexpr Ret invoke(Ret(Obj::*ptr)(Args...), T&& t, Args1&&... args)
{
	return (t.*ptr)(std::forward<Args1>(args)...);
}

template<typename Fn, typename T, typename ...Args>
constexpr auto invoke(const Fn&, T&& t, Args&&... args)
	->std::enable_if_t<is_reflex_member_function<Fn>::value, decltype(invoke(Fn::value, std::forward<T>(t), std::forward<Args>(args)...))>
{
	return invoke(Fn::value, std::forward<T>(t), std::forward<Args>(args)...);
}

template<typename Tuple, typename ...Args>
struct is_overload_match : std::false_type {};
template<typename ...Args, typename ...Args1>
struct is_overload_match<std::tuple<Args...>, Args1...> : std::is_invocable<void(Args...), Args1...> {};
template<typename Overload, typename Obj, typename ...Args>
constexpr auto invoke(Obj&& obj, Args&&... args)
{
	using overload_t = typename Overload::overload_type;
	using object_t = typename Overload::object_type;
	static_assert(std::is_same_v<std::decay_t<Obj>, object_t>);
	constexpr auto index = details::for_each_finder_tuple_meta<overload_t>(
		[](auto fn, auto index) constexpr {
		return is_overload_match<decltype(fn)::param_type, Args...>::value;
	});
	static_assert(index != -1, "args is not match");
	using type = std::tuple_element_t<index, overload_t>;
	return std::invoke(type::value, std::forward<Obj>(obj), std::forward<Args>(args)...);
}

template<typename T>
constexpr bool is_overload_function(const T& fn)
{
	static_assert(is_reflex_member_function<T>::value);
	using reflex_t = reflex_type_t<typename T::object_type>;
	using reflex_type = typename reflex_t::reflex_type;
	constexpr auto name = get_name(fn);
	constexpr auto arr = reflex_t::arr();
	size_t count = 0;
	details::for_each_meta_index_impl<reflex_type>([&](auto meta, auto index) constexpr
	{
		if constexpr(arr[index()] == name)
		{
			count++;
		}
	}, std::make_index_sequence<arr.size()>{});
	return count > 1;
}

template<typename T, typename F>
constexpr void for_each_function_overload(const T& fn, F&& f)
{
	static_assert(is_reflex_member_function<T>::value);
	static_assert(is_overload_function(std::declval<T>()));
	using reflex_t = reflex_type_t<typename T::object_type>;
	using reflex_type = typename reflex_t::reflex_type;
	constexpr auto name = get_name(fn);
	constexpr auto arr = reflex_t::arr();
	details::for_each_meta_index_impl<reflex_type>([&](auto meta, auto index) constexpr
	{
		if constexpr(arr[index()] == name)
		{
			f(meta, index);
		}
	}, std::make_index_sequence<arr.size()>{});
}

template<typename T, typename F>
constexpr void for_each_function(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using reflex_t = reflex_type_t<T>;
	using reflex_type = typename reflex_t::reflex_type;
	using functions_index_sequence = typename member_function_index<reflex_type>::type;
	details::for_each_ptr_impl<reflex_type>(std::forward<F>(f), functions_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_field(T&& t, F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using reflex_t = reflex_type_t<T>;
	using reflex_type = typename reflex_t::reflex_type;
	using member_index_sequence = typename member_field_index<reflex_type>::type;
	details::for_each_impl<reflex_type>(std::forward<T>(t), std::forward<F>(f), member_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_field_ptr(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using reflex_t = reflex_type_t<T>;
	using reflex_type = typename reflex_t::reflex_type;
	using member_index_sequence = typename member_field_index<reflex_type>::type;
	details::for_each_ptr_impl<reflex_type>(std::forward<F>(f), member_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_all(F&& f)
{
	static_assert(is_reflection<T>::value);
	using reflex_t = reflex_type_t<T>;
	using reflex_type = typename reflex_t::reflex_type;
	using index_sequence = std::make_index_sequence<reflex_t::all_member_size>;
	details::for_each_ptr_impl<reflex_type>(std::forward<F>(f), index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_function_meta(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using reflex_t = reflex_type_t<T>;
	using reflex_type = typename reflex_t::reflex_type;
	using functions_index_sequence = typename member_function_index<reflex_type>::type;
	details::for_each_meta_impl<reflex_type>(std::forward<F>(f), functions_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_field_meta(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using reflex_t = reflex_type_t<T>;
	using reflex_type = typename reflex_t::reflex_type;
	using M_index_sequence = typename member_field_index<reflex_type>::type;
	details::for_each_meta_impl<reflex_type>(std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_all_meta(F&& f)
{
	static_assert(is_reflection<T>::value);
	using reflex_t = reflex_type_t<T>;
	using reflex_type = typename reflex_t::reflex_type;
	using index_sequence = std::make_index_sequence<reflex_t::all_member_size>;
	details::for_each_meta_impl<reflex_type>(std::forward<F>(f), index_sequence{});
}
}

#endif // WATERY_HPP

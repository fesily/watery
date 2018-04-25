#pragma once
#ifndef WATERY_HPP
#define WATERY_HPP
#include <type_traits>
#include <tuple>
#include <watery\reflection.h>

template<typename T> struct iguana_reflect_members;
namespace watery
{
template<typename T>
struct is_member_object_pointer;
template<typename T, T ptr>
struct is_member_object_pointer<reflex_info<T, ptr>>
{
	static constexpr auto value = std::is_member_object_pointer_v<T>;
};
template<typename T>
struct is_member_function_pointer;
template<typename T, T ptr>
struct is_member_function_pointer<reflex_info<T, ptr>>
{
	static constexpr auto value = std::is_member_function_pointer_v<T>;
};
template<typename T>
constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
template<typename T>
constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

namespace details
{
template<class T, size_t S2> struct push_integer_sequence;
template<class T, T... I, size_t J> struct push_integer_sequence<std::integer_sequence<T, I...>, J>
{
	using type = std::integer_sequence<T, I..., J>;
};
template<typename Tuple, template <typename...> typename Fn, typename List, size_t index>
struct get_member_type_index_impl
{
	using element_t = std::tuple_element_t<index, Tuple>;
	using type = std::conditional_t < Fn<element_t>::value, typename push_integer_sequence<List, index>::type, List >;
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
template <typename Tuple, typename T, typename F, size_t... Idx>
constexpr void for_each_impl(T&& t, F&& f, std::index_sequence<Idx...>)
{
	using expander = int[];
	(void)expander {
		0, (void(
			std::forward<F>(f)((t.*std::tuple_element_t<Idx, Tuple>::value), std::integral_constant<size_t, Idx>{}))
			, 0)...
	};
}
template <typename Tuple, typename F, size_t... Idx>
constexpr void for_each_ptr_impl(F&& f, std::index_sequence<Idx...>)
{
	using expander = int[];
	(void)expander {
		0, (void(
			std::forward<F>(f)(std::tuple_element_t<Idx, Tuple>::value, std::integral_constant<size_t, Idx>{}))
			, 0)...
	};
}
template <typename Tuple, typename F, size_t... Idx>
constexpr void for_each_meta_impl(F&& f, std::index_sequence<Idx...>)
{
	using expander = int[];
	(void)expander {
		0, (void(
			std::forward<F>(f)(std::tuple_element_t<Idx, Tuple>{}))
			, 0)...
	};
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
	struct get_reflex_name
	{
		static_assert(is_reflection<T>::value, "");
		constexpr static auto name()
		{
			using M = reflex_type_t<T>;
			return M::arr()[Index];
		}
	};
	template<typename T, T ptr, size_t Index>
	struct get_reflex_name<reflex_info<T, ptr>, Index>
	{
		static_assert(is_reflection<typename reflex_info<T, ptr>::object_type>::value, "");
		constexpr static auto name()
		{
			using M = reflex_type_t<typename reflex_info<T, ptr>::object_type>;
			return M::arr()[Index];
		}
	};

	template<typename T>
	struct get_reflex_type
	{
		using type = reflex_type_t<T>;
	};
	template<typename T,T p>
	struct get_reflex_type<reflex_info<T,p>>
	{
		using type = reflex_type_t<typename reflex_info<T, p>::object_type>;
	};

	template<typename T>
	struct is_reflex_info :std::false_type{};
	template<typename T,T p>
	struct is_reflex_info<reflex_info<T,p>> :std::true_type {};
}

template<typename T>
constexpr size_t get_reflex_index()
{
	using object = typename T::object_type;
	using reflex_type = typename reflex_type_t<object>::reflex_type;
	using sequence = std::make_index_sequence<std::tuple_size_v<reflex_type>>;
	return details::for_each_finder_tuple_meta<reflex_type>([](auto meta, auto index) constexpr {
		return std::is_same_v<decltype(meta), T>;
	});
}
template<typename T>
constexpr size_t get_reflex_index(T finder)
{
	static_assert(std::is_enum_v<T>);
	using reflex_type = typename reflex_type_t<T>::reflex_type;
	using sequence = std::make_index_sequence<std::tuple_size_v<reflex_type>>;
	return details::for_each_finder_tuple_meta<reflex_type>([finder](auto meta, auto index) constexpr {
		return decltype(meta)::value == finder;
	});
}
template<typename T, size_t index>
constexpr const char* get_name()
{
	return details::get_reflex_name<T, index>::name();
}
template<typename T, typename Index>
constexpr const char* get_name(const T&,const Index&)
{
	return details::get_reflex_name<T, Index::value>::name();
}
template<typename T, T ptr>
constexpr const char* get_name(const reflex_info<T,ptr>&)
{
	constexpr size_t Index = get_reflex_index<reflex_info<T, ptr>>();
	return details::get_reflex_name<reflex_info<T, ptr>, Index>::name();
}
template<typename T>
constexpr auto get_name(T value)->std::enable_if_t<std::is_enum_v<T>, const char*>
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
constexpr const char* get_name()
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	return M::name();
}
template<typename Obj,typename Ret,typename ...Args, typename T, typename... Args1>
constexpr Ret invoke(Ret(Obj::*ptr)(Args...), T&& t, Args1&&... args)
{
	return (t.*ptr)(std::forward<Args1>(args)...);
}

template<typename Fn, typename T, typename ...Args>
constexpr auto invoke(const Fn&, T&& t, Args&&... args)
	->std::enable_if_t<details::is_reflex_info<Fn>::value, decltype(invoke(Fn::value, std::forward<T>(t), std::forward<Args>(args)...))>
{
	return invoke(Fn::value, std::forward<T>(t), std::forward<Args>(args)...);
}

template<typename T, typename F>
constexpr void for_each_function(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	using reflex_type = typename M::reflex_type;
	using M_index_sequence = typename member_function_index<reflex_type>::type;
	details::for_each_ptr_impl<reflex_type>(std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_field(T&& t, F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	using reflex_type = typename M::reflex_type;
	using M_index_sequence = typename member_field_index<reflex_type>::type;
	details::for_each_impl<reflex_type>(std::forward<T>(t), std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_field_ptr(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	using reflex_type = typename M::reflex_type;
	using M_index_sequence = typename member_field_index<reflex_type>::type;
	details::for_each_ptr_impl<reflex_type>(std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_all(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	using M_index_sequence = std::make_index_sequence<M::all_member_size>;
	details::for_each_ptr_impl<reflex_type>(std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_function_meta(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	using reflex_type = typename M::reflex_type;
	using M_index_sequence = typename member_function_index<reflex_type>::type;
	details::for_each_meta_impl<reflex_type>(std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_field_meta(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	using reflex_type = typename M::reflex_type;
	using M_index_sequence = typename member_field_index<reflex_type>::type;
	details::for_each_meta_impl<reflex_type>(std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
constexpr void for_each_all_meta(F&& f)
{
	static_assert(is_reflection<T>::value, "");
	using M = reflex_type_t<T>;
	using reflex_type = typename M::reflex_type;
	using M_index_sequence = std::make_index_sequence<M::all_member_size>;
	details::for_each_meta_impl<reflex_type>(std::forward<F>(f), M_index_sequence{});
}
}

#endif // WATERY_HPP

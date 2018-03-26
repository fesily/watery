#pragma once
#ifndef WATERY_HPP
#define WATERY_HPP
#include <type_traits>
#include <tuple>
#include <watery\reflection.h>
template<typename T> struct iguana_reflect_members;
namespace watery
{
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
	using type = typename calc_member_x_index<MemberTuple, std::is_member_object_pointer>::type;
};
template<typename MemberTuple>
struct member_function_index {
	using type = typename calc_member_x_index<MemberTuple, std::is_member_function_pointer>::type;
};
namespace details
{
template <typename T, typename... Args, typename F, size_t... Idx>
void for_each_impl(T&& t, const std::tuple<Args...>& ptrs, F&& f, std::index_sequence<Idx...>)
{
	using expander = int[];
	(void)expander {
		0, (void(
			std::forward<F>(f)(t.*std::get<Idx>(ptrs), std::integral_constant<size_t, Idx>{}))
			, 0)...
	};
}
template <typename... Args, typename F, size_t... Idx>
void for_each_ptr_impl(const std::tuple<Args...>& ptrs, F&& f, std::index_sequence<Idx...>)
{
	using expander = int[];
	(void)expander {
		0, (void(
			std::forward<F>(f)(std::get<Idx>(ptrs), std::integral_constant<size_t, Idx>{}))
			, 0)...
	};
}

}
template<typename T>
struct iguana_reflect_info
{
	using type = iguana_reflect_members<T>;
};
template<typename T>
using iguana_reflect_info_t = typename iguana_reflect_info<typename std::decay<T>::type>::type;
template<typename T, typename F>
void for_each_function(F&& f)
{
	using M = iguana_reflect_info_t<T>;
	using M_index_sequence = typename member_function_index<decltype(M::apply_impl())>::type;
	details::for_each_ptr_impl(M::apply_impl(), std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
void for_each_field(T&& t, F&& f)
{
	using M = iguana_reflect_info_t<T>;
	using M_index_sequence = typename member_field_index<decltype(M::apply_impl())>::type;
	details::for_each_impl(std::forward<T>(t), M::apply_impl(), std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
void for_each_field_ptr(F&& f)
{
	using M = iguana_reflect_info_t<T>;
	using M_index_sequence = typename member_field_index<decltype(M::apply_impl())>::type;
	details::for_each_ptr_impl(M::apply_impl(), std::forward<F>(f), M_index_sequence{});
}

template<typename T, typename F>
void for_each_all(F&& f)
{
	using M = iguana_reflect_info_t<T>;
	using M_index_sequence = std::make_index_sequence<M::all_member_size>;
	details::for_each_ptr_impl(M::apply_impl(), std::forward<F>(f), M_index_sequence{});
}
}

#endif // WATERY_HPP

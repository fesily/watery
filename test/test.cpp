#include <watery/watery.hpp>
#include <cassert>
#include <type_traits>
#include "watery/reflection_macro.h"
#include <tuple>


template<typename T>
struct is__ : std::is_same<T, int>
{

};
struct TEST
{
	int a;
	int b()
	{
		return 0;
	}
	int c;
	int d;
	void e()
	{
		
	}
	void f()
	{
		
	}
	int g;
};
WATERY_REFLECTION(TEST, a, b, c, d, e, f, g)
template<typename T>
struct TEST1
{
	T a;
	int b()
	{
		return 0;
	}
	T c;
	T d;
	void e()
	{

	}
	void f()
	{

	}
	T g;
};
WATERY_REFLECTION_TEMPLATE(TEST1,1, a, b, c, d, e, f, g)
WATERY_REFLECTION_TEMPLATE_FULL(TEST1,MACRO_FORWARD(int), a, b, c, d, e, f, g)
template<typename T,typename T2>
struct TEST2
{
	T a;
	int b()
	{
		return 0;
	}
	T c;
	T2 d;
	void e()
	{

	}
	void f()
	{

	}
	T2 g;
};

WATERY_REFLECTION_TEMPLATE(TEST2,2,a)
WATERY_REFLECTION_TEMPLATE_PARTIAL(TEST2, MACRO_FORWARD(int, T1),1, a, b, c, d, e, f, g)
WATERY_REFLECTION_TEMPLATE_FULL(TEST2, MACRO_FORWARD(int, double), a, b, c, d, e, f, g)
template<typename TEST>
void DoWork(TEST t)
{
	watery::for_each_function<TEST>([t](auto fn, auto index) mutable
	{
		(t.*fn)();
	});
	watery::for_each_field_ptr<TEST>([&t](auto ptr, auto index) mutable
	{
		t.*ptr = 1;
	});
	assert(t.a == 1);
	assert(t.c == 1);
	assert(t.d == 1);
	assert(t.g == 1);
	watery::for_each_field(t,[](auto &value, auto index)
	{
		value = 2;
	});
	assert(t.a == 2);
	assert(t.c == 2);
	assert(t.d == 2);
	assert(t.g == 2);
}

int main()
{
    using namespace watery;
    {
		using namespace watery::details;

		using t1 = calc_member_x_index<std::tuple<int, double, double, int, double, int>, is__>::type;
		static_assert(std::is_same_v<t1, std::index_sequence<0, 3, 5>>, "");
		using t2 = calc_member_x_index<std::tuple<int>, is__>::type;
		static_assert(std::is_same_v<t2, std::index_sequence<0>>, "");
		using t3 = calc_member_x_index<std::tuple<int, int>, is__>::type;
		static_assert(std::is_same_v<t3, std::index_sequence<0, 1>>, "");
		auto tuple = std::make_tuple(&TEST::a, &TEST::b, &TEST::c, &TEST::d, &TEST::f, &TEST::g);

		using t4 = typename calc_member_x_index<decltype(tuple), std::is_member_function_pointer>::type;
		static_assert(std::is_same_v<t4, std::index_sequence<1, 4>>, "");

		using t5 = typename calc_member_x_index<decltype(tuple), std::is_member_object_pointer>::type;
		static_assert(std::is_same_v<t5, std::index_sequence<0, 2, 3, 5>>, "");
    }
	{
    	TEST t ={};		
		DoWork(t);
	} 
	{
		TEST1<double> t = {};
		DoWork(t);
	}
    {
		TEST1<int> t = {};
		DoWork(t);
    }
    {
	    struct MMM
	    {
		    
	    };
		static_assert(!is_reflection<MMM>::value, "");
    }
    {
		// macro impl test
		TEST2<int, float> t1;
		TEST2<int, int> t2;
		TEST2<float, int> t3;
		static_assert(std::is_same<decltype( ____watery_reflect_invoker(t1)),reflect_details::TEST2iguana_reflect_members<TEST2<int,float>>>::value,"");
		static_assert(std::is_same<decltype(____watery_reflect_invoker(t2)), reflect_details::TEST2iguana_reflect_members<TEST2<int, int>>>::value, "");
		static_assert(std::is_same<decltype(____watery_reflect_invoker(t3)), reflect_details::TEST2iguana_reflect_members<TEST2<float, int>>>::value, "");
    }
    return 0;
}

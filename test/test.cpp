#include <watery/watery.hpp>
#include <watery/reflection_template.hpp>
#include <cassert>
#include <type_traits>
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
	int& dd(int, std::string& , const std::string&)
	{
		static int ret = 0;
		return ret;
	}
	int g;
};
//WATERY_SIMAPLE_REFLECTION(TEST,a, b, c, d, e, f, g, dd) is same as WATERY_REFLECTION(TEST, (a, b, c, d, e, f, g, dd))
WATERY_REFLECTION_SIMAPLE(TEST, a, b, c, d, e, f, g, dd)

reflect_details::TEST_iguana_reflect_members ____watery_reflect_invoker(TEST);
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
WATERY_REFLECTION_TEMPLATE(TEST1, 1, (a, b, c, d, e, f, g))
WATERY_REFLECTION_TEMPLATE_PARTIAL(TEST1,(int), (a, b, c, d, e, f, g))

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
WATERY_REFLECTION_TEMPLATE(TEST2,2,(a))
WATERY_REFLECTION_TEMPLATE_PARTIAL(TEST2, (int, WATERY_TYPENAME),( a, b, c, d, e, f, g))
WATERY_REFLECTION_TEMPLATE_PARTIAL(TEST2, (int, double), (a, b, c, d, e, f, g))
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
enum class ReflxType
{
	a,
	b,
};
WATERY_REFLECTION_ENUM(ReflxType, a, b)
template<typename Obj, typename Ret, typename ...Args>
constexpr Ret(Obj::*get_overload(Ret(Obj::*ptr)(Args...)))(Args...)
{
	return ptr;
	using Type = Ret(Obj::*)(Args...);
}

struct TestOverLoad{
	void T1(){}
	void T1(int){}
	void* T1(int, double) { return nullptr; }
	void T2() {}
	void T2(int) {}
	void* T2(int, double) { return nullptr; }
	void T3() {};
	void T4() {};
	int a,b,c;
};
WATERY_REFLECTION(TestOverLoad, (T1, void(), void(int), void*(int, double)), (T2, void(), void(int), void*(int, double)), (T3,T4,a,b,c))

int main()
{
	using T = watery::reflex_member_function<void(TestOverLoad::*)(), &TestOverLoad::T1>;
	using namespace watery;
	//ENUM object
	{
		constexpr auto index = get_reflex_index(ReflxType::a);
		static_assert(index == 0);
		constexpr auto name = get_name(ReflxType::a);
		static_assert(name == "a"); 

		constexpr auto error_type = ReflxType(99999);
		constexpr auto index1 = get_reflex_index(error_type);
		static_assert(index1 == -1);
		constexpr auto name1 = get_name(error_type);
		static_assert(name1.empty());
	}
	{
		watery::for_each_all<TestOverLoad>([](auto meta, auto index)
		{
			printf("%zd\n", index());
		});
		watery::for_each_all_meta<TestOverLoad>([](auto meta)
		{
			auto name = watery::get_name(meta);
			printf("%s\n", name.data());
		});
	}
	{
		using T1 = std::tuple_element_t<0, watery::reflex_type_t<TEST>::reflex_type>;
		constexpr auto index = get_reflex_index<T1>();
		static_assert(index == 0);
	}

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
		//DoWork(t);
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
		TEST t;
		watery::for_each_function_meta<TEST>([=](auto fn) mutable {
			constexpr auto name = watery::get_name(decltype(fn){});
			if constexpr (name != "dd")
			{
				watery::invoke(fn, t);
			}
			else
			{
				int a = 99;
				std::string s = "1234";
				auto& r = watery::invoke(fn, t, a, s, "455");
			}
		});
		int a = 99;
		std::string s = "1234";
		auto& r = watery::invoke(&TEST::dd, t, a, s, "455");
		r += 1;
		auto& r1 = watery::invoke(&TEST::dd, t, a, s, "455");
		assert(r1 == 1);
    }
    {
		// macro impl test
		TEST2<int, float> t1;
		TEST2<int, int> t2;
		TEST2<float, int> t3;
		static_assert(std::is_same<decltype( ____watery_reflect_invoker(t1)),reflect_details::TEST2_iguana_reflect_members<TEST2<int,float>>>::value,"");
		static_assert(std::is_same<decltype(____watery_reflect_invoker(t2)), reflect_details::TEST2_iguana_reflect_members<TEST2<int, int>>>::value, "");
		static_assert(std::is_same<decltype(____watery_reflect_invoker(t3)), reflect_details::TEST2_iguana_reflect_members<TEST2<float, int>>>::value, "");
    }
    return 0;
}

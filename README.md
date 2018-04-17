# watery
	c++ static reflection header only library.
	now support enum object, class object and template object.
	try implemented c++ iso standardization Proposed static reflection:p0194r3.
	
##Note:

	Only test on msvc2017.15.6!
	I think watery can work well on any compiler which supported c++17.
	
##Install:

	Just only use <include> dir.
	
##Example:

	###Enum
		enum class Type{
			a=-1,
			b=999,
		}
		WATERY_ENUM_REFLECTION(Type,a,b)
		static_assert(watery::get_name(Type::a) == "a");
		static_assert(watery::get_name(Type::b) == "b");
		// this can support serializer/deserializer from int/string to enum
		
	###Ojbect
		class Test{
			int a;
			int b;
			int c(){}
		}
		WATERY_REFLECTION(Test,a,b,c)
		watery::for_each_all_meta<Test>([](auto meta){
			constexpr auto name = watery::get_name(meta);
			if constexpr(watery::is_member_object_v(meta))
			{
			
			}
			else if constexpr(watery::is_member_function_v(meta))
			{
				Test t;
				watery::invoke(meta,t);// call Test.c()
			}
		});
		
##Future:

	1.fork support c++11,c++14.
	2.fork support msvc 2013,2015.
	3.add msvc/clang extended plugin to support this lib, automatically generated reflection code.
	4.improve reflection template object.

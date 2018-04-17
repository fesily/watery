# watery
	c++ �����ڷ����,ֻ��Ҫͷ�ļ�.
	����֧��ö��,��ͨ������,�Լ�ģ������

##Note:

	ֻ��MSVC2017�±����,��Ϊ������������ûװ.֧��c++17�ı�����Ӧ�ö��ǿ��Ե�.

##Install:

	����Include�ļ��м���
	
##Example:

	###Enum
		```cpp
		enum class Type{
			a=-1,
			b=999,
		}
		WATERY_ENUM_REFLECTION(Type,a,b)
		static_assert(watery::get_name(Type::a) == "a");
		static_assert(watery::get_name(Type::b) == "b");
		//����������ܵ��ô��������л�/�����л�,�����������ַ���ת����ö��,˳�������.
		```
	###Ojbect
		```cpp
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
		```
	###Template
		//ûд,���Test.cpp
		
##Future:

	1.��֧֧��c++11,c++14.��������������еĹ��ܶ�������c++11��ʵ��,���Ǵ����е��ѿ���.
	2.��֧֧��msvc 2013,2015.
	3.���VC��Clang�Ĳ��,ֱ�����ɷ���ĺ����.(����һ�������ڱ���ǰ��̬����,�ݻ�)
	4.�Ľ�ģ�巴��ĺ�д��,Ŀǰ������

# watery
c++ 编译期反射库,只需要头文件.  
现在支持枚举,普通的类型,以及模板类型  
## Note:
只在MSVC2017下编译过,因为其他编译器都没装.支持c++17的编译器应该都是可以的.  
## nstall:
包括Include文件夹即可,如果要使用 模板反射模块,则依赖BOOST.preprocessor库
## Example:
### Enum
```cpp
enum class Type{
	a=-1,
	b=999,
}
WATERY_ENUM_REFLECTION(Type,a,b)
static_assert(watery::get_name(Type::a) == "a");
static_assert(watery::get_name(Type::b) == "b");
//这个东西可能的用处就是序列化/反序列化,从整数或者字符串转换到枚举,顺带检测了.
```
### Ojbect
```cpp
class Test{
	int a;	
	int b;
	int c(){}
}
//WATERY_SIMAPLE_REFLECTION(Test,a,b,c) == WATERY_REFLECTION(Test,(a,b,c))
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
### Template
//没写,详见Test.cpp		
## Future:
1.分支支持c++11,c++14.理论上这个库所有的功能都可以在c++11上实现,就是代码有点难看吧.  
2.分支支持msvc 2013,2015.  
3.添加VC和Clang的插件,直接生反射的宏代码.(更进一步可以在编译前动态更新,暂缓)  
4.改进模板反射的宏写法,目前的难用  

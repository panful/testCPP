**Effective Modern C++读书笔记**

# 一、型别推导
## 1.理解模板型别推导
- 在模板型别推导过程中，具有引用型别的实参会被当成非引用型别来处理。换言之，其引用会被忽略。
- 对万能引用形参进行推导时，左值实参会进行特殊处理。（左值会被推导为左值引用）
- 对按值传递的形参进行推导时，若实参型别中带有const或volatile饰词，则它们还是会被当作不带const或volatile饰词的型别来处理。
- 在模板型别推导过程中，数组或函数型别的实参会退化成对应的指针，除非它们被利用来初始化引用。
## 2.理解auto型别推导

# 二、auto
## 5.优先选用auto，而非显式型别声明

## 6.当auto推导的型别不符合要求时，使用带显式型别的初始化物习惯用法
# 三、转向现代C++
## 7.在创建对象时注意区分()和{}
```C++
std::vector<int> vec1{10, 20}; // 两个元素10和20
std::vector<int> vec2(10, 20); // 10个值为20的元素

template<typename T, typename... Args>
void Func(Args&&... args)
{
    // 利用args构造T
    T t1(std::forward<Args>(args)...);
    T t2{std::forward<Args>(args)...};

    // 典型示例：
    std::make_unique
    std::make_shared
}

class T
{
public:
    // 类有std::initializer_list类型的参数作为构造函数和没有时
    // T t{};
    T(std::initializer_list<int>)
}
```
## 8.优先选用nullptr，而非0或NULL
- 避免在整形和指针型别之间重载，std::nullptr_t可以隐式转换到所有指针型别
```C++
#define NULL 0
#define NULL 0L
#define NULL ((void *)0)
```
## 9.优先选用别名声明而非typedef
- typedef不支持模板化，但是别名声明支持
- 在模板内，对于内嵌typedef的引用经常要求加上typename前缀
## 10.优先选用限定作用域的枚举型别，而非不限定作用域的枚举型别
- 不限范围的枚举型别中的枚举量可以隐式转换到整形型别，并能够从此处进一步转换到浮点型别等
- enum class总是可以前置声明（默认型别是int），enum只有指定了默认底层型别的前提下才可以前置声明
- enum可以用来当作`std::get<Enum>(tuple)`的模板参数
## 11.优先选用删除函数，而非private未定义函数
- 习惯上，删除函数一般被声明为public，编译器会提供有更好的错误信息、
- 删除函数可以让参数隐式转换失败
- 删除函数可以阻止不应该进行的模板具现
- 任何函数都可以删除，包括非成员函数和模板具现
```C++
// 隐式转换失败
void Func(int){}
void Func(float) = delete
Func(1.f); // error

// 阻止模板具现
template<typename T>
void Func(T*){}

template<>
void Func<void>(void*) = delete;
```
## 12.为意在改写的函数添加override声明
- 子类中重写的函数都应该添加override声明
- 成员函数引用饰词使得对于左值和右值对象(*this)的处理能够区分开来
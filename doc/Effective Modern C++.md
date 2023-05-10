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
## 13.优先选用const_iterator，而非iterator
- 优先选用非成员函数版本的begin()、end()和rbegin()等函数
```C++
std::begin(container);
std::end(container);

template<class C>
auto cbegin(const C& con) -> decltype(std::begin(con))
{
    // 注意此处是std::begin()
    return std::begin(con);
}
```
## 14.只要函数不会发射异常，就为其加上noexpect声明
- 异常中立函数（自身不抛异常，调用的函数会抛出异常）不具备noexcept性质，大多数函数是异常中立的
- 只要能确保函数不抛出异常，就一定要加上noexcept
- noexcept性质对于移动操作、swap、内存释放函数和析构函数最有价值
- delete、delete[]、析构函数等从C++11开始隐式的具备noexcept性质
## 15.只要有可能使用constexpr，就使用它
- 所有constexpr对象都是const对象，而并非所有的const对象都是constexpr对象
- constexpr对象都具备const属性，并由编译期已知的值完成初始化
## 16.保证const成员函数的线程安全性
- 带有const的成员函数应该线程安全
- 运用std::atomic型别的变量会比运用互斥量提供更好的性能，但前者仅适用对单个变量或内存区域的操作
## 17.理解特种成员函数的生成机制
- 特种成员函数是指那些C++会自动生成的函数：默认构造函数、析构函数、复制操作、移动操作
- 移动操作只有在用户没有显式声明复制操作、移动操作以及析构函数时才会自动生成
- 只要没有显式声明拷贝赋值和移动操作就会自动生成拷贝赋值，拷贝构造同理
- 在已经存在显式声明析构函数的条件下，生成复制操作已经成为了废弃行为
- 成员函数模板在任何情况下都不会抑制特种成员函数的生成
# 四、智能指针
## 18.使用std::unique_ptr管理具备专属所有权的资源
- std::unique_ptr是小巧的、高速的、具备只移型别的智能指针，对托管资源实施专属所有权语义
- 默认地，资源析构采用delete运算符来实现，但可以指定自定义删除器。有状态的删除器和采用函数指针实现的删除器会增加std::unique_ptr型别对象的尺寸
- 将std::unique_ptr转换为std::shared_ptr是容易实现的。
## 19.使用std::shared_ptr管理具备共享所有权的资源
- std::shared_ptr提供方便的手段，实现了任意资源在共享所有权语义下进行生命周期管理的垃圾回收
- 与std::unique_ptr相比，std::shared_ptr的尺寸通常是裸指针尺寸的两倍，它还会带来控制块的开销，并要求原子化的引用计数操作
- 默认的资源析构通过delete运算符进行，但同时也支持定制删除器。删除器的型别对std::shared_ptr的型别没有影响
- 避免使用裸指针型别的变量来创建std::shared_ptr指针
## 20.对于类似std::shared_ptr但有可能空悬的指针使用std::weak_ptr
- 使用std::weak_ptr来替代可能空悬的std::shared_ptr
- std::weak_ptr可能的用武之地包括缓存，观察者列表，以及避免std::shared_ptr指针环路
## 21.优先选用std::make_unique和std::make_shared，而非直接使用new
- 相比于直接使用new表达式，make系列函数消除了重复代码、改进了异常安全性，并且对于std::make_shared和std::make_allocated_shared而言，生成的目标代码会尺寸更小，速度更快
- 不适于使用make系列函数的场景包括需要定制删除器，以及期望直接传递大括号初始化物
- 对于std::shared_ptr，不建议使用make系列函数的额外场景包括：
    1. 自定义内存管理的类
    2. 内存紧张的系统、非常大的对象、以及存在比指向到相同对象的std::shared_ptr生存期更久的std::weak_ptr
## 22.使用Pimpl惯用法时，将特殊成员函数的定义放到实现文件中
- Pimpl惯用法通过降低类的客户和类的实现者之间的依赖性，减少了构建遍数。
- 对于采用std::unique_ptr来实现的Pimpl指针，须在类的头文件中声明特种成员函数，但在实现文件中实现它们，即使默认函数实现有着正确行为，也必须这样做
- 上述建议仅适用于std::unique_ptr，但并不适用std::shared_ptr

Visual Studio报错：can't delete an incomplete type 解决方法：将类的构造函数和析构函数在cpp文件中实现
# 五、右值引用、移动语义和完美转发
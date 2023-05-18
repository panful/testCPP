**目录结构说明**


## 2.Modern C++
现代C++特性介绍与使用
### C++11
|特性|示例|描述|
|:-|:-|:-|
|auto|||
|decltype|01_05_TEST2||
|using                     |00_13_TEST10            |别名模板                |
|Args...                   |00_03                   |可变参数模板            |
|std::tuple                |01_07                   |                       |
|{}                        |                        |初始化列表              |
|lambda                    |02_02_01                |                       |
|union                     |01_02_TEST8             |POD                    |
|for()                      |00_17_TEST3             |基于范围的for循环       |
|constexpr                 |00_10                   |                       |
|long long                 |00_18_TEST4             |                       |
|rvale                     |01_08_01                |                       |
|std::move std::forward    |01_08_02                |移动语义 完美转发       |
|nullptr                   |00_05                   |                       |
|std::unique+shared_ptr    |00_05                   |                       |
|成员函数的引用饰词|00_14_TEST3|限制*this是左值或右值时才可以调用该函数|

### C++14
|特性|示例|描述|
|:-|:-|:-|
|返回值类型推导             |                   ||
|lambda参数可以使用auto     |                   ||
|变量模板                   |00_13_TEST2        ||
|constexpr减少限制          |00_10                |返回void也可以使用，函数体可以多条语句|
|\[\[deprecated]]标记       |00_17_TEST4        ||
|二进制字面量                |00_18_TEST5       ||
|std::shared_lock 读写锁    |00_04_03           ||
|std::integer_sequence     |01_07_TEST5         ||
|std::exchange             |02_03_TEST3         ||
|std::quoted               |02_08_TEST7         ||

### C++17
|特性|示例|描述|
|:-|:-|:-|
|构造函数模板推导              |  00_13_TEST14        ||
|结构化绑定                    | 01_03_TEST1         ||
|if\switch中初始化，执行语句   |  01_03_TEST3         ||
|内联变量                      | 00_20_TEST2         ||
|折叠表达式                    | 00_03_TEST7         ||
|变长using声明                 | 00_07_02_TEST16        ||
|constexpr lambda表达式        | 02_02_01_TEST2      ||
|namespace嵌套                 | 00_16_TEST3        ||
|__has_include预处理表达式     |  00_17_TEST5        ||
|lambda用*this捕获对象副本     |  02_02_01_TEST3     ||
|新增Attribute                 | 00_17_TEST16       ||
|std::from_chars to_chars      | 02_08_TEST8        ||
|std::variant                  | 01_02_TEST2        ||
|std::any                      | 01_02_TEST4        ||
|std::visit                    | 01_02_TEST3        ||
|std::apply                    | 01_07_TEST4        ||
|std::make_from_tuple          | 01_07_TEST6        ||
|std::string_view              | 02_08_TEST1        ||
|std::as_const                 | 00_10_TEST8        ||
|std::file_system              | 01_01_TEST8        ||
|std::shared_mutex             | 00_04_03     ||

### C++20

[一篇文章掌握C++17核心特性](https://oreki.blog.csdn.net/article/details/124345662)

[C++11、C++14、C++17、C++20新特性总结（5万字详解）](https://blog.csdn.net/qq_41854911/article/details/119657617)
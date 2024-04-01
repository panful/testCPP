/*
 * 1. std::string_view
 * 2. 自己实现一个 std::string_view
 * 3. std::string中的字符替换
 * 4. 字面值 原生字符串 R"()" ""s ""sv
 * 5. std::string 中文字符串比较
 * 6. 字符串拼接
 * 7. std::quoted 给字符串加双引号""
 * 8. std::from_chars std::to_chars char*和数值互相转换
 * 9. std::string 能否被继承
 */

// std::string的实现方式以及缺点 https://www.zhihu.com/question/54664311?sort=created
// https://blog.csdn.net/weixin_33755554/article/details/93150731
// std::string 的缺点 https://www.zhihu.com/question/35967887?sort=created

#define TEST9

#ifdef TEST1

#include <chrono>
#include <iostream>
#include <string>
#include <string_view>

class Test
{
public:
    Test() : m_startTime(std::chrono::high_resolution_clock::now())
    {
    }

    ~Test()
    {
#if __cplusplus > 201703L && defined WIN32
        std::cout << "Used for " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_startTime)
                  << "\n";
#else
        std::cout << "Used for "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_startTime).count() << "\n";
#endif
    }

    void f1(const std::string_view& str)
    {
        auto x = str;
        std::cout << x << '\n';
    }

    void f2(std::string_view str)
    {
        auto x = str;
        std::cout << x << '\n';
    }

    void f3(const std::string& str)
    {
        auto x = str;
    }

    void f4(std::string str)
    {
        auto x = str;
    }

private:
    std::chrono::high_resolution_clock::time_point m_startTime;
};

constexpr size_t count = 1'000'000;

#define TEST_MICRO(f, x)     \
    {                        \
        Test t;              \
        auto _count = count; \
        while (_count-- > 0) \
        {                    \
            t.f(x);          \
        }                    \
    }

#define TEST_CONSTRUCT_1(...)                    \
    {                                            \
        Test t;                                  \
        auto _count = count;                     \
        while (_count-- > 0)                     \
        {                                        \
            std::string_view sv { __VA_ARGS__ }; \
        }                                        \
    }

#define TEST_CONSTRUCT_2(x)                                     \
    {                                                           \
        Test t;                                                 \
        auto _count = count;                                    \
        while (_count-- > 0)                                    \
        {                                                       \
            std::string_view sv { std::begin(x), std::end(x) }; \
        }                                                       \
    }

std::string_view GetStringView()
{
    std::string str { "this is a string" };
    std::string_view sv { str };
    return sv;
}

std::string_view GetModifiedSV(const std::string_view& sv)
{
    return sv;
}

// std::string_view总结 https://segmentfault.com/a/1190000018387368
// std::string_view仅仅用来作为函数参数，其他时候没必要使用

#define TEST_PRODUCT

int main()
{
#ifdef TEST_PRODUCT

    // 传递参数
    // std::string_view 大约是 std::string效率的十倍
    // 传std::string大约是其他三个(const char*,char[],"")的5-10倍
    for (size_t i = 0; i < 10; ++i)
    {
        const char* szBuf = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        char szArr[]      = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        std::string str { "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" };

        TEST_MICRO(f1, szBuf);
        TEST_MICRO(f2, szBuf);
        TEST_MICRO(f3, szBuf);
        TEST_MICRO(f4, szBuf);

        std::cout << "+++++++++++++++++\n";

        TEST_MICRO(f1, szArr);
        TEST_MICRO(f2, szArr);
        TEST_MICRO(f3, szArr);
        TEST_MICRO(f4, szArr);

        std::cout << "+++++++++++++++++\n";

        TEST_MICRO(f1, str);
        TEST_MICRO(f2, str);
        TEST_MICRO(f3, str);
        TEST_MICRO(f4, str);

        std::cout << "+++++++++++++++++\n";

        TEST_MICRO(f1, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        TEST_MICRO(f2, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        TEST_MICRO(f3, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        TEST_MICRO(f4, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

        std::cout << "++++++++ " << i << i << i << " +++++++++\n";
    }

    std::cout << "=====================================================\n";

    // 构造std::string_view
    // std::string_view存储的是{数据的起始指针,数据的长度}
    // 该数据类型的实例不会具体存储原数据，仅仅存储指向的数据的起始指针和长度，所以这个开销是非常小的。
    for (size_t i = 0; i < 10; ++i)
    {
        char szBuf[] { "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" };
        std::string str { "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" };

        std::string_view sv0 { "abc" };    // 需要通过遍历获取数据的长度，时间复杂度O(n)
        std::string_view sv1 { "abc", 3 }; // 这种方式构造最快，因为数据大小不需要通过遍历再去获取
        std::string_view sv2 { str };
        std::string_view sv3 { szBuf };
#if __cplusplus > 201703L
        std::string_view sv4 { std::begin(str), std::end(str) };
        std::string_view sv5 { std::begin(szBuf), std::end(szBuf) };
#endif

        // [4]最耗时；[0][3]次之；[1]耗时最少
        TEST_CONSTRUCT_1("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");     // [0]
        TEST_CONSTRUCT_1("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 40); // [1]
        TEST_CONSTRUCT_1(str);                                            // [2]
        TEST_CONSTRUCT_1(szBuf);                                          // [3]
#if __cplusplus > 201703L
        TEST_CONSTRUCT_2(str);   // [4]
        TEST_CONSTRUCT_2(szBuf); // [5]
#endif
        std::cout << "++++++++ " << i << i << i << " +++++++++ \n";
    }

    std::cout << "-----------------------------------------------\n";

#endif // TEST_PRODUCT

    // std::string_view 和 std::string 互转
    {
        std::string_view sv { "abcd1234" };
        std::string s { "xyz789" };

        auto svTos = static_cast<std::string>(sv);
        auto sTosv = std::string_view(s);
        std::cout << "string_view <=> string\n" << svTos << '\n' << sTosv << '\n';
    }

    // std::string_view范围内的字符串可能不含有\0
    {
        std::string_view sv { "abcd", 2 };
        auto str = static_cast<std::string>(sv); // ab
        auto sz  = sv.data();                    // abcd 【注意】此处返回的是"abcd"而不是"ab"
        std::cout << str << '\t' << sz << '\n';

        auto lambda = [](const char* s) { std::cout << s << '\n'; };

        lambda(sv.data());
    }

    // std::string_view指向的内容的生命周期可能比其本身短
    {
        auto sv = GetStringView();
        // 打印的内容是乱码
        std::cout << "get string view : " << sv << '\n';
    }

    {
        std::string str = "abc";
        auto sv         = GetModifiedSV(str + "xyz");
        // 获取到的sv并不是"abcxyz"
        std::cout << "get modified sv : " << sv << '\n';
    }

    return 0;
}
#endif // TEST1

#ifdef TEST2

// https://mp.weixin.qq.com/s/tTe7mwoHzTGGTGHwE67LfQ

#include <iostream>
#include <string>

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view
{
public:
    using traits_type = Traits;
    using size_type   = std::size_t;

    using value_type = CharT;

    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;

    using const_iterator         = const value_type*;
    using iterator               = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator       = const_reverse_iterator;

    static constexpr size_type npos = size_type(-1);

public:
    constexpr basic_string_view() noexcept : str_ { nullptr }, size_ { 0 }
    {
    }

    constexpr basic_string_view(const basic_string_view&) noexcept = default;

    constexpr basic_string_view(basic_string_view&&) noexcept = default;

    constexpr basic_string_view(const CharT* str) noexcept : str_ { str }, size_ { traits_type::length(str) }
    {
    }

    constexpr basic_string_view(const CharT* str, size_type len) : str_ { str }, size_ { len }
    {
    }

    constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;

    constexpr basic_string_view(const std::basic_string<CharT, Traits>& str) noexcept : str_ { str.c_str() }, size_ { str.size() }
    {
    }

    explicit constexpr operator std::basic_string<CharT, Traits>() const
    {
        return std::basic_string<CharT, Traits>(this->str_, this->size_);
    }

public:
    constexpr const_iterator begin() const noexcept
    {
        return this->str_;
    }

    constexpr const_iterator end() const noexcept
    {
        return this->str_ + this->size_;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return this->str_;
    }

    constexpr const_iterator cend() const noexcept
    {
        return this->str_ + this->size_;
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(this->end());
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(this->begin());
    }

public:
    constexpr size_type size() const noexcept
    {
        return this->size_;
    }

    constexpr size_type length() const noexcept
    {
        return this->size_;
    }

    constexpr size_type max_size() const noexcept
    {
        // (npos - sizeof(size_type) - sizeof(void*)) / sizeof(value_type) / 4; impl of gcc
        return npos - 1;
    }

    constexpr bool empty() const noexcept
    {
        return this->size_ == 0;
    }

public:
    constexpr const_pointer data() const noexcept
    {
        return this->str_;
    }

    constexpr const_reference operator[](size_type pos) const noexcept
    {
        return *(this->str_ + pos);
    }

    constexpr const_reference at(size_type pos) const
    {
        if (pos >= this->size_)
        {
            throw std::out_of_range { "out of range" };
        }
        return *(this->str_ + pos);
    }

    constexpr const_reference front() const noexcept
    {
        return *this->str_;
    }

    constexpr const_reference back() const noexcept
    {
        return *(this->str_ + this->size_ - 1);
    }

    constexpr void remove_prefix(size_type n) noexcept
    {
        this->str_ += n;
        this->size_ -= n;
    }

    constexpr void remove_suffix(size_type n) noexcept
    {
        this->size_ -= n;
    }

    constexpr void swap(basic_string_view& v) noexcept
    {
        auto temp = *this;
        *this     = v;
        v         = temp;
    }

    constexpr basic_string_view substr(size_type pos = 0, size_type len = npos) const
    {
        if (pos > this->size_)
        {
            throw std::out_of_range { "out of range" };
        }
        const size_type n = std::min(len, this->size_ - pos);
        return basic_string_view(this->str_ + pos, n);
    }

private:
    const CharT* str_;
    size_type size_;
};

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const basic_string_view<CharT, Traits>& str)
{
    os.write(str.data(), static_cast<std::streamsize>(str.size()));
    return os;
}

using string_view = basic_string_view<char>;

// clang++ StringView.cc -std=c++14
int main()
{
    const char* data = "helloWorld 12345678";

    string_view sv1 { data };
    string_view sv2 { data, 5 };
    string_view sv3 { data + 3, 3 };
    string_view sv4 { data + 5 };

    std::string str { "helloWorld" };
    string_view sv5 { str };
    std::string string = static_cast<std::string>(sv5);

    std::cout << data << std::endl;
    std::cout << sv1 << std::endl;
    std::cout << sv2 << std::endl;
    std::cout << sv3 << std::endl;
    std::cout << sv4 << std::endl;
    std::cout << sv5 << std::endl;
    std::cout << string << std::endl;
}
#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <string>

void replace_all(std::string& str, const std::string& old_value, const std::string& new_value)
{
    // [1]
    // while (true) {
    //    std::string::size_type pos(0);
    //    if ((pos = str.find(old_value)) != std::string::npos)
    //        str.replace(pos, old_value.length(), new_value);
    //    else break;
    //}

    // [2]
    std::string::size_type pos(0);
    while ((pos = str.find(old_value)) != std::string::npos)
    {
        str.replace(pos, old_value.length(), new_value);
    }
}

int main()
{
    std::string str5("aaabab123abbabbb");
    replace_all(str5, "ab", "@@");
}

#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <string>
#include <string_view>

#define PRINT(x) std::cout << "size:" << x.size() << "\t" << x << '\n';

int main()
{
    // 内联命名空间
    using namespace std::literals;

    // \0表示字符串的结束

    {
        std::string s1 { "abc\t123" };
        std::string s2 { "abc\t123"s };
        std::string s3 { "abc\0123" };
        std::string s4 { "abc\0123"s };

        PRINT(s1);
        PRINT(s2);
        PRINT(s3);
        PRINT(s4);
    }
    std::cout << "----------------------------\n";
    {
        std::string s1 = "abc\t123";
        std::string s2 = "abc\t123"s;
        std::string s3 = "abc\0123";
        std::string s4 = "abc\0123"s;
        std::string s5 = "abc\0\0def";
        std::string s6 = "abc\0\0def"s;

        PRINT(s1);
        PRINT(s2);
        PRINT(s3);
        PRINT(s4);
        PRINT(s5);
        PRINT(s6);
    }
    std::cout << "----------------------------\n";
    {
        std::string_view s1 = "abc\t123";
        std::string_view s2 = "abc\t123"sv;
        std::string_view s3 = "abc\0123";
        std::string_view s4 = "abc\0123"sv;
        std::string_view s5 = "abc\0\0def";
        std::string_view s6 = "abc\0\0def"sv;

        PRINT(s1);
        PRINT(s2);
        PRINT(s3);
        PRINT(s4);
        PRINT(s5);
        PRINT(s6);
    }
    std::cout << "----------------------------\n";
    {
        std::string s1 = R"(abc\t123)";
        std::string s2 = R"(abc\t123)";
        std::string s3 = R"(abc\0123)";
        std::string s4 = R"(abc\0123)";

        PRINT(s1);
        PRINT(s2);
        PRINT(s3);
        PRINT(s4);
    }
    std::cout << "----------------------------\n";
    {
        std::string_view s1 = R"(abc\t123)";
        std::string_view s2 = R"(abc\t123)";
        std::string_view s3 = R"(abc\0123)";
        std::string_view s4 = R"(abc\0123)";

        PRINT(s1);
        PRINT(s2);
        PRINT(s3);
        PRINT(s4);
    }

    // auto ret = std::string("ss").compare("ss");
    // auto ret1 = std::string("ss").compare("sss");

    // auto l = L"abcd1234";
    // auto u = U"1234abcd";
    // auto aa = 0.0F;
    // auto bb = u8"sss";
    // auto cc = 2l;

    // https://blog.csdn.net/qq_21746331/article/details/111125583
}
#endif // TEST4

#ifdef TEST5

// std::string由于编码的原因，字符串比较会出现不是预期的结果

#include <iostream>
#include <string>

int main()
{
    std::string strZh = "中文字符串";
    if (strZh == "中文字符串")

    {
        std::cout << "111\n";
    }
    if (strZh.compare("中文字符串") == 0)
    {
        std::cout << "222\n";
    }
    if (strZh == std::string("中文字符串"))
    {
        std::cout << "333\n";
    }
}

#endif // TEST5

#ifdef TEST6

#include <array>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <vector>

// #define Method_1

template <typename T>
auto glue(T&& t, std::string separator = "|")
{
#ifdef Method_1
    // 指针不能使用empty()
    if (t.empty())
    {
        return std::string();
    }
    // 指针不能使用begin()等函数
    {
        auto itBeg  = t.begin();
        auto retVal = *itBeg;
        for (auto it = ++itBeg; it != t.end(); ++it)
        {
            retVal += (separator + *it);
        }
        return retVal;
    }
#else
    {
        std::string retVal;
        for (const auto& elem : t)
        {
            retVal += (elem + separator);
        }
        if (!separator.empty())
        {
            return retVal.substr(0, retVal.size() - separator.size());
        }
        return retVal;
    }
#endif
}

int main()
{
    std::set<std::string> setStr { "aa", "bb", "cc", "dd", "ee" };
    std::list<std::string> listStr { "aa", "bb", "cc", "dd", "ee" };
    std::array<std::string, 5> arrayStr { "aa", "bb", "cc", "dd", "ee" };
    std::vector<std::string> vecStr { "aa", "bb", "cc", "dd", "ee" };

    std::cout << glue(setStr, "#") << '\n';
    std::cout << glue(listStr, "@") << '\n';
    std::cout << glue(arrayStr, "$") << '\n';
    std::cout << glue(vecStr, "") << '\n';
    std::cout << glue(std::vector<std::string> {}) << '\n';
    std::cout << glue(std::list<std::string> { "single_str" }, "#") << '\n';

#ifndef Method_1
    std::string pStr[5] { "aa", "bb", "cc", "dd", "ee" };
    std::cout << glue(pStr, "^") << '\n';
#endif // Method_1

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iomanip>
#include <iostream>
#include <sstream>

int main()
{
    std::string str1("aa bb \t cc dd tt \\ ff");
    auto str2 = std::quoted(str1);
    auto str3 = std::quoted(str1, '#', 'd');
    auto str4 = std::quoted(str1, '#');

    /*
        s      - 要插入或释出的字符串
        delim  - 用作分隔符的字符，默认为 " 就是将原字符串用delim包围起来
        escape - 用作转义字符的字符，默认为 \
    */
    std::cout << str1 << '\n' << str2 << '\n' << str3 << '\n' << str4;
}

#endif // TEST7

#ifdef TEST8

#include <charconv>
#include <iostream>

int main()
{
    // number -> char*
    {
        auto f = [](auto&& value, int base = 10)
        {
            // result用来接收转换结果，如果buffer大小不足可能会出现意想不到的结果
            char result[100] { 0 };
            auto [ptr, ec] = std::to_chars(result, result + sizeof(result), value);

            std::cout << "source:\t" << value << '\t';

            if (ec == std::errc())
            {
                std::cout << "convert success\tresult:" << result << '\n';
            }
            else
            {
                std::cout << "convert failed\t\t\tptr:" << ptr << '\n';
            }
        };

        f(100);
        f(11.11);
        f(22.22f);
        f(0x11);
        f(0b0101);
        f('x');
        f(111'111'111'111'111'111);
    }

    std::cout << "----------------------------------------\n";

    // char* -> number
    {
        auto f = [](const char* first, const char* last, int base = 10)
        {
            // 函数参数：char*开始位置，char*结束位置，转换结果，进制（默认10）
            int ret        = 0;
            auto [ptr, ec] = std::from_chars(first, last, ret, base);

            std::cout << "source:\t" << first << '\t';

            if (ec == std::errc())
            {
                std::cout << "convert success\tresult:" << ret << "\tptr:" << ptr << '\n';
            }
            else
            {
                std::cout << "convert failed\t\t\tptr:" << ptr << '\n';
            }
        };

        const char c1[] = "12345";
        const char c2[] = "a1";
        const char c3[] = "12 34";
        const char c4[] = " 123";
        const char c5[] = "123 ";
        const char c6[] = "12a34";
        const char c7[] = "0xa";
        const char c8[] = "A1h12";

        // 必须是数字开头，且遇到非数字(10进制)立马停止转换
        f(c1, c1 + sizeof(c1));
        f(c2, c2 + sizeof(c2));
        f(c3, c3 + sizeof(c3));
        f(c4, c4 + sizeof(c4));
        f(c5, c5 + sizeof(c5));
        f(c6, c6 + sizeof(c6));

        std::cout << "-----------------------------------------\n";

        // 16进制遇到非数字和abcdefABCDEF停止转换
        f(c2, c2 + sizeof(c2), 16);
        f(c7, c7 + sizeof(c7), 16);
        f(c8, c8 + sizeof(c8), 16);
    }
}
#endif // TEST8

#ifdef TEST9

#include <iostream>
#include <string>

// std::string没有被final修饰，所以可以继承，不过std::string的析构函数不是虚函数，所以继承时需要额外注意
// 可以使用private继承std::string

class MyString : private std::string
{
public:
    MyString(const char* str) : std::string(str)
    {
    }

    constexpr size_type size() const noexcept
    {
        return std::string::size();
    }

    constexpr const_pointer data() const noexcept
    {
        return std::string::data();
    }

    friend std::ostream& operator<<(std::ostream& os, MyString& str)
    {
        os << str.data();
        return os;
    }

    // ...
};

int main()
{
    {
        std::string str { "xyz789" };
        std::cout << str.size() << '\t' << str << '\n';
    }

    {
        MyString str { "abc123" };
        std::cout << str.size() << '\t' << str << '\n';
    }

    {
        auto ptr = new std::string("qwert");
        std::cout << ptr->size() << '\t' << *ptr << '\n';
        delete ptr;
    }

    {
        auto ptr = new MyString("123456");
        std::cout << ptr->size() << '\t' << *ptr << '\n';
        delete ptr;
    }

    {
        // 因为是私有继承，类型转换失败，所以不能赋值子类指针给父类指针变量
        // std::string* ptr = new MyString("qwert12345");
    }

    return 0;
}

#endif // TEST9

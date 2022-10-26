/*
* 1. std::string能否被继承
* 2. 自己实现一个 std::string_view
* 3. std::string中的字符替换
* 4. std::string_view 字面值 原生字符串 https://segmentfault.com/a/1190000018387368
* 5. std::string 中文字符串比较
* 6. 字符串拼接
*/

// std::string的实现方式以及缺点 https://www.zhihu.com/question/54664311?sort=created
// https://blog.csdn.net/weixin_33755554/article/details/93150731
// std::string 的缺点 https://www.zhihu.com/question/35967887?sort=created


#define TEST6

#ifdef TEST1

#include <iostream>
#include <string>

// std::string的析构函数不是虚析构函数，所以std::string不能被继承

class MyString :public std::string
{
public:
    MyString(const char* p) :
        std::string(p)
    {
    }

    MyString():
        std::string()
    {
    }
};

int main()
{
    MyString myString("ss");
    auto sz = myString.size();
}

#endif // TEST1

#ifdef TEST2

// https://mp.weixin.qq.com/s/tTe7mwoHzTGGTGHwE67LfQ

#include <iostream>
#include <string>

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_string_view {
public:
    using traits_type = Traits;
    using size_type = std::size_t;

    using value_type = CharT;

    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using const_iterator = const value_type*;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;

    static constexpr size_type npos = size_type(-1);

public:
    constexpr basic_string_view() noexcept : str_{ nullptr }, size_{ 0 } {}

    constexpr basic_string_view(const basic_string_view&) noexcept = default;

    constexpr basic_string_view(basic_string_view&&) noexcept = default;

    constexpr basic_string_view(const CharT* str) noexcept : str_{ str }, size_{ traits_type::length(str) } {}

    constexpr basic_string_view(const CharT* str, size_type len) : str_{ str }, size_{ len } {}

    constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;

    constexpr basic_string_view(const std::basic_string<CharT, Traits>& str) noexcept
        : str_{ str.c_str() }, size_{ str.size() } {}

    explicit constexpr operator std::basic_string<CharT, Traits>() const {
        return std::basic_string<CharT, Traits>(this->str_, this->size_);
    }

public:
    constexpr const_iterator begin() const noexcept { return this->str_; }

    constexpr const_iterator end() const noexcept { return this->str_ + this->size_; }

    constexpr const_iterator cbegin() const noexcept { return this->str_; }

    constexpr const_iterator cend() const noexcept { return this->str_ + this->size_; }

    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(this->end()); }

    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(this->begin()); }

    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(this->end()); }

    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(this->begin()); }

public:
    constexpr size_type size() const noexcept { return this->size_; }

    constexpr size_type length() const noexcept { return this->size_; }

    constexpr size_type max_size() const noexcept {
        // (npos - sizeof(size_type) - sizeof(void*)) / sizeof(value_type) / 4; impl of gcc
        return npos - 1;
    }

    constexpr bool empty() const noexcept { return this->size_ == 0; }

public:
    constexpr const_pointer data() const noexcept { return this->str_; }

    constexpr const_reference operator[](size_type pos) const noexcept { return *(this->str_ + pos); }

    constexpr const_reference at(size_type pos) const {
        if (pos >= this->size_) {
            throw std::out_of_range{ "out of range" };
        }
        return *(this->str_ + pos);
    }

    constexpr const_reference front() const noexcept { return *this->str_; }

    constexpr const_reference back() const noexcept { return *(this->str_ + this->size_ - 1); }

    constexpr void remove_prefix(size_type n) noexcept {
        this->str_ += n;
        this->size_ -= n;
    }

    constexpr void remove_suffix(size_type n) noexcept { this->size_ -= n; }

    constexpr void swap(basic_string_view& v) noexcept {
        auto temp = *this;
        *this = v;
        v = temp;
    }

    constexpr basic_string_view substr(size_type pos = 0, size_type len = npos) const {
        if (pos > this->size_) {
            throw std::out_of_range{ "out of range" };
        }
        const size_type n = std::min(len, this->size_ - pos);
        return basic_string_view(this->str_ + pos, n);
    }

private:
    const CharT* str_;
    size_type size_;
};

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
    const basic_string_view<CharT, Traits>& str) {
    os.write(str.data(), static_cast<std::streamsize>(str.size()));
    return os;
}

using string_view = basic_string_view<char>;


// clang++ StringView.cc -std=c++14
int main() {
    const char* data = "helloWorld 12345678";

    string_view sv1{ data };
    string_view sv2{ data, 5 };
    string_view sv3{ data + 3, 3 };
    string_view sv4{ data + 5 };

    std::string str{ "helloWorld" };
    string_view sv5{ str };
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

#include <string>
#include <iostream>

void replace_all(std::string& str, const std::string& old_value, const std::string& new_value)
{
    // [1]
    //while (true) {
    //    std::string::size_type pos(0);
    //    if ((pos = str.find(old_value)) != std::string::npos)
    //        str.replace(pos, old_value.length(), new_value);
    //    else break;
    //}

    // [2]
    std::string::size_type pos(0);
    while (( pos = str.find(old_value)) != std::string::npos)
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

#include <string>
#include <string_view>
#include <iostream>

int main()
{
    using namespace std::literals;

    std::string_view s1 = "abc\0\0def";
    std::string_view s2 = "abc\0\0def"sv;
    std::string s3 = "abc\0\0def";
    std::string s4 = "abc\0\0def"s;
    std::string s41 = "abc\tdef"s;
    std::string s5 = R"(abc\0\0def)"; // 原生字符串
    std::string s6 = R"(abc\n\ndef)"; // 原生字符串

    std::cout << "s1: " << s1.size() << '\t' << s1 << '\n';
    std::cout << "s2: " << s2.size() << '\t' << s2 << '\n';
    std::cout << "s3: " << s3.size() << '\t' << s3 << '\n';
    std::cout << "s4: " << s4.size() << '\t' << s4 << '\n';
    std::cout << "s5: " << s5.size() << '\t' << s5 << '\n';
    std::cout << "s6: " << s6.size() << '\t' << s6 << '\n';
    std::cout << "s41: " << s41.size() << '\t' << s41 << '\n';

    auto ret = std::string("ss").compare("ss");
    auto ret1 = std::string("ss").compare("sss");

    auto l = L"abcd1234";
    auto u = U"1234abcd";
    auto aa = 0.0F;
    auto bb = u8"sss";
    auto cc = 2l;

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

#include <vector>
#include <list>
#include <set>
#include <array>
#include <string>
#include <iostream>

//#define Method_1

template <typename T>
auto glue(T&& t,std::string separator = "|")
{
#ifdef Method_1
    // 指针不能使用empty()
    if (t.empty())
    {
        return std::string();
    }
    // 指针不能使用begin()等函数
    {
        auto itBeg = t.begin();
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
        if(!separator.empty())
        {
            return retVal.substr(0, retVal.size() - separator.size());
        }
        return retVal;
    }
#endif
}

int main()
{
    std::set<std::string> setStr{ "aa","bb","cc","dd","ee" };
    std::list<std::string> listStr{ "aa","bb","cc","dd","ee" };
    std::array<std::string, 5> arrayStr{ "aa","bb","cc","dd","ee" };
    std::vector<std::string> vecStr{ "aa","bb","cc","dd","ee" };

    std::cout << glue(setStr, "#") << '\n';
    std::cout << glue(listStr, "@") << '\n';
    std::cout << glue(arrayStr, "$") << '\n';
    std::cout << glue(vecStr, "") << '\n';
    std::cout << glue(std::vector<std::string>{}) << '\n';
    std::cout << glue(std::list<std::string>{"single_str"}, "#") << '\n';

#ifndef Method_1
    std::string pStr[5]{ "aa","bb","cc","dd","ee" };
    std::cout << glue(pStr, "^") << '\n';
#endif // Method_1

    return 0;
}

#endif // TEST6

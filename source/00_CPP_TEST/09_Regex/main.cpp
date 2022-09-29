/*
* 1. std::regex基本用法
* 2. 邮箱解析
* 3.
*/


#define TEST2

#ifdef TEST1

#include <regex>
#include <iostream>

int main()
{
    std::regex r1("[a-z0-9]");      // 一个字符小写字母或数字
    std::regex r2("\\d{11}");       // 电话号码
    std::regex r3("[1-9][0-9]*");   // 整数
    std::regex r4("[(a-z)(0-9)]+"); // 一个或多个小写字母或数字（最少一个）
    std::regex r5("[a-z]*");        // 0个或多个小写字母
    std::regex r6("[a-d]");         // 一个字符a\b\c\d

    std::string s1("123_sss");
    std::string s2("12345678901");
    std::string s3("abc12345678900cyz");
    std::string s4("a12345678x900z666sss33");
    std::string s5("123");
    std::string s6("");
    std::string s7("123aaff;88ccdd+++22dd");

    bool ret = std::regex_match(s1, r1);          // 匹配
    bool ret11 = std::regex_match(s5, r2);
    bool ret1 = std::regex_match(s2, r2);
    bool ret2 = std::regex_match(s3, r2);
    bool ret3 = std::regex_search(s3, r2);        // 查找
    bool ret4 = std::regex_search(s4, r2);
    bool ret5 = std::regex_match(s5, r3);
    bool ret6 = std::regex_match(s4, r3);
    bool ret7 = std::regex_match(s4, r4);
    bool ret9 = std::regex_match(s6, r5);
    auto ret10 = std::regex_replace(s7, r6, "哈");  // 替换

    std::smatch sm; //分组使用“()"
    bool ret8 = std::regex_search(s4, sm, r3);
    auto sz = sm.size();

    for (auto elem : sm)
    {
        std::cout << elem << std::endl;
    }

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <regex>
#include <vector>

int main()
{
    std::vector<std::string> emails;
    emails.emplace_back(std::string("boq@gmail.com"));
    emails.emplace_back(std::string("tom@qq.com"));
    emails.emplace_back(std::string("boq@163.com"));
    emails.emplace_back(std::string("boq@outlook.com"));
    emails.emplace_back(std::string("boqoutlook.com"));
    emails.emplace_back(std::string("boq@outlook@com"));

    std::regex e("([[:w:]]+)@([[:w:]]+).com");
    std::smatch m;

    for (auto&& email : emails)
    {
        bool found = std::regex_search(email, m, e);

        if (found)
        {
            //std::cout << "m.size() " << m.size() << std::endl;

            for (int i = 0; i < m.size(); ++i)
            {
                //std::cout << "m.str(" << i << "): " << m.str(i);
                std::cout << m.str(i) << '\t';
            }
            std::cout << std::endl;
            //std::cout << "m.prefix().str(): " << m.prefix().str() << std::endl;
            //std::cout << "m.suffix().str(): " << m.suffix().str() << std::endl;
        }
        else
        {
            std::cout << email << "\tNot Found" << std::endl;
        }
    }

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <regex>
#include <string>

int main()
{
    std::string s("abc123**000aaa");
    std::regex r(".*aaa");

    auto ret = std::regex_match(s, r);

    int test = 0;
}

#endif // TEST3

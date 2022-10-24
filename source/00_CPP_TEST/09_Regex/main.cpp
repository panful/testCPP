/*
* 1. std::regex高级用法
* 2.
* 3. std::regex设置不同文法，设置flag，异常
* 4. std::regex_match   匹配
* 5. std::regex_search  查找 按组查找
* 6. std::regex_replace 替换
* 7. 读文件 std::sregex_iterator
*/

#define TEST7

#ifdef TEST1

#include <iostream>
#include <regex>

#define PRINT_RET(str) std::cout<<std::boolalpha<<str<<std::endl;

int main()
{
    {
        std::string str(R"(1234)");
        std::regex reg(R"([\d]*)");
        // 如果字符串str的格式和正则表达式reg匹配，则返回true
        auto ret = std::regex_match(str, reg);
        PRINT_RET(ret);
    }
    {
        std::string str(R"(1234)");
        std::regex reg(R"([\d]*)");
        // 如果字符串str的格式和正则表达式reg匹配，则返回true
        // 第三个参数可以指定匹配类型
        auto ret = std::regex_match(str, reg, std::regex_constants::match_any);
        PRINT_RET(ret);
    }
    {
        std::string str(R"(abc1234)");
        std::regex reg(R"([\d]*)");
        // 将字符串的某一部分和正则表达式匹配
        auto ret = std::regex_match(str.begin() + 3, str.end(), reg);
        PRINT_RET(ret);
    }
    std::cout << "---------------\n";
    {
        std::string str(R"(123abc123)");
        std::regex reg(R"([a-z]{3})"); // 三个连续的小写字母
        std::smatch sm; // 是一个容器，用来存放查找到的结果
        // str中有三个连续的小写字母，所以查找到了返回true
        auto found = std::regex_search(str, sm, reg);
        if (found)
        {
            for (size_t i = 0; i < sm.size(); ++i)
            {
                auto sub_sm = sm.str();
                std::cout << sub_sm << '\t';
            }
            // prefix 返回的是查找到的字符串的前面的字符
            // suffix 返回的是查找到的字符串的后面的字符
            std::cout << "prefix:" << sm.prefix().str() << "\tsuffix:" << sm.suffix().str() << '\n';
        }
    }
    {
        std::string str(R"(123abc456abc789)");
        std::regex reg(R"([a-z]{3})");
        std::smatch sm;
        // 查找到str中的【第一个】符合正则表达式的字符串就立马返回true
        auto found = std::regex_search(str, sm, reg);
        if (found)
        {
            for (size_t i = 0; i < sm.size(); ++i)
            {
                auto sub_sm = sm.str();
                std::cout << sub_sm << '\t';
            }
            std::cout << "prefix:" << sm.prefix().str() << "\tsuffix:" << sm.suffix().str() << '\n';
        }
    }
    {
        std::string str(R"(abc@123#XYZ=456)");
        std::regex reg(R"(([\d]{3})@([\d]{3})#([X-Z]{3})=([4-6]{3}))");
        std::smatch sm;
        // 所有的组都必须匹配才会返回true
        auto found = std::regex_search(str, sm, reg);
        if (found)
        {
            std::cout << "found\n";
        }
        else
        {
            std::cout << "not found\n";
        }
    }
    {
        std::string str(R"(abc@123#XYZ=456)");
        std::regex reg(R"(([a-c]{3})@([\d]{3})#([X-Z]{3})=([4-6]{3}))");
        std::smatch sm;
        auto found = std::regex_search(str, sm, reg);
        if (found)
        {
            // sm中保存的是每个"()"里边的内容，如果整个字符串匹配，sm的第一个内容是整个字符串
            for (size_t i = 0; i < sm.size(); ++i)
            {
                auto sub_sm = sm.str(i);
                std::cout << sub_sm << '\t';
            }
            // 查找到的是整个字符串，所以没有前缀和后缀，返回空字符串
            std::cout << "prefix:" << sm.prefix().str() << "\tsuffix:" << sm.suffix().str() << '\n';
        }
    }

    std::cout << "----------------------\n";

    {
        // 多个匹配的结果
        std::string str(R"(111aaa111 222bbb222 333ccc333)");
        std::regex reg(R"([\d]{3}[a-z]{3}[\d]{3})"); // 查找由3个数字三个小写字母三个数字组成的长度为9的字符串
        std::sregex_iterator pos(str.cbegin(), str.cend(), reg);
        std::sregex_iterator end;

        for (; pos != end; ++pos)
        {
            std::cout << pos->str() << '\t';
        }
        std::cout << '\n';
    }
    {
        // 多个匹配的结果
        // 简单理解就是用来搜索子字符串
        std::string str(R"(111aaa111###222bbb222$$$$123333ccc333)");
        std::regex reg(R"([\d]{3}[a-z]{3}[\d]{3})"); // 查找由3个数字三个小写字母三个数字组成的长度为9的字符串
        std::sregex_iterator pos(str.cbegin(), str.cend(), reg);
        std::sregex_iterator end;

        for (; pos != end; ++pos)
        {
            std::cout << pos->str() << '\t';
        }
        std::cout << '\n';
    }
    std::cout << "------------------------------\n";
    {
        // 多个匹配的结果
        // 每个匹配结果有多个分组
        std::string str(R"(111aaa111 222bbb222 333ccc333)");
        std::regex reg(R"(([\d]{3})([a-z]{3})([\d]{3}))");
        std::sregex_iterator pos(str.cbegin(), str.cend(), reg);
        std::sregex_iterator end;

        while (pos != end)
        {
            std::cout << "size:" << pos->size() << '\t'; // 每个匹配结果有多少组
            for (size_t i = 0; i < pos->size(); ++i)
            {
                // 此处str()的参数即使大于size()也不会崩溃
                // 打印每一个匹配结果的每一组内容
                std::cout << pos->str(i) << '\t';
            }
            ++pos;
            std::cout << '\n';
        }
    }
    std::cout << "--------------------------------\n";
    {
        // 多个匹配的结果
        // 每个匹配结果有多个分组
        // 获取指定分组的内容
        std::string str = R"(111aaa111 222bbb222 333ccc333)";
        std::regex reg(R"(([\d]{3})([a-z]{3})([\d]{3}))");
        // 最后一个参数表示第几个分组(从0开始），例如上面的正则表达式匹配到了3个结果，每个结果4个分组
        // 这个参数可以为负数，也可以大于匹配到的分组个数，但是此时值就是空的
        std::sregex_token_iterator pos(str.cbegin(), str.cend(), reg, 3);
        std::sregex_token_iterator end;

        for (; pos != end; ++pos)
        {
            std::cout << "每个匹配结果的第三个分组:" << pos->str() << std::endl;
        }
    }
    std::cout << "----------------------------------------\n";
    {
        // 使用匹配结果中的某几个组，生成新的字符串
        std::string str = "bob@gmail.com  tom@hotmail.com leo@163.com";
        std::regex reg("([[:w:]]+)@([[:w:]]+)\\.com"); // c++中'\'需要转义，正则表达式中'.'需要转义，因此需要两个'\'
        std::cout << std::regex_replace(str, reg, "$1 is on $2\n", std::regex_constants::format_no_copy); // 除了捕捉到的组以外，其他的东西均舍弃
    }

    std::cout << "----------------------------------------\n";

    {
        // std::sregex_iterator适用于不确定有几组
        std::string str(R"(#aaa$123$345$456#bbb$111$222#ccc$111#ddd$111$222$333$444#eee$123$456$789)");
        std::regex reg(R"(#([\w]+)([$\w\d]+))");

        std::sregex_iterator pos(str.cbegin(), str.cend(), reg);
        std::sregex_iterator end;

        while (pos != end)
        {
            auto tempStr = pos->str(2);
            std::regex reg2(R"([$][\d]+)"); // 只要符合R"([$][\d]+)"都会被遍历，注意'$'要使用'[]'括起来
            std::sregex_iterator pos2(tempStr.cbegin(), tempStr.cend(), reg2);
            while (pos2 != end)
            {
                std::cout << pos2->str(0) << '\t';
                ++pos2;
            }
            ++pos;
            std::cout << '\n';
        }
    }


    return 0;
}

#endif // TEST1

#ifdef TEST2

#endif // TEST2

#ifdef TEST3

#include <regex>
#include <string>
#include <iostream>

int main()
{
    std::cout << std::boolalpha;
    {
        // 设置不同文法，匹配结果可能不同
        std::string str(R"(123abc###+++)");
        //std::regex reg(R"([\d]{3}[a-z]{3}[#]{3}[+]{3})");  //true
        //std::regex reg(R"([\d]{3}[a-z]{3}[#]{3}[+]{3})", std::regex_constants::grep);  // false
        //std::regex reg(R"([\d]{3}[a-z]{3}[#]{3}[+]{3})", std::regex_constants::egrep);  // false
        std::regex reg(R"([\d]{3}[a-z]{3}[#]{3}[+]{3})", std::regex_constants::awk);  // false

        std::cout << std::regex_match(str, reg) << '\n';
    }
    {
        // 设置不区分大小写
        std::string str(R"(123abc###+++)");
        //std::regex reg(R"([\d]{3}[A-Z]{3}[#]{3}[+]{3})");  // false
        //std::regex reg(R"([\d]{3}[A-Z]{3}[#]{3}[+]{3})", std::regex_constants::icase);  // true
        std::regex reg(R"([\d]{3}[A-Z]{3}[#]{3}[+]{3})", std::regex_constants::icase | std::regex_constants::grep);  // false

        std::cout << std::regex_match(str, reg) << '\n';
    }
    {
        // 构造std::regex抛出异常
        try
        {
            std::regex reg(R"(*)");
        }
        catch (...)
        {
            std::cout << "catch error\n";
        }
    }
}

#endif // TEST3

#ifdef TEST4

#include <string>
#include <regex>
#include <iostream>

#define PRINT_REGEX(str,reg) std::cout << std::regex_match(str, std::regex(reg)) << '\t' << reg << "\t\t" << str << '\n';

int main()
{
    // ===纯字符匹配===
    {
        std::string str(R"(abcd)");
        std::string reg(R"(abcd)"); // true R"(abcd)"完全匹配

        PRINT_REGEX(str, reg);
    }
    {
        std::string str(R"(abcd)");
        std::string reg(R"(ab)"); // false R"(abcd)"和R"(ab)"不匹配

        PRINT_REGEX(str, reg);
    }

    std::cout << "-----------------------------\n";

    // ===元字符===
    {
        // \d表示一个数字 \D非数字字符
        std::string str(R"(123456)");
        std::string reg(R"(\d)"); // false "\d"只能匹配一个数字，"123456"有6个数字，所以不完全匹配
        // "\d"等价于"[0-9]"匹配一个数字，C++默认文法下"[:d:]"和"\d"是等价的，其他文法不保证
        PRINT_REGEX(str, reg);
    }
    {
        // \w表示一个字母或数字或下划线，\W非字母或数字或下划线
        std::string str(R"(abc1234xyz_)");
        std::string reg(R"(\w)"); // false "\w"只能匹配一个字母数字或下划线，所以不完全匹配 
        // "\w"等价于 "[a-zA-Z0-9_]" 匹配单个任意字母或数字或下划线, C++默认文法下[:w:]和\w是等价的，其他文法不保证
        PRINT_REGEX(str, reg);
    }
    {
        // [...]表示...中的任意一个字符，例如[abc]表示abc中的任意一个字符a或b或c，[a-d]表示a或b或c或d中的任意一个字符
        // []中出现的所有字符都是代表本身意思的字符（没有特殊含义），如[.]只能匹配.符号，而不能匹配任意符号
        std::string str(R"(abc1234xyz)");
        std::string reg(R"([\w]+)"); // true 匹配一个或多个字母或数字或下划线
        //std::string reg(R"([]*)"); // false

        PRINT_REGEX(str, reg);
    }
    {
        // \s表示空白符（空格、换行、换页、制表），\S非空白字符
        std::string str("\n\r\t   \r\n");
        std::string reg(R"([\s]+)");    // true 表示一个或多个空白符
        //std::string reg(R"([]+)");      // false
        //std::string reg(R"([]*)");      // false
        //std::string reg(R"([ ]*)");     // false
        //std::string reg(R"([ ]+)");     // false 只能匹配一个或多个空格

        PRINT_REGEX(str, reg);
    }
    {
        // ()分组
        // "\1" "\2"分别表示第一个分组和第二个分组匹配的内容，注意是匹配的内容不是正则表达式的内容
        // 例如 "(x)(y)\2"表示匹配第一个字符是x第二个字符是y第三个字符是y的字符串
        std::string str("123abc===123abc");
        //std::string reg(R"(([\d]{3})([a-c]{3})([=]{3})(123)([abc]*))");    // true
        //std::string reg(R"(([\d]{3})([a-c]{3})([=]{3})(\1)(\2))");         // true 第一个分组匹配到"123"第二个分组匹配到"abc"
        //std::string reg(R"([\d]{3}[a-c]{3}[=]{3}123[abc]*)");              // true
        //std::string reg(R"((1)(2)\3\4)");   // 抛出异常，因为只有两组
        std::string reg(R"((123abc)===\1)");  // true

        PRINT_REGEX(str, reg);
    }
    {
        // .表示除了\n以外的任意一个字符
        std::string str(R"(!\t)"); // 相当于 string("!\\t")所以是3个字符
        std::string reg(R"(!..)"); // true 匹配以"!"开头的，后面两个字符不为"\n"的长度为3的字符串

        PRINT_REGEX(str, reg);
    }
    {
        std::string str("\n");
        std::string reg(R"(.)");   // false .表示除了\n以外的任意一个字符

        PRINT_REGEX(str, reg);
    }
    {
        std::string str("\t");
        std::string reg(R"(.)");   // true "\t"制表符

        PRINT_REGEX(str, reg);
    }
    {
        // [^...]匹配除...以外的任何一个字符，在[]内部使用^表示非，例如[^abc]表示除abc以外的任何一个字符
        // 注意区分不在[]内部使用^
        std::string str(R"(a)");
        std::string reg(R"([^\d])");     // true 表示任何一个非数字字符
        //std::string reg(R"([^a-z])");  // false 匹配除小写字母以外的任何一个字符
        //std::string reg(R"([^\n])");   // true 匹配除换行符'\n'以外的任何一个字符

        PRINT_REGEX(str, reg);
    }
    {
        // x|y 表示x或者y，正则表达式中的|优先级比较混乱，最好使用()将x|y括起来，例如(x|y)不然可能不是自己想要的结果
        std::string str(R"(123X)");
        //std::string reg(R"(123([\d]|[X]))");     // true
        //std::string reg(R"(123[\d]|[X])");       // false 两个选项分别是 "123[\d]" "[X]"
        //std::string reg(R"(.*|X)");              // true
        //std::string reg(R"([\d]{3}|X)");         // false
        //std::string reg(R"([\d]{3}|aaa[X])");    // false 正则表达式两个选项分别是 "[\d]{3}" "aaa[X]"
        std::string reg(R"(([\d]{3}|aaa)[X])");  // true 正则表达式两个选项分别是 "[\d]{3}" "aaa"

        PRINT_REGEX(str, reg);
    }

    {
        // \需要使用转义字符
        std::string str(R"(123\abc/456)");
        //std::string reg(R"(123\abc/456)");  // false
        std::string reg(R"(123\\abc/456)");  // true

        PRINT_REGEX(str, reg);
    }

    std::cout << "-----------------------------\n";

    // ===定位符===
    // 不能将量词与定位符一起使用
    {
        // \b匹配一个单词边界，即一个单词与空格之间的位置
        // \B匹配一个非单词边界
        std::string str("This is a string.");
        std::string reg(R"(This\b \bis\b \ba\b \bstring.)"); // true
        //std::string reg(R"(\bThis\b \bis\b \ba\b \bstring\b.)"); // true
        //std::string reg(R"(\bThis\b \bis\b \ba\b \bstring\b.\b)"); // false
        //std::string reg(R"(This is\b \ba\b \bstring.)"); // true

        PRINT_REGEX(str, reg);
    }
    {
        // 一定注意"^[\t]"和"[^\t]"还有"^\t"的区别
        // ^表示字符串的开头，$表示字符串的结尾
        // C++默认文法std::regex_match使用^或$时必须将其他所有字符都匹配
        std::string str(R"(123abc==)");

        //std::string reg(R"(^\d)");   // false
        //std::string reg(R"(^[\d])"); // false
        //std::string reg(R"(^[1])");  // false
        //std::string reg(R"(^1)");    // false

        //std::string reg(R"(=$)");    // false
        //std::string reg(R"([=]$)");  // false

        //std::string reg(R"(^\d{3}[a-c]{3}[=]{2})");  // true
        std::string reg(R"(^\d{3}[a-c]{3}[=]{2}$)"); // true
        //std::string reg(R"(\d{3}[a-c]{3}[=]{2})");   // true
        //std::string reg(R"(^[1][=]$)");              // false

        PRINT_REGEX(str, reg);
    }

    std::cout << "-----------------------------\n";

    // ===量词===
    {
        // +表示匹配前面的一个字符一次或多次(至少一次)
        std::string str(R"()");
        std::string reg(R"(\w+)");   // false 匹配长度最少为1的由数字或大小写字母或下划线组成的字符串，R"()"中没有任何字符，长度为0，匹配失败
        //std::string reg(R"(+)");   // 调用std::regex(reg)的时候抛出异常，因为+之前没有字符

        PRINT_REGEX(str, reg);
    }
    {
        // *表示匹配前面的字符0次1次或多次
        std::string str(R"()");
        std::string reg(R"(\w*)");   // true 可以匹配任意长度由数字或大小写字母或下划线组成的字符串
        //std::string reg(R"(*)");   // 抛出异常，因为*之前没有任何字符
        PRINT_REGEX(str, reg);
    }
    {
        // ?表示前面的一个字符0次或1次
        std::string str(R"(123a456)");
        std::string reg(R"(123a?456)"); //true 此正则表达式可以匹配123a456或123456

        PRINT_REGEX(str, reg);
    }
    {
        // {n}表示前面的字符必须出现n次，n>=0，例如 x{5}表示x必须出现5次
        std::string str(R"(11aabb)");
        std::string reg(R"(11a{2}bb)");     // true 只能匹配"11aabb"
        //std::string reg(R"({0})");        // 抛出异常，因为{0}之前没有任何字符
        //std::string reg(R"(11aax{0}bb)"); // true 相当于 R"(11aabb)"
        //std::string reg(R"(11a{3}bb)");   // false "11aabb"只有两个'a'，不匹配

        PRINT_REGEX(str, reg);
    }
    {
        // {n,}表示前面的字符至少出现n次，n>=0，例如 x{5,}表示x必须至少出现5次
        std::string str(R"(11aabb)");
        //std::string reg(R"(11a{2,}bb)");    // true 可以匹配"11aabb" "11aaabb" "11aaaaaaaabb"等等
        //std::string reg(R"({0,})");         // 抛出异常，因为{0,}之前没有任何字符
        //std::string reg(R"(11aax{0,}bb)");  // true 可以匹配"11aabb" "11aaxbb" "11aaxxxxxbb"等等
        std::string reg(R"(11a{3,}bb)");      // false "11aabb"只有两个a，正则表达式要求最少3个'a'

        PRINT_REGEX(str, reg);
    }
    {
        // {n,m}表示前面的字符出现n~m次，n>=0,m>=n，例如 x{5,8}表示x出现5次或6次或7次或8次
        std::string str(R"(11aabb)");
        //std::string reg(R"(11a{1,5}bb)");     // true 可以匹配 "11abb" "11aabb" "11aaabb" "11aaaabb" "11aaaaabb"
        //std::string reg(R"({0,1})");          // 抛出异常，因为{0,1}之前没有任何字符
        //std::string reg(R"({2,1})");          // 抛出异常，因为2>1
        //std::string reg(R"(11aax{0,0}bb)");   // true 只能匹配 "11aabb"
        std::string reg(R"(11aax{0,1}bb)");     // true 可以匹配 "11aabb"或"11aaxbb"

        PRINT_REGEX(str, reg);
    }

    std::cout << "-----------------------------\n";
    // ===中文===
    // 打印中文有问题，暂时先不做
    {
        // 使用std::wregex时 \w可以匹配中文
        std::wstring str(L"中文abc中文123中文a_1_a");
        //std::wstring str(L"中文恭喜發財");
        //std::wstring reg(L"[\u4e00-\u9fa5]*"); // 只匹配中文，繁体也可以匹配
        //std::wstring reg(L"中文abc中文123中文");
        std::wstring reg(L"([\\w]*)"); // 可以匹配中文、英文字母、数字、下划线
        //std::wstring reg(L"[\u4e00-\u9fa5a-c1-3]*");  // 匹配中文以及"abc123"

#define PRINT_REGEX_CHN(str,reg) std::wcout << std::regex_match(str, std::wregex(reg)) << '\n';
        PRINT_REGEX_CHN(str, reg);
    }
}

#endif // TEST4

#ifdef TEST5

#include <string>
#include <regex>
#include <iostream>

#define PRINT_SEARCH(str,reg) std::cout << std::regex_search(str, std::regex(reg))<<'\t' << reg<<"\t\t\t" <<str<<'\n';
#define PRINT_SEARCH2(str,reg) do\
{\
std::cout<<str<<'\t'<<reg<<'\t';\
std::smatch sm;\
std::cout << std::regex_search(str, sm, std::regex(reg)) << '\t' << sm.size() << '\t';\
for (auto& elem : sm){\
    std::cout << elem.str() << '\t';\
}\
std::cout<<'\n';\
} while (false);


int main()
{
    std::cout << std::boolalpha;

    {
        std::string str(R"(abc123xyz456)");
        std::string reg(R"(xyz)");
        auto ret = std::regex_search(str, std::regex(reg)); // 只要找到就返回true
    }
    {
        std::string str("123abc456");
        //std::string reg(R"(abc)");       // true
        //std::string reg(R"(\w)");        // true
        //std::string reg(R"([\w]*)");     // true
        //std::string reg(R"([\w]{9})");   // true
        //std::string reg(R"([\w]{10})");  // false str没有10个字母或数字或下划线，查找失败
        std::string reg(R"([\d]{3}abc[456]+)");  // true

        PRINT_SEARCH(str, reg);
    }
    std::cout << "-----------------------------\n";
    {
        std::string str(R"(123abc456)");
        std::string reg(R"(([\d]+)([a-z]+)([\w]+))");

        std::smatch sm;
        auto ret = std::regex_search(str, sm, std::regex(reg)); // 每组都匹配到才返回true，会将匹配的每一组放到std::smatch中
        auto size = sm.size(); // 总共匹配到几组，一个()是一组，整个正则表达式也是一组，所以此处返回的是：()的个数+1
        auto prefix = sm.prefix();
        auto strPrefix = sm.prefix().str();
        //sm.format();
        //sm.position();
        auto suffix = sm.suffix();
        auto strSuffix = sm.suffix().str();
        for (auto& elem : sm)
        {
            auto str = elem.str(); // 每一组匹配结果，
        }
    }
    {
        std::string str(R"(yang@leo.com)");
        std::string reg(R"(([\w]+)@([\w]+)[.]([\w]+))");     // 4       yang@leo.com    yang    leo     com
        //std::string reg(R"(^([\w]+)@([\w]+)[.]([\w]+)$)");  // 4       yang@leo.com    yang    leo     com
        //std::string reg(R"(([\w]))");  // 2 y y
        //std::string reg(R"([\w]+)");  // 1 yang
        //std::string reg(R"(([\w]+))");  // 2 yang yang
        //std::string reg(R"(([\w]+)@([\d]+).([\d]+))"); // false

        PRINT_SEARCH2(str, reg);
    }
}

#endif // TEST5

#ifdef TEST6

#include <regex>
#include <iostream>

int main()
{
    {
        std::regex reg(R"([a-d])"); // 一个字符a或b或c或d
        std::string str(R"(123abc==cx--ss#a#b++)"); // 123哈哈哈==哈x--ss#哈#哈++
        std::cout << std::regex_replace(str, reg, "哈") << '\n';     // 将查找到的a或b或c或d全部替换为"哈"
    }

    {
        std::regex reg(R"(aa.*bb)"); // 以"aa"开头"bb"结尾，且中间没有换行符'\n'的字符串
        //std::string str(R"(123aa++==123bb456)");    // 123XXX456
        //std::string str("123aa++=\t=\t123bb456");   // 123XXX456
        //std::string str("123aa++=\n=\t123bb456");   // 替换失败，返回原字符串
        std::string str("123aa++=  =\t123bb456");   // 123XXX456
        std::cout << std::regex_replace(str, reg, "XXX") << '\n';     // 将查找到的字符串替换为"XXX"
    }

    {
        std::regex reg(R"(image:url([\w]*);)");         // 以"image:url("开头，以");"结尾的字符串，且字符串中间的字符为0个1个或多个数字或字母或下划线
        std::string str(R"({image:url(./resource/test.svg);}this is a svg file.)"); // {image:url(./resource/test.svg);}this is a svg file.
        std::cout << std::regex_replace(str, reg, "new_file") << '\n';     // 将查找到的路径替换为new_file
    }

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <string_view>

int main()
{
    {
        std::ifstream ifs("resource/testRegex.txt");
        std::stringstream ss;
        ss << ifs.rdbuf();
        auto str = ss.str();

        //std::regex reg(R"([#][\s\w\d-./]*)");
        std::regex reg1(R"([#][^#]*)");
        std::sregex_iterator pos1(str.cbegin(), str.cend(), reg1);
        std::sregex_iterator end;
        while (pos1 != end)
        {
            auto temp = pos1->str();

            std::regex reg2(R"(.*[\n])");
            std::sregex_iterator pos2(temp.cbegin(), temp.cend(), reg2);
            while (pos2 != end)
            {
                std::cout << pos2->str();
                ++pos2;
            }
            ++pos1;
            std::cout << "------------\n";
        }
    }
}

#endif // TEST7

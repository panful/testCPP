/*
* 1. std::map.try_emplace(value)
* 2. std::map常用函数
*/

#define TEST1

#include <map>
#include <string>
#include <iostream>

#ifdef TEST1

int main()
{
    std::map<int, std::string> myMap;
    myMap.emplace(1, "sss");
    myMap.try_emplace(1, "333");  // try_emplace不会构造 std::pair(1,"333")  c++17支持
    myMap.emplace(1, "eee");      // emplace会先构造 std::pair(1,"eee")插入失败，然后再析构pair

    std::cout << myMap.at(1);
}

#endif // TEST1

#ifdef TEST2

#include <map>
#include <algorithm>
#include <iterator>

int main()
{
    {
        std::map<int, int> m{ {1,1} };
        std::map<int, int> m2{ {2,2} };
        std::map<int, int> m3;
        std::merge(m.begin(), m.end(), m2.begin(), m2.end(), std::inserter(m3, m3.begin()));
        m2.insert(m.begin(), m.end());
        m2.erase(1);
    }

    {
        std::map<int, char> m;
        auto ret1 = m.emplace(1, 'c');
        auto ret2 = m.emplace(1, 'b');
        auto ret3 = m.emplace(2, 'a');
    }

}

#endif // TEST2


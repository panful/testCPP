/**
 * 101. std::transform 不保证按顺序在一个范围内应用函数
 * 102. std::for_each 按顺序在一个范围内应用函数
 * 103. std::set_difference set_symmetric_difference set_union set_intersection set_difference 已排序范围上的集合操作
 * 104. std::search 查找子序列 std::search_n 查找多个连续的相同元素
 * 105. std::includes 在已排序的范围中查找子序列
 * 106. std::find_end 在一个范围中查找子序列最后一次出现的位置
 * 107. std::find std::find_if std::find_if_not 查找范围中满足判别标准的首个元素迭代器
 * 108. std::find_first_of 在给定范围中查找另一个范围中的任意元素
 * 109. std::adjacent_find 在给定范围中查找首对连续相同元素的位置，可以自定义连续两个元素满足指定要求，比如递增、递减、升序、降序...
 * 110. std::copy std::copy_if std::copy_n std::copy_backward 拷贝指定范围内的元素到指定范围
 * 111. std::fill std:fill_n std::generate std::generate 对指定范围填充指定值
 * 112. std::remove_copy std::remove_copy_if std::partition_copy 将指定范围中满足特定条件的元素拷贝到指定范围
 * 113. std::partition std::is_partitioned std::stable_partition std::partition_point
 * 114. std::is_sorted std::is_sorted_until
 * 115. std::lower_bound std::upper_bound std::equal_range
 * 116. std::binary_search
 * 117. std::any_of std::none_of std::all_of 检查范围中的元素（所有或任意）是否对lambda返回true或false
 * 118. std::merge std::inplace_merge 合并两个排序的范围，注意和103区别
 * 119. std::sort std::stable_sort std::partial_sort std::partial_sort_copy std::nth_element
 *
 * 201. std::execution 并行算法
 * 202. 并行算法数据竞争问题
 * 203. 使用 std::iota 生成连续递增的数字序列，用来索引并行算法
 *
 * 301. std::reduce std::accumulate 将一个范围内的元素累加起来、相乘、组合等
 * 302. std::transform_reduce 并行地对一个范围内的元素变换并求和
 *
 * 401. std::exchange
 */

// https://zh.cppreference.com/w/cpp/header/algorithm
// c++并行算法 openmp tbb mkl opencl => 06_02

#define TEST119

#ifdef TEST101

#include <algorithm>
#include <format>
#include <iostream>
#include <string>
#include <vector>

template <typename T>
void Print(T&& t)
{
    std::cout << std::format("size: {}\telements: ", t.size());
    for (auto elem : t)
    {
        std::cout << elem << ' ';
    }
    std::cout << std::endl;
}

// 注意：std::transform 不保证按顺序应用函数对象

int main()
{
    // 使用begin()作为插入迭代器
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2(vec1.size()); // 必须给接收者预分配大小，不能使用reserve预分配内存
        std::transform(vec1.cbegin(), vec1.cend(), vec2.begin(), [](int val) { return val * val; });
        Print(vec2);
    }

    // 使用std::back_inserter作为插入迭代器
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2;
        vec2.reserve(vec1.size()); // 只需要给接收者预分配内存（不分配也可以，效率低）
        std::transform(vec1.cbegin(), vec1.cend(), std::back_inserter(vec2), [](int val) { return val * val; });
        Print(vec2);
    }

    // 两个输入，一个输出
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2 { 2, 3, 4, 5, 6, 7 }; // 第二个输入不能小于第一个输入的长度
        std::vector<int> vec3(vec1.size());
        std::transform(vec1.cbegin(), vec1.cend(), vec2.cbegin(), vec3.begin(), [](auto&& a, auto&& b) { return a + b; });
        Print(vec3);
    }

    // 同一个容器既是输入也是输出
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::transform(vec1.cbegin(), vec1.cend(), vec1.begin(), [](auto&& a) { return a * 2; });
        Print(vec1);
    }
}

#endif // TEST101

#ifdef TEST102

#include <algorithm>
#include <iostream>
#include <vector>

// std::for_each 保证按顺序应用函数对象

int main()
{
    // std::for_each 就是一个for循环
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2;
        std::for_each(vec1.begin(), vec1.end(), [](int& i) { i = i * 2; }); // 引用捕获，修改vec1的值
        std::for_each(vec1.begin(), vec1.end(), [](int i) {}); // std::for_each的lambda不需要返回值，值捕获相当于啥都没干
        std::for_each(vec1.begin(), vec1.end(), [&vec2](const int i) { vec2.emplace_back(i * 2); });
    }

    // std::for_each 的 continue
    {
        std::vector<int> vec { 1, 2, 3, 4, 5 };

        // return 类似于continue
        std::for_each(vec.cbegin(), vec.cend(),
            [](auto val)
            {
                if (3 == val)
                {
                    // std::for_each 中的 return 和循环中的 continue 效果一样
                    // 在std::for_each 的lambda中不能使用continue
                    return;
                }

                std::cout << val << ' ';
            });

        std::cout << std::endl;
    }

    // std::for_each 的 break
    {
        std::vector<int> vec { 1, 2, 3, 4, 5 };

        try
        {
            std::for_each(vec.cbegin(), vec.cend(),
                [](auto val)
                {
                    if (3 == val)
                    {
                        throw "break";
                    }

                    std::cout << val << ' ';
                });
        }
        catch (...)
        {
        }

        std::cout << std::endl;
    }
}

#endif // TEST102

#ifdef TEST103

#include <algorithm>
#include <iostream>
#include <iterator>
#include <set>
#include <vector>

// MSVC实现和这个肯定不一样，g++可能一样
template <class InputIt1, class InputIt2, class OutputIt, class Compare>
OutputIt my_set_difference(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, OutputIt d_first, Compare comp)
{
    while (first1 != last1)
    {
        // 如果第二个set为空，直接返回set1的拷贝
        if (first2 == last2)
            return std::copy(first1, last1, d_first);

        if (comp(*first1, *first2))
        {
            *d_first++ = *first1++;
        }
        else
        {
            if (!comp(*first2, *first1))
            {
                ++first1;
            }
            ++first2;
        }
    }

    return d_first;
}

int main()
{
    std::set<int> set1 { 1, 2, 3 };
    std::set<int> set2 { 0, 2, 4 };
    std::set<int> set3 { 7, 3, 8 }; // set3 = {3,7,8}
    std::set<int> set4;
    std::set<int> set5;
    std::set<int> set6;
    std::set<int> set7;
    std::set<int> set8;

    // 将set2中有，set1中没有，且大于2的元素，插入到set8的开始位置
    // MSVC使用同样的lambda会崩溃，g++可以使用
    my_set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::inserter(set8, set8.begin()),
        [](auto&& a, auto&& b)
        {
            // std::cout << a << ',' << b << '\n';
            return a > 2;
        });

    // 以下函数参数不一定非要std::set，只要容器元素满足升序都可以，例如：
    std::vector<int> v1 { 1, 2, 3 };
    std::vector<int> v2 { 0, 2, 4 };
    std::vector<int> v3;
    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3)); // v3 = {1,3}

    // 差集 将set2中有，set1中没有的元素，插入到set7开始位置
    std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::inserter(set7, set7.begin())); // set7 = {0,4}
    // 差集，set1中的元素在set2中没有的元素
    std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),
        std::inserter(set3, set3.end())); // set3 = 1,3,7,8; 1,3在set2中没有，std::inserter用法请看02_04_Iterator
    // 对称差集，除过两个集合中相同元素以外的所有元素加起来
    std::set_symmetric_difference(
        set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(set4, set4.end())); // set4 = 0,1,3,4; 除过2都有，其他所有元素加起来
    // 并集，两个集合的所有元素加起来
    std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(set5, set5.begin())); // set5 = 0,1,2,3,4;
    // 交集，两个集合都有的元素
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(set6, set6.begin())); // set6 = 2; 两个集合都有2

    //
    // std::set_new_handler();
    // std::set_terminate();
    // std::set_unexpected();
}

#endif // TEST103

#ifdef TEST104

#include <algorithm>
#include <functional> // std::boyer_moore_searcher
#include <iostream>
#include <string>
#include <vector>

int main()
{
    // std::search 查找子序列，如果查找到，返回子序列首次出现的位置
    {
        std::vector<int> vec1 { 1, 3, 5, 7, 9, 0, 2, 4, 6, 8 };
        std::vector<int> vec2 { 9, 0, 2 };
        std::vector<int> vec3 { 5, 6, 7 };

        if (auto result = std::search(vec1.cbegin(), vec1.cend(), vec2.cbegin(), vec2.cend()); result != vec1.cend())
        {
            std::cout << "Found subsequence\n";
        }
        if (auto result = std::search(vec1.cbegin(), vec1.cend(), vec3.cbegin(), vec3.cend()); result == vec1.cend())
        {
            std::cout << "No subsequences found\n";
        }
    }

    // 指定查找子序列的算法
    {
        std::vector<int> vec1 { 1, 3, 5, 7, 9, 0, 2, 4, 6, 8 };
        std::vector<int> vec2 { 9, 0, 2 };
        std::vector<int> vec3 { 5, 6, 7 };

        // 使用BM(boyer-moore)算法查找子序列
        const std::boyer_moore_searcher bm_searcher2(vec2.cbegin(), vec2.cend());
        const std::boyer_moore_searcher bm_searcher3(vec3.cbegin(), vec3.cend());

        if (auto result = std::search(vec1.cbegin(), vec1.cend(), bm_searcher2); result != vec1.cend())
        {
            std::cout << "BM: Found subsequence\n";
        }
        if (auto result = std::search(vec1.cbegin(), vec1.cend(), bm_searcher3); result == vec1.cend())
        {
            std::cout << "BM: No subsequences found\n";
        }

        // 使用 Boyer-Moore-Horspool 算法查找子序列
        const std::boyer_moore_horspool_searcher bmh_searcher2(vec2.cbegin(), vec2.cend());
        const std::boyer_moore_horspool_searcher bmh_searcher3(vec3.cbegin(), vec3.cend());
        if (auto result = std::search(vec1.cbegin(), vec1.cend(), bmh_searcher2); result != vec1.cend())
        {
            std::cout << "BM_horspool: Found subsequence\n";
        }
        if (auto result = std::search(vec1.cbegin(), vec1.cend(), bmh_searcher3); result == vec1.cend())
        {
            std::cout << "BM_horspool: No subsequences found\n";
        }
    }

    // 自定义比较方法
    {
        std::vector<int> vec1 { 1, 3, 5, 7, 9, 0, 2, 4, 6, 8 };
        std::vector<int> vec2 { 9, 0, 2 };
        std::vector<int> vec3 { 5, 6, 7 };

        if (auto result = std::search(vec1.cbegin(), vec1.cend(), vec2.cbegin(), vec2.cend(), [](auto&& a, auto&& b) { return a == b; });
            result != vec1.cend())
        {
            std::cout << "Found subsequence\n";
        }
        if (auto result = std::search(vec1.cbegin(), vec1.cend(), vec3.cbegin(), vec3.cend(), [](auto&& a, auto&& b) { return a == b; });
            result == vec1.cend())
        {
            std::cout << "No subsequences found\n";
        }
    }

    // 查找多个连续的相同元素
    {
        std::string str { "1001010100010101001010101" };

        if (auto result = std::search_n(str.cbegin(), str.cend(), 4, '0'); result == str.cend())
        {
            std::cout << "Multiple consecutive elements were not found\n";
        }
        if (auto result = std::search_n(str.cbegin(), str.cend(), 3, '0'); result != str.cend())
        {
            std::cout << "Multiple consecutive elements found\n";
        }
    }
}
#endif // TEST104

#ifdef TEST105

#include <algorithm>
#include <cctype>
#include <format>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    // 在已排序的范围上查找已排序的子序列，如果查找到返回true
    // 如果乱序则程序中断
    // 子序列可以不连续
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        std::vector<int> vec2 { 3, 4, 5 };
        std::vector<int> vec3 { 2, 4, 6 }; // 子序列可以不连续
        std::vector<int> vec4 { 1, 2, 9 };
        std::vector<int> vec5 { 0, 1, 2 };

        auto v2 = std::includes(vec1.cbegin(), vec1.cend(), vec2.cbegin(), vec2.cend());
        auto v3 = std::includes(vec1.cbegin(), vec1.cend(), vec3.cbegin(), vec3.cend());
        auto v4 = std::includes(vec1.cbegin(), vec1.cend(), vec4.cbegin(), vec4.cend());
        auto v5 = std::includes(vec1.cbegin(), vec1.cend(), vec5.cbegin(), vec5.cend());

        std::cout << std::format("vec2: {}\nvec3: {}\nvec4: {}\nvec5: {}\n", v2, v3, v4, v5);
    }

    // 自定义比较方法，注意lambda中的比较符号是'<'，因为输入默认都是升序
    {
        std::string str1 { "abcdefg" };
        std::string str2 { "ace" };
        std::string str3 { "ABC" };

        auto s2 = std::includes(str1.cbegin(), str1.cend(), str2.cbegin(), str2.cend());
        auto s3 = std::includes(str1.cbegin(), str1.cend(), str3.cbegin(), str3.cend());
        auto s3_nocase =
            std::includes(str1.cbegin(), str1.cend(), str3.cbegin(), str3.cend(), [](char a, char b) { return std::tolower(a) < std::tolower(b); });

        std::cout << std::format("str2: {}\nstr3 case: {}\nstr3 nocase: {}\n", s2, s3, s3_nocase);
    }
}

#endif // TEST105

#ifdef TEST106

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main()
{
    {
        std::string str1 { "ababbaabb" };
        std::string str2 { "ab" };
        std::string str3 { "ba" };

        auto cend  = str1.cend();
        auto Print = [cend](auto& it)
        {
            std::cout << "distance to the end: " << std::distance(it, cend) << "\t elemnts: ";
            for (; it != cend; it++)
            {
                std::cout << *it << ' ';
            }
            std::cout << std::endl;
        };

        if (auto it = std::find_end(str1.cbegin(), str1.cend(), str2.cbegin(), str2.cend()); it != cend)
        {
            Print(it);
        }
        if (auto it = std::find_end(str1.cbegin(), str1.cend(), str3.cbegin(), str3.cend()); it != cend)
        {
            Print(it);
        }
    }
}

#endif // TEST106

#ifdef TEST107

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

int main()
{
    std::vector<int> v { 1, 2, 3, 4 };
    int n1 = 3;
    int n2 = 5;

    auto cend  = v.cend();
    auto Print = [cend](auto it, auto val)
    {
        if (it != cend)
        {
            std::cout << "found: " << *it << std::endl;
        }
        else
        {
            std::cout << "not found: " << val << std::endl;
        }
    };

    // std::find 搜索等于(==)val的元素
    auto result1 = std::find(v.cbegin(), v.cend(), n1);
    auto result2 = std::find(v.cbegin(), v.cend(), n2);
    // std::find_if 搜索函数对象返回true的元素
    auto result3 = std::find_if(v.cbegin(), v.cend(), [](int i) { return i % 2 == 0; });
    // std::find_if_not 搜索函数对象返回false的元素
    auto result4 = std::find_if_not(v.cbegin(), v.cend(), [](int i) { return i % 2 == 0; });

    Print(result1, n1);
    Print(result2, n2);
    Print(result3, "even");
    Print(result4, "odd");
}

#endif // TEST107

#ifdef TEST108

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v1 { 1, 2, 4, 5, 7, 9 };
    std::vector<int> v2 { 2, 3, 4 };
    std::vector<int> v3 { 6, 4, 2 };
    std::vector<int> v4 { 4, 5 };
    std::vector<int> v5 { 3, 6 };
    std::vector<int> v6 { 3, 6, 2 };

    auto cend  = v1.cend();
    auto Print = [cend](auto it)
    {
        if (it != cend)
        {
            std::cout << "found: " << *it << std::endl;
        }
        else
        {
            std::cout << "not found\n";
        }
    };

    // 在v1中查找v2中的任意元素，只要在v1中找到符合要求的元素，立即返回指向v1的迭代器
    auto it2 = std::find_first_of(v1.cbegin(), v1.cend(), v2.cbegin(), v2.cend());
    auto it3 = std::find_first_of(v1.cbegin(), v1.cend(), v3.cbegin(), v3.cend());
    auto it4 = std::find_first_of(v1.cbegin(), v1.cend(), v4.cbegin(), v4.cend());
    auto it5 = std::find_first_of(v1.cbegin(), v1.cend(), v5.cbegin(), v5.cend());
    // 在v1中查找函数对象返回true的第一个元素的迭代器
    auto it6 = std::find_first_of(v1.cbegin(), v1.cend(), v2.cbegin(), v2.cend(), [](auto a, auto b) { return b * 2 == a; });

    Print(it2);
    Print(it3);
    Print(it4);
    Print(it5);
    Print(it6);
}

#endif // TEST108

#ifdef TEST109

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> v1 { 3, 2, 1, 1, 6, 5, 5 };
    std::vector<int> v2 { 0, 1, 4, 4, 7, 8, 8 };

    auto Print = [](auto& it, const auto cend)
    {
        std::cout << "distance to the end: " << std::distance(it, cend) << "\t elemnts: ";
        for (; it != cend; it++)
        {
            std::cout << *it << ' ';
        }
        std::cout << std::endl;
    };

    auto it1 = std::adjacent_find(v1.cbegin(), v1.cend());
    auto it2 = std::adjacent_find(v2.cbegin(), v2.cend());
    // 自定义相邻两个元素的比较方法，函数对象返回true，则直接返回当前迭代器
    auto it3 = std::adjacent_find(v1.cbegin(), v1.cend(), std::greater<int>()); // 如果整个范围是升序，返回end()
    auto it4 = std::adjacent_find(v2.cbegin(), v2.cend(), std::greater<int>()); // 如果整个范围是升序，返回end()

    Print(it1, v1.cend());
    Print(it2, v2.cend());
    Print(it3, v1.cend());
    Print(it4, v2.cend());
}

#endif // TEST109

#ifdef TEST110

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2(10);
        std::vector<int> vec3;
        vec3.reserve(5);

        // 将vec1的所有元素拷贝至vec2的开始位置
        std::copy(vec1.cbegin(), vec1.cend(), vec2.begin()); // 从vec2.begin()到vec2.end()必须不小于vec1的大小

        // 将vec1的所有元素拷贝至vec2的末尾
        std::copy(vec1.cbegin(), vec1.cend(), std::back_inserter(vec3)); // vec3不需要提前分配大小，可以提前分配内存
    }

    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2(5);
        std::vector<int> vec3;
        vec3.reserve(5);

        std::copy_n(vec1.cbegin(), 3, vec2.begin()); // 将vec1的前3个元素拷贝到vec2的开始位置
        std::copy_n(vec1.cbegin(), 5, std::back_inserter(vec3));
    }

    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2(5);
        std::vector<int> vec3;
        vec3.reserve(5);

        // 将使函数对象返回true的元素拷贝至指定位置
        std::copy_if(vec1.cbegin(), vec1.cend(), vec2.begin(), [](auto val) { return val % 2 == 0; });
        std::copy_if(vec1.cbegin(), vec1.cend(), std::back_inserter(vec3), [](auto val) { return val % 2 == 0; });
    }

    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2(10);
        std::vector<int> vec3;

        // 将vec1的所有元素复制到终于vec2.end()的范围
        std::copy_backward(vec1.cbegin(), vec1.cend(), vec2.end());

        // 只支持双向迭代器，不支持插入迭代器
        // std::copy_backward(vec1.cbegin(), vec1.cend(), std::back_inserter(vec3));
    }
}

#endif // TEST110

#ifdef TEST111

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    {
        std::vector<int> vec { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        // 将指定范围内的所有元素设置为指定值
        std::fill(vec.begin(), vec.end(), -1);
    }

    {
        std::vector<int> vec { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        // 将从指定位置开始连续的N个元素设置为指定值，（开始位置+N）不能超出容器大小
        std::fill_n(vec.begin(), 5, -1);
    }

    {
        std::vector<int> vec { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        int number { 10 };

        // 将指定范围内的所有元素设置为函数对象的返回值
        std::generate(vec.begin(), vec.end(), [&number]() { return number++; });
    }

    {
        std::vector<int> vec { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        // 将从指定位置开始连续的N个元素设置为函数对象的返回值
        std::generate_n(vec.begin(), 5, []() { return -1; });
    }
}

#endif // TEST111

#ifdef TEST112

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2(5);
        std::vector<int> vec3;
        vec3.reserve(5);

        // 将指定范围的元素复制到指定位置，忽略满足特定标准的值
        std::remove_copy(vec1.cbegin(), vec1.cend(), vec2.begin(), 3);             // 1245
        std::remove_copy(vec1.cbegin(), vec1.cend(), std::back_inserter(vec3), 3); // 1245
    }

    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2(5);
        std::vector<int> vec3;
        vec3.reserve(5);

        // 将指定范围的元素复制到指定位置，忽略使函数对象返回true的元素
        std::remove_copy_if(vec1.cbegin(), vec1.cend(), vec2.begin(), [](auto val) { return val % 2 == 0; });             // 135
        std::remove_copy_if(vec1.cbegin(), vec1.cend(), std::back_inserter(vec3), [](auto val) { return val % 2 == 0; }); // 135
    }

    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2(5);
        std::vector<int> vec3(5);

        // 将指定范围的元素，按照函数对象返回的类别分为两组：函数对象返回true一组，返回false另一组
        std::partition_copy(vec1.cbegin(), vec1.cend(), vec2.begin(), vec3.begin(), [](auto val) { return val % 2 == 0; });
    }
}

#endif // TEST112

#ifdef TEST117

#include <algorithm>
#include <string>
#include <vector>

int main()
{
    {
        std::vector<int> vec { 1, 3, 5, 7, 9 };
        // lambda对所有元素返回false，则none_of返回true
        auto none = std::none_of(vec.cbegin(), vec.cend(), [](auto v) { return v % 2 == 0; }); // true
        // lambda对所有元素返回true，则all_of返回true
        auto all = std::all_of(vec.cbegin(), vec.cend(), [](auto v) { return v == 3; }); // false
        // lambda对任意至少一个元素返回true，则any_of返回true
        auto any = std::any_of(vec.cbegin(), vec.cend(), [](auto v) { return v < 3; }); // true
    }

    {
        std::string str { "12345@#abcde" };
        auto none = std::none_of(str.cbegin(), str.cend(), [](auto v) { return std::isupper(v); }); // true
        auto all  = std::all_of(str.cbegin(), str.cend(), [](auto v) { return std::isdigit(v); });  // false
        auto any  = std::any_of(str.cbegin(), str.cend(), [](auto v) { return std::islower(v); });  // true
    }
}

#endif // TEST117

#ifdef TEST118

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

int main()
{
    // 合并两个按照 std::less{} 排序的序列
    {
        std::vector<int> vec1 { 1, 3, 5, 7 };
        std::vector<int> vec2 { 2, 4, 6, 8 };
        std::vector<int> vec3;

        // vec1 和 vec2 必须按照由小到大排序，如果不是有序则会直接中断
        // vec3 为 {1,2,3,4,5,6,7,8}
        auto it = std::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(vec3));
    }

    // 自定义比较函数对象
    {
        std::vector<int> vec1 { 7, 6, 5, 3, 1 };
        std::vector<int> vec2 { 8, 7, 6, 4, 2 };
        std::vector<int> vec3;

        // vec3 为 {8,7,7,6,6,5,4,3,2,1}
        auto it = std::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), std::back_inserter(vec3), std::greater {});
    }

    // 将两个相继的有序范围合并为一个有序范围
    {
        std::vector<int> vec1 { 1, 3, 5, 7, 2, 4, 6, 8 };

        // vec1 为 {1,2,3,4,5,6,7,8}
        std::inplace_merge(vec1.begin(), vec1.begin() + 4, vec1.end());
    }
}

#endif // TEST118

#ifdef TEST119

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    // 不保证维持相等元素的顺序
    {
        std::vector vec { 1, 3, 5, 7, 2, 4, 6, 8 };
        std::sort(vec.begin(), vec.end(), std::greater {});
    }

    // 保持相等的元素之间的顺序
    {
        std::vector vec { 1, 3, 5, 7, 2, 4, 6, 8 };
        std::stable_sort(vec.begin(), vec.end(), std::greater {});
    }

    // 重排范围中指定个数的元素，其余元素顺序未知，不保证相等元素之间的顺序
    {
        std::vector vec { 3, 7, 1, 8, 4, 5, 2, 6 };
        // 将vec中最大的4个元素排序放在vec的开始位置
        std::partial_sort(vec.begin(), vec.begin() + 4, vec.end(), std::greater {});
    }

    // 获取范围中第i个排序后的元素，第i大或第i小等等
    // vec不一定会被排序，只能保证指定位置是想要获取的值
    // MSVC2022 就会将输入范围完整排序，GCC13 只会将指定位置修改为要获取的值
    {
        std::vector vec { 3, 7, 1, 8, 4, 5, 2, 6 };
        std::nth_element(vec.begin(), vec.begin() + 3, vec.end());
        auto th_4 = *(vec.begin() + 3); // 第4小 4
        std::nth_element(vec.begin(), vec.begin() + 2, vec.end(), std::greater {});
        auto th_3 = *(vec.begin() + 2); // 第3大 6
    }
}

#endif // TEST119

#ifdef TEST201

#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <vector>

class Timer
{
public:
    Timer()
    {
        StartTime = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        auto useTime = std::chrono::high_resolution_clock::now() - StartTime;
        auto time    = std::chrono::duration_cast<std::chrono::milliseconds>(useTime);
        std::cout << "time consumed: " << time.count() << " ms\n";
    }

private:
    std::chrono::steady_clock::time_point StartTime {};
};

int main()
{
    std::vector<int> vec(10'000'000);
    std::generate(std::begin(vec), std::end(vec),
        []()
        {
            static int num { 0 };
            return num++;
        });

    // 不使用任何执行策略，c++17之前
    {
        Timer t;
        std::sort(std::begin(vec), std::end(vec));
    }

    // 顺序执行
    {
        Timer t;
        std::sort(std::execution::seq, std::begin(vec), std::end(vec));
    }

    // 乱序执行 c++20
    {
        Timer t;
        std::sort(std::execution::unseq, std::begin(vec), std::end(vec));
    }

    // 并行执行
    {
        Timer t;
        std::sort(std::execution::par, std::begin(vec), std::end(vec));
    }

    // 单个或多个线程上乱序执行
    {
        Timer t;
        std::sort(std::execution::par_unseq, std::begin(vec), std::end(vec));
    }

    return 0;
}

#endif // TEST201

#ifdef TEST202

#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <iostream>
#include <vector>

int main()
{
    // 并行策略必须保证不引发数据竞争，以下代码在多个线程中读取并修改count可能会引发问题
    {
        std::vector<int> vec(10);
        int count = 0;
        std::for_each(std::execution::par, std::begin(vec), std::end(vec), [&](int& x) { x = ++count; });
    }

    // 将共享数据原子化或互斥保护，但这样违背了并行策略的初衷
    {
        std::vector<int> vec(10);
        std::atomic_int count = 0;
        std::for_each(std::execution::par, std::begin(vec), std::end(vec), [&](int& x) { x = ++count; });
    }

    return 0;
}

#endif // TEST202

#ifdef TEST203

#include <algorithm>
#include <execution>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

int main()
{
    std::vector<int> vec1 { 1, 2, 3, 4, 5 };
    std::vector<int> vec2(5);

    std::vector<size_t> indices(vec1.size());
    // 如果std::iota操作的是自定义类型，需要对自定义类型重载前置++运算符
    // 可以自定义自增的大小
    std::iota(indices.begin(), indices.end(), 0); // 从 0 开始生成连续的递增int序列

    // 使用int序列索引std::vector
    std::for_each(std::execution::par, indices.cbegin(), indices.cend(), [vec1, &vec2](auto index) { vec2[index] = vec1[index] * vec1[index]; });

    for (const auto& elem : vec2)
    {
        std::cout << elem << ' ';
    }
}

#endif // TEST203

#ifdef TEST301

#include <format>
#include <iostream>
#include <numeric> // std::accumulate std::reduce
#include <string>
#include <vector>

int main()
{
    // std::accumulate 对范围内的元素做累加操作（顺序）
    // 可以设置累加、累减、累乘等以及自定义操作
    {
        std::vector<int> vec { 1, 2, 3, 4 };

        auto accumulate = std::accumulate(std::begin(vec), std::end(vec), 0);
        auto plus       = std::accumulate(std::begin(vec), std::end(vec), 0.0, std::plus<double>());
        auto minus      = std::accumulate(std::begin(vec), std::end(vec), 0.0, std::minus<double>());
        auto multiplies = std::accumulate(std::begin(vec), std::end(vec), 2.0, std::multiplies<double>());

        std::cout << std::format("accumulate: {}\tplus: {}\tminus: {}\tmultiplies: {}\n", accumulate, plus, minus, multiplies);

        // 自定义操作，初始值可以和范围内的元素类型不一致
        auto dash_fold  = [](std::string&& a, int b) { return std::move(a) + '-' + std::to_string(b); };
        std::string str = std::accumulate(std::next(vec.begin()), vec.end(), std::to_string(vec.front()), dash_fold);
        std::cout << std::format("dash fold: {}\n", str);
    }

    // std::reduce 对范围内的元素做累加操作（乱序）
    // 可以设置累加、累减、累乘等以及自定义操作
    {
        std::vector<int> vec { 1, 2, 3, 4 };

        auto reduce      = std::reduce(std::begin(vec), std::end(vec));
        auto reduce_init = std::reduce(std::begin(vec), std::end(vec), 2);
        auto plus        = std::reduce(std::begin(vec), std::end(vec), 0.0, std::plus<double>());
        auto minus       = std::reduce(std::begin(vec), std::end(vec), 0.0, std::minus<double>());
        auto multiplies  = std::reduce(std::begin(vec), std::end(vec), 2.0, std::multiplies<double>());

        std::cout << std::format("reduce: {}\treduce_init: {}\tplus: {}\tminus: {}\tmultiplies: {}\n", reduce, reduce_init, plus, minus, multiplies);
    }

    return 0;
}

#endif // TEST301

#ifdef TEST302

#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

int main()
{
    std::vector<int> vec { 1, 2, 3, 4, 5 };

    auto result = std::transform_reduce(
        std::execution::par, vec.cbegin(), vec.cend(), 0, [](auto a, auto b) { return a + b; }, [](auto val) { return 2 * val; });

    std::cout << result << '\n';
}

#endif // TEST302

#ifdef TEST401

#include <utility>
#include <vector>

int main()
{
    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2 { 9, 8, 7, 6 };

        std::exchange(vec1, { 6, 7, 8, 9 });
        std::exchange(vec1, { 8, 8, 8 });
        std::exchange(vec1, vec2); // vec1的值变为vec2的值，vec2的值并未改变

        // exchange(vec1,vec2)相当于下面两句
        vec1.clear();
        vec1.assign(vec2.cbegin(), vec2.cend());
    }

    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5 };
        std::vector<int> vec2 { 9, 8, 7, 6 };
        std::swap(vec1, vec2); // vec1的值变为原来vec2的值，vec2的值变为原来vec1的值
    }
}
#endif // TEST401

/*
* 1. set_difference set_symmetric_difference set_union set_intersection set_difference
* 2. std::transform std::for_each std::for_each
* 3. std::exchange
* 4. std::reduce
* 5. 
* 6. for_each transform中使用lambda
* 7. std::execution 并行算法
*/

// c++并行算法 openmp tbb mkl opencl => 06_02

#define TEST7

#ifdef TEST1

#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

// MSVC实现和这个肯定不一样，g++可能一样
template<class InputIt1, class InputIt2, class OutputIt, class Compare>
OutputIt my_set_difference(
    InputIt1 first1,
    InputIt1 last1,
    InputIt2 first2,
    InputIt2 last2,
    OutputIt d_first,
    Compare comp)
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
    std::set<int> set1{ 1,2,3 };
    std::set<int> set2{ 0,2,4 };
    std::set<int> set3{ 7,3,8 }; // set3 = {3,7,8}
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
            //std::cout << a << ',' << b << '\n';
            return a > 2;
        }
    );

    // 以下函数参数不一定非要std::set，只要容器元素满足升序都可以，例如：
    std::vector<int> v1{ 1,2,3 };
    std::vector<int> v2{ 0,2,4 };
    std::vector<int> v3;
    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3)); // v3 = {1,3}


    // 差集 将set2中有，set1中没有的元素，插入到set7开始位置
    std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::inserter(set7, set7.begin())); // set7 = {0,4}
    // 差集，set1中的元素在set2中没有的元素
    std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(set3, set3.end())); // set3 = 1,3,7,8; 1,3在set2中没有，std::inserter用法请看02_04_Iterator
    // 对称差集，除过两个集合中相同元素以外的所有元素加起来
    std::set_symmetric_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(set4, set4.end())); // set4 = 0,1,3,4; 除过2都有，其他所有元素加起来
    // 并集，两个集合的所有元素加起来
    std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(set5, set5.begin())); // set5 = 0,1,2,3,4;
    // 交集，两个集合都有的元素
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(set6, set6.begin())); // set6 = 2; 两个集合都有2


    //
    //std::set_new_handler();
    //std::set_terminate();
    //std::set_unexpected();

}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

int main()
{
    std::vector<std::string> v1{ "1","2","3","4" };
    std::vector<int> v2;
    v2.resize(v1.size());  // 必须给v2预分配大小
    std::vector<std::string> v3;
    v3.resize(v1.size());  // 必须给v3预分配大小

    std::transform(v1.begin(), v1.end(), v2.begin(), [](const std::string& str) {return std::stoi(str); });
    std::transform(v1.begin(), v1.end(), v3.begin(), [](const std::string& str) {return "123"; });


}

#endif // TEST2

#ifdef TEST3

#include <vector>
#include <utility>

int main()
{
    {
        std::vector<int> vec1{ 1,2,3,4,5 };
        std::vector<int> vec2{ 9,8,7,6 };

        std::exchange(vec1, { 6,7,8,9 });
        std::exchange(vec1, { 8,8,8 });
        std::exchange(vec1, vec2); // vec1的值变为vec2的值，vec2的值并未改变

        // exchange(vec1,vec2)相当于下面两句
        vec1.clear();
        vec1.assign(vec2.cbegin(),vec2.cend());
    }
    {
        std::vector<int> vec1{ 1,2,3,4,5 };
        std::vector<int> vec2{ 9,8,7,6 };
        std::swap(vec1, vec2); // vec1的值变为原来vec2的值，vec2的值变为原来vec1的值
    }

}
#endif // TEST3

#ifdef TEST6

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

int main()
{
    {
        std::vector<int> vec1{ 1,2,3,4,5 };

        auto it = vec1.begin();
        std::advance(it, 3);
        std::for_each(vec1.begin(), it, [](int& i) {i = i * 2; });
        std::for_each(vec1.begin(), vec1.end(), [](int& i) {return i * 2; });  //lambda表达式没有对vec1的元素进行修改，所以vec1没有变化
        std::for_each(vec1.begin(), vec1.end(), [](int i) {return ++i; });     //值捕获，不会改变vec1
        std::for_each(vec1.begin(), vec1.end(), [](int& i) {return ++i; });    //引用捕获，vec1 = {2,3,4}
        std::for_each(vec1.begin(), vec1.end(), [](int& i) {i = -i; });        //vec1 = {-2,-3,-4}
    }

    {
        std::vector<int> vec1{ 1,2,3,4,5 };
        std::vector<int> vec2;
        // 此处lambda的作用就是将vec1的每一个值乘2后赋值给vec2，对vec1并没有做任何修改
        std::for_each(vec1.begin(), vec1.end(),
            [&vec2](const int i) {
                vec2.emplace_back(i * 2);
            });
    }

    { // 以下代码和上面的lambda效果一模一样
        std::vector<int> vec1{ 1,2,3,4,5 };
        std::vector<int> vec2;
        for (auto elem : vec1)
            vec2.emplace_back(elem);
    }

    {
        std::vector<int> vec1{ 1,2,3,4,5 };
        std::vector<int> vec2;
        vec2.resize(vec1.size());
        std::transform(vec1.begin(), vec1.end(), vec2.begin(), [](int i) {return -i; });  //vec2 = {-1,-2,-3,-4,-5}，vec2必须提前分配好大小
        std::transform(vec1.begin(), vec1.end(), vec1.begin(), [](int i) {return -i; });  //vec1 = {-1,-2,-3,-4,-5}
        //std::transform(vec1.begin(), vec1.end(), vec2.begin(), [](int& i) {++i; });   //报错：“ = ”: 无法从“void”转换为“int”，所以此处的函数对象必须有返回值，用来输出到vec2
    }


    std::vector<std::string> v = { "111", "222", "333", "444", "555" };

    // return 类似于continue
    std::for_each(v.begin(), v.end(), [](const std::string& s) {
            if (s == "333")
            {
                // std::for_each中的 return 和循环中的 continue 效果一样
                //continue;
                return;
            }

            std::cout << s << std::endl;
        });

    std::cout << "--------\n";

    try
    {
        // throw类似于break
        std::for_each(v.begin(), v.end(), [](const std::string& s) {
                if (s == "333")
                {
                    throw("");
                }

                std::cout << s << std::endl;
            });
    }
    catch (...)
    {

    }

    return 0;
}

#endif // TEST6

#ifdef TEST7

#include <iostream>
#include <vector>
#include <execution>
#include <algorithm>
#include <chrono>
using namespace std;

int main(int argc, char* argv[])
{
    auto count = std::numeric_limits<int>::max() / (sizeof(int) * 40);
    cout << "count:" << count << endl;//13421772
    vector<int> inc(count);
    std::generate(inc.begin(), inc.end(), []() {static int i = 0; return i++; });

    std::vector<int> outc(count);
    //获取当前时间
    auto beforetime = std::chrono::system_clock::now();
    //①直接进行for循环  //6153ms
    /*for (size_t i = 0; i < inc.size(); i++)
        outc[i] = inc[i] * 10;*/
        //②并行算法 //49ms
        //transform(std::execution::par_unseq, inc.begin(), inc.end(), outc.begin(), [](auto i) {return i * 10; });
        //③串行执行 算法  257ms
        //transform(inc.begin(), inc.end(), outc.begin(), [](auto i) {return i * 10; });
        //④串行排序 //17818ms
        //sort(inc.begin(), inc.end());
        //⑤并行排序 //4773ms
    sort(std::execution::par, inc.begin(), inc.end());

    //获取结束时间
    auto aftertime = std::chrono::system_clock::now();

    //得到一个算法耗时
    cout << std::chrono::duration_cast<std::chrono::milliseconds>(aftertime - beforetime).count() << "ms" << endl;

    return 0;
}
#endif // TEST7


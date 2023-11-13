/*
 * 1. vector reserve() 初始化与否导致的巨大性能差异
 * 2. emplace_back push_back
 * 3. vector扩容机制
 * 4. vector转指针 指针转std::vector
 * 5. 扩容、裁剪
 */

#define TEST5

#ifdef TEST1

// https://mp.weixin.qq.com/s/umAhRgDRLTI2CGYEHsxAug

#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

int const num = 100000;

struct AdItem
{
    AdItem()
    {
    }

    AdItem(int x, int y, int z)
    {
        x_ = x;
        y_ = y;
        z_ = z;
    }

    int x_;
    int y_;
    int z_;
};

void time_report(const std::function<void()>& f1, const std::function<void()>& f2)
{
    auto start = std::chrono::high_resolution_clock::now();
    f1();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "allocation done in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    f2();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "assignment done in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}

void uninitialized_performance()
{
    std::vector<AdItem> v;
    time_report([&v]() { v.reserve(num); },
        [&v]()
        {
            for (int i = 0; i < num; ++i)
            {
                v[i] = { i + 2, i + 4, i + 6 };
            }
        });
}

void initialized_performance()
{
    std::vector<AdItem> v;
    time_report(
        [&v]() {
            v.assign(num, AdItem { 0, 0, 0 });
        },
        [&v]()
        {
            for (int i = 0; i < num; ++i)
            {
                v[i] = { i + 2, i + 4, i + 6 };
            }
        });
}

int main()
{
    // msvc没有resize不能直接使用[]赋值
    // uninitialized_performance();
    initialized_performance();
    return 0;
}
#endif // TEST1

#ifdef TEST2
#include <iostream>
#include <vector>

class Helper
{
public:
    Helper()
    {
        std::cout << "construct\n";
    }

    Helper(const Helper&)
    {
        std::cout << "copy construct\n";
    }

    Helper(Helper&&)
    {
        std::cout << "move construct\n";
    }

    Helper& operator=(const Helper&)
    {
        std::cout << "copy assignment\n";
        return *this;
    }

    Helper& operator=(Helper&&)
    {
        std::cout << "move assignment\n";
        return *this;
    }

    ~Helper()
    {
        std::cout << "destruct\n";
    }
};

// push_back 比 emplace_back 多一次拷贝构造

int main()
{
    Helper h1, h2;
    std::cout << "1----------------------\n";

    std::vector<Helper> vec;

    vec.emplace_back(h1);
    std::cout << "2----------------------\n";

    vec.push_back(h2);
    std::cout << "3----------------------\n";

    vec.emplace_back(Helper());
    std::cout << "4----------------------\n";

    vec.push_back(Helper());
}

#endif // TEST2

#ifdef TEST3

// vs使用1.5倍扩容机制，不足1舍去
// g++使用2倍扩容
#include <iostream>
#include <vector>

int main()
{
    size_t sz;
    std::vector<int> foo;
    // foo.reserve(100); // 先将vector底层空间扩增到100个，然后插入
    sz = foo.capacity();
    std::cout << "capacity:" << sz << std::endl;
    std::cout << "============\n";
    for (int i = 0; i < 100; ++i)
    {
        foo.emplace_back(i);
        if (sz != foo.capacity())
        {
            sz = foo.capacity();
            std::cout << "capacity changed: " << sz << '\n';
        }
    }
}
#endif // TEST3

#ifdef TEST4

#include <iostream>
#include <vector>

int main()
{
    // 指针转std::vector
    {
        int arr[] = { 0, 1, 2, 3, 4 };
        std::vector<int> vec;
        vec.reserve(5);
        vec.insert(vec.cbegin(), arr, arr + 5); // 第二三个参数是指针的起始和结束地址
        for (auto v : vec)
        {
            std::cout << v << ' ';
        }
        std::cout << std::endl;
    }

    // 指针转多层std::vector
    {
        int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        std::vector<int> vec2;
        vec2.insert(vec2.begin(), arr, arr + 3);

        std::vector<int> vec3;
        vec3.insert(vec3.begin(), arr + 3, arr + 6); // 将数组从第3个值到第5个值插入到vector => {4,5,6}

        std::vector<int> vec4;
        vec4.insert(vec4.begin(), arr + 6, arr + 9);

        std::vector<std::vector<int>> vec { vec2, vec3, vec4 };
    }

    // 多维std::vector转换为指针
    // std::vector内部的std::vector内存不是连续的
    {
        std::vector<std::vector<int>> vec { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 } };
        int* p = vec.front().data();

        size_t size { 0 };
        for (auto&& v : vec)
        {
            size += v.size();
        }

        int* intArray = new int[size]();

        size_t index = 0;
        for (const auto& innerVec : vec)
        {
            for (const int& value : innerVec)
            {
                intArray[index++] = value;
            }
        }

        for (size_t i = 0; i < size; ++i)
        {
            std::cout << intArray[i] << ' ';
        }
        std::cout << std::endl;

        delete[] intArray;
    }

    // 将指针插入std::vector<struct>
    {
        struct Point
        {
            double x;
            double y;
            double z;
        };

        double input[] { 1., 2., 3., 4., 5., 6., 7., 8., 9. };
        std::vector<Point> output;
        std::copy(reinterpret_cast<Point*>(input), reinterpret_cast<Point*>(input + 9), std::back_inserter(output));

        auto out = reinterpret_cast<double*>(output.data());

        for (const auto& elem : output)
        {
            std::cout << elem.x << ' ' << elem.y << ' ' << elem.z << '\n';
        }
        for (size_t i = 0; i < 9; ++i)
        {
            std::cout << out[i] << ' ';
        }
        std::cout << std::endl;
    }
}

#endif // TEST4

#ifdef TEST5

#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    {
        std::vector<float> vec;
        vec.resize(10); // vec共有10个元素，且都为0.f

        std::fill(vec.begin(), vec.end(), 3.f); // 将vec的值全部设置为3.f
        vec.resize(20);                         // 将vec的大小扩充至20，后10个元素的值为0.f，前10个仍为原值

        std::fill(vec.begin(), vec.end(), 4.f); // 将vec的值全部设置为4.f
        vec.resize(10);                         // 将vec的大小缩小为10，这10个元素的值为原值
    }

    {
        std::vector<int> vec1 { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        // 构造新的std::vector时传入需要截取的范围，即可截取输入std::vectotr的指定内容
        std::vector vec3(vec1.cbegin(), vec1.cbegin() + 2);     // 12
        std::vector vec4(vec1.cbegin() + 2, vec1.cbegin() + 5); // 345
        std::vector vec5(vec1.cbegin() + 5, vec1.cend());       // 6789
    }
}

#endif // TEST5
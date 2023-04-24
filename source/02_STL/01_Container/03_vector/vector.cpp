/*
* 1. vector reserve() 初始化与否导致的巨大性能差异
* 2. emplace_back push_back
* 3. vector扩容机制
* 4. vector转指针 指针转vector
* 5. vector扩容时调用拷贝构造和移动构造，使用reserve预分配空间
*/

#define TEST4

#ifdef TEST1

// https://mp.weixin.qq.com/s/umAhRgDRLTI2CGYEHsxAug

#include <chrono>
#include <functional>
#include <iostream>
#include <vector>

int const num = 100000;
struct AdItem
{
    AdItem() {}

    AdItem(int x, int y, int z)
    {
        x_ = x; y_ = y; z_ = z;
    }

    int x_;
    int y_;
    int z_;
};


void time_report(const std::function<void()>& f1, const std::function<void()>& f2) {
    auto start = std::chrono::high_resolution_clock::now();
    f1();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "allocation done in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    f2();
    end = std::chrono::high_resolution_clock::now();
    std::cout << "assignment done in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}

void uninitialized_performance() {
    std::vector<AdItem> v;
    time_report([&v]() { v.reserve(num); },
        [&v]() {
            for (int i = 0; i < num; ++i) {
                v[i] = { i + 2, i + 4, i + 6 };
            } });
}

void initialized_performance() {
    std::vector<AdItem> v;
    time_report([&v]() { v.assign(num, AdItem{ 0, 0, 0 }); },
        [&v]() {
            for (int i = 0; i < num; ++i) {
                v[i] = { i + 2, i + 4, i + 6 };
            } });

}

int main()
{
    // msvc没有resize不能直接使用[]赋值
    //uninitialized_performance();
    initialized_performance();
    return 0;
}
#endif // TEST1

#ifdef TEST2
#include <iostream>
#include <vector>

class A
{
public:
    A() { std::cout << "construct\n"; }
};

int main()
{
    A a1, a2, a3, a4;
    std::cout << "===\n";
    std::vector<A> vec;
    vec.emplace_back(a1);
    vec.push_back(a2);
    vec.emplace_back(a3);
    vec.push_back(a4);
    vec.emplace_back(A());
    vec.push_back(A());
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
#include <sstream>

int main()
{
    // 指针转std::vector
    {
        int arr1[] = { 1,2,3,4,5 };
        std::vector<int> vec2;
        //vec2.reserve(5);
        vec2.insert(vec2.begin(), arr1, arr1 + 5);
        int* p = vec2.data();
    }

    // 指针转多层std::vector
    {
        float arr1[] = { 1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9 };
        std::vector<std::vector<float>> vec1;

        std::vector<float> vec2;
        vec2.insert(vec2.begin(), arr1, arr1 + 3);

        std::vector<float> vec3;
        vec3.insert(vec3.begin(), arr1+3, arr1 + 6);

        std::vector<float> vec4;
        vec4.insert(vec4.begin(), arr1+6, arr1 + 9);

        vec1 = { vec2,vec3,vec4 };

    }

}

#endif // TEST4

#ifdef TEST5

#include <iostream>
#include <vector>

class Test
{
public:
    Test() { std::cout << " *** construct\n"; }
    ~Test() { std::cout << " --- destruct\n"; }

    Test& operator=(const Test&)noexcept { std::cout << " 222 operator=\n"; return *this; }
    Test& operator=(const Test&&)noexcept { std::cout << " 888 operator=\n"; return *this; }
    Test& operator=(Test&&)noexcept { std::cout << " 333 operator=\n"; return *this; }
    Test& operator=(Test&)noexcept { std::cout << " 999 operator=\n"; return *this; }

    Test(const Test&) { std::cout << " 444 copy construct\n"; }
    Test(const Test&&)noexcept { std::cout << " 555 move construct\n"; }
    Test(Test&&)noexcept { std::cout << " 666 move construct\n"; }
    Test(Test&)noexcept { std::cout << " 777 copy construct\n"; }
};

int main()
{
    {
        // 20次析构
        Test t1, t2, t3, t4, t5;
        std::vector<Test> vec;
        vec.emplace_back(t1);
        vec.emplace_back(t2);
        vec.emplace_back(t3);
        vec.emplace_back(t4);
        vec.emplace_back(t5);
    }
    std::cout << "1111111111111111111111111111111111\n";
    {
        // 10次析构
        // 这种消耗最少
        Test t1, t2, t3, t4, t5;
        std::vector<Test> vec;
        vec.reserve(5);
        std::cout << "******** " << vec.size() << '\t' << vec.capacity()<< '\n'; // 0,5
        vec.emplace_back(t1);
        vec.emplace_back(t2);
        vec.emplace_back(t3);
        vec.emplace_back(t4);
        vec.emplace_back(t5);
    }
    std::cout << "22222222222222222222222222222222222\n";
    {
        // 27次析构
        Test t1, t2, t3, t4, t5;
        std::vector<Test> vec;
        vec.resize(5);
        std::cout << "******** " << vec.size() << '\t' << vec.capacity() << '\n'; //5,5
        vec.emplace_back(t1);
        vec.emplace_back(t2);
        vec.emplace_back(t3);
        vec.emplace_back(t4);
        vec.emplace_back(t5);
    }
    std::cout << "333333333333333333333333333333333333\n";
    {
        // 27次析构
        Test t1, t2, t3, t4, t5;
        std::vector<Test> vec;
        vec.reserve(5);
        vec.resize(5);
        std::cout << "******** " << vec.size() << '\t' << vec.capacity() << '\n'; //5,5
        vec.emplace_back(t1);
        vec.emplace_back(t2);
        vec.emplace_back(t3);
        vec.emplace_back(t4);
        vec.emplace_back(t5);
    }
    std::cout << "44444444444444444444444444444444444444\n";
    {
        // 27次析构
        Test t1, t2, t3, t4, t5;
        std::vector<Test> vec;
        vec.resize(5);
        vec.reserve(5);
        std::cout << "******** " << vec.size() << '\t' << vec.capacity() << '\n'; //5,5
        vec.emplace_back(t1);
        vec.emplace_back(t2);
        vec.emplace_back(t3);
        vec.emplace_back(t4);
        vec.emplace_back(t5);
    }
}

#endif // TEST5

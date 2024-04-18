#include <array>
#include <iostream>

int main()
{
    {
        std::array<int, 2> arr {};
        auto ptr = arr.data();
        ptr[0]   = 2; // 修改 std::array 的数据
        ptr[1]   = 3;
    }

    // 不能使用运行期变量定义 std::array
    // 可以使用 const constexpr 变量
    {
        const size_t size { 2 };
        std::array<int, size> arr {};
    }

    {
        constexpr size_t size { 3 };
        std::array<int, size> arr {};
    }

    // sizeof(std::array) 返回的是实际数据所占内存的字节个数
    // std::array 内部只有一个变量 T data[size];
    {
        std::array<int, 3> arr1 {};
        std::array<char, 5> arr2 {};
        std::array<double, 3> arr3 {};

        auto s1 = sizeof(arr1); // 12
        auto s2 = sizeof(arr2); // 5
        auto s3 = sizeof(arr3); // 24
    }
}

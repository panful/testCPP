/*
 * 1. std::move std::forward都是仅仅执行强制型别转换的函数
 * 2. 不要对常量使用std::move
 * 3. 对标准库容器使用std::move
 * 4. std::move_if_noexcept
 * 深入理解std::move https://mp.weixin.qq.com/s/GYn7g073itjFVg0OupWbVw
 */

#define TEST3

#ifdef TEST1

#include <iostream>

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

void Func1(Helper&&)
{
    std::cout << "Helper&&\n";
}

void Func2(const Helper&)
{
    std::cout << "const Helper&\n";
}

// 自定义一个std::move
template <typename T>
constexpr std::remove_reference_t<T>&& MyMove(T&& t) noexcept
{
    std::cout << " --- my move ---\n";
    return static_cast<std::remove_reference_t<T>&&>(t);
}

int main()
{
    std::cout << "-------------------------------------------------------\n";
    // std::move和std::forward其实就是使用static_cast做强制类型转换
    // 在运行期不做任何操作
    {
        Helper h1;

        Func1(std::move(h1));
        Func1(std::forward<Helper&&>(h1));
        Func1(static_cast<Helper&&>(h1));

        Func2(h1);
        Func2(std::forward<Helper&>(h1));
        Func2(static_cast<Helper&>(h1));
    }
    std::cout << "-------------------------------------------------------\n";
    {
        Helper h;

        Func1(std::move(static_cast<Helper>(h))); // static_cast<Helper>(h)调用拷贝构造，std::move执行完后立即析构
        Func1(std::move(static_cast<Helper&>(h)));
        Func1(std::move(static_cast<Helper&&>(h)));

        std::cout << "++++++++++\n";

        Func1(MyMove(static_cast<Helper>(h)));
        Func1(MyMove(static_cast<Helper&>(h)));
        Func1(MyMove(static_cast<Helper&&>(h)));
    }
    std::cout << "-------------------------------------------------------\n";
    {
        Helper h;

        Func1(std::forward<Helper&&>(static_cast<Helper>(h)));
        Func1(std::forward<Helper&&>(static_cast<Helper&>(h)));
        Func1(std::forward<Helper&&>(static_cast<Helper&&>(h)));

        // Func2(std::forward<Helper&>(static_cast<Helper>(h)));   // error 不能将右值转换为左值
        Func2(std::forward<Helper&>(static_cast<Helper&>(h)));
        // Func2(std::forward<Helper&>(static_cast<Helper&&>(h))); // error 同上
    }
    std::cout << "-------------------------------------------------------\n";
    {
        Helper h1;
        auto h2 = std::move(h1);              // move
        auto h3 = std::forward<Helper&&>(h1); // move
        auto h4 = std::forward<Helper&>(h1);  // copy
        auto h5 = std::forward<Helper>(h1);   // move
    }
    std::cout << "-------------------------------------------------------\n";

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

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

class Test
{
public:
    Test(const Helper& h) : m_helper(std::move(h))
    {
    }

private:
    Helper m_helper;
};

int main()
{
    std::cout << "-------------------------------------------------------\n";
    {
        const Helper h {};
        std::cout << "-----------------\n";

        // 将h传递给实参调用1次拷贝构造
        // 使用std::move(h)将h赋值给m_helper调用用一次拷贝构造
        //
        // 虽然使用了std::move但是并没有调用Helper的移动构造
        // 因为std::move返回的是一个const Helper&&，不能传递给Helper的移动构造函数
        // 移动构造函数只能接收非const型别的右值
        Test t(h);
    }
    std::cout << "-------------------------------------------------------\n";
    {
        const Helper h {};
        auto h2 = std::move(h); // 复制构造函数，MSVC此处还会有警告
    }
    std::cout << "-------------------------------------------------------\n";
}

#endif // TEST2

#ifdef TEST3

#include <array>
#include <iostream>
#include <map>
#include <vector>

void* operator new(size_t n)
{
    std::cout << "new size: " << n << '\n';
    return malloc(n);
}

void operator delete(void* p)
{
    std::cout << "delete\n";
    return free(p);
}

// 标准库的容器使用移动构造时，会将实参的数据全部绑定到新的对象上，即实参数据将会清空

int main()
{
    std::cout << "-------------------------------------------------------\n";
    {
        std::vector<int> vec { 1, 2, 3, 4 };
        std::vector<int> vec2(std::move(vec));
        std::cout << vec.data() << '\t' << vec.size() << '\t' << vec.capacity() << '\n';
        std::cout << vec2.data() << '\t' << vec2.size() << '\t' << vec2.capacity() << '\n';
    }
    std::cout << "-------------------------------------------------------\n";
    {
        std::array<int, 3> arr { 1, 2, 3 };
        std::array<int, 3> arr2(std::move(arr));
        std::cout << arr.data() << '\t' << arr.size() << '\n';
        std::cout << arr2.data() << '\t' << arr2.size() << '\n';
    }
    std::cout << "-------------------------------------------------------\n";
    {
        std::map<int, char> m { { 1, 'a' }, { 2, 'b' }, { 3, 'c' } };
        std::map<int, char> m2(std::move(m));
        std::cout << m.size() << '\n';
        std::cout << m2.size() << '\n';
    }
}

#endif // TEST3

#ifdef TEST4

#endif // TEST4
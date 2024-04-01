
/*
* 7. 四种类型转换
*/

#define TEST7

#ifdef TEST7

// 00_07_TEST8 dynamic_cast

#include <iostream>

class A {};
class B :public A {};

void testFunc1() {}
int testFunc2(int) { return 1; }

#define PRINT_TYPE(x) std::cout<<#x<<'\t'<<typeid(x).name()<<'\n';

int main()
{
    // static_cast
    {
        int n = 0, * pn = &n;
        //auto r1 = static_cast<float*>(pn);    // error
        auto r2 = static_cast<float>(n);      // ok r2类型为float
        auto r3 = static_cast<int*>(nullptr); // ok r3类型为int*，且r3 = nullptr
        //auto r4 = static_cast<float*>(pn);    // error 不能转换空指针以外的指针类型

        A* pA = new A();
        B* pB = new B();
        auto r5 = static_cast<B*>(pA);  // ok 父类转子类，不安全，不建议这样做
        auto r6 = static_cast<A*>(pB);  // ok 子类转父类，安全
        auto r7 = static_cast<void*>(nullptr); // ok
    }

    // const_cast
    {
        const int cn = 0, * pcn = &cn, & rcn = cn;
        int n = 0, * pn = &n, & rn = n;
        //auto r1 = const_cast<int>(cn);      // error <>中的类型只能是指针或引用

        auto r2 = const_cast<int*>(pcn);       // ok 删除const限定
        auto& r3 = const_cast<int&>(rcn);      // ok r3类型为 int&
        auto r4 = const_cast<const int*>(pn);  // ok 添加const限定
        auto& r5 = const_cast<const int&>(n);  // ok r5类型为 const int&

        // typeid无法判断非指针数据为const https://bbs.csdn.net/topics/190122047

        PRINT_TYPE(cn);
        PRINT_TYPE(pcn);
        PRINT_TYPE(rcn);
        PRINT_TYPE(n);
        PRINT_TYPE(pn);
        PRINT_TYPE(rn);

        PRINT_TYPE(r2);
        PRINT_TYPE(r3);
        PRINT_TYPE(r4);
        PRINT_TYPE(r5);
    }

    std::cout << "---------------\n";

    {
        const int a = 10;
        const int* p = &a;
        int* q = const_cast<int*>(p);
        *q = 20;    //fine  这句代码是一个未定义行为(UB)

        // 指针p和指针q都是指向a变量的，指向地址相同，但是值却不一样
        // 常量折叠 可以看00_10_TEST4
        std::cout << "a=" << a << " " << "&a = " << &a << std::endl;
        std::cout << "*p=" << *p << " " << "p = " << p << std::endl;
        std::cout << "*q=" << *q << " " << "q = " << q << std::endl;
    }

    std::cout << "---------------\n";

    {
        int c = 11;
        const int a = c;
        const int* p = &a;
        int* q = const_cast<int*>(p);
        *q = 20;    //fine

        std::cout << "a=" << a << " " << "&a = " << &a << std::endl;
        std::cout << "*p=" << *p << " " << "p = " << p << std::endl;
        std::cout << "*q=" << *q << " " << "q = " << q << std::endl;
    }

    std::cout << "---------------\n";

    {
        const int c = 11;
        const int a = c;
        const int* p = &a;
        int* q = const_cast<int*>(p);
        *q = 20;    //fine

        std::cout << "a=" << a << " " << "&a = " << &a << std::endl;
        std::cout << "*p=" << *p << " " << "p = " << p << std::endl;
        std::cout << "*q=" << *q << " " << "q = " << q << std::endl;
    }

    // reinterpret_cast
    {
        int n = 666, * p = &n, & r = n;
        auto r1 = reinterpret_cast<float*>(p); // ok r1的类型为float*
        auto ret1 = *r1; // ret1是一个非法值
        auto r2 = reinterpret_cast<float&>(r); // ok r2的类型为float不是float&
        //auto r3 = reinterpret_cast<float>(n);  // error
        //auto r4 = reinterpret_cast<float>(r);  // error
        auto r5 = reinterpret_cast<float*>(&r);  // ok
        // auto ret2 = *r5; // error 运行时报错
        auto r6 = reinterpret_cast<float&>(n);  // ok

        // 函数指针转换
        typedef void (*FUNC)();
        auto r7 = reinterpret_cast<FUNC>(testFunc1);
        auto r8 = reinterpret_cast<FUNC>(testFunc2);
        r7(); // ok
        r8(); // ok

        //auto r9 = reinterpret_cast<int*>(nullptr); // error，空指针转换应该用static_cast
        float* pf = nullptr;
        auto r10 = reinterpret_cast<int*>(pf); //ok

        // 一定要谨慎使用reinterpret_cast 
        // 例如{auto r5 = reinterpret_cast<float*>(r);}不应该被转换，但是编译器不会报错
        // {auto r8 = reinterpret_cast<FUNC>(testFunc2);}不应该被转换，函数签名不一样
    }

    // dynamic_cast 00_07_02_TEST8
    {
        A* pA = new A();
        B* pB = new B();

        auto r1 = dynamic_cast<A*>(pB); // 子类向父类转换，安全
        //dynamic_cast<B*>(pA); // 因为类A中没有虚函数，所以编译失败
    }

}

#endif // TEST7
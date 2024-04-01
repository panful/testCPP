
#include "new.h"
#include "object.h"
#include "smartpointer.h"
#include <iostream>

class Helper : public Object
{
public:
    // 必须重写此静态函数
    static Helper* New()
    {
        return new Helper();
    }

protected:
    Helper()
    {
        std::cout << "construct\n";
    }

    ~Helper() override
    {
        std::cout << "destruct\n";
    }

private:
    Helper(const Helper&)            = delete;
    Helper& operator=(const Helper&) = delete;
};

// 测试自定义智能指针
int main()
{
    {
        New<Helper> h;
        std::cout << h->GetReferenceCount() << std::endl;
    }

    std::cout << "-------------" << std::endl;

    {
        SmartPointer<Helper> h;
        if (!h)
        {
            h = SmartPointer<Helper>::New();
        }
        auto h2 = h;
        std::cout << h2->GetReferenceCount() << std::endl;
    }

    std::cout << "-------------" << std::endl;

    {
        New<Helper> h;
        SmartPointer<Helper> h2 = h;
        std::cout << h2->GetReferenceCount() << std::endl;
    }

    std::cout << "-------------" << std::endl;

    {
        SmartPointer<Helper> h = SmartPointer<Helper>::New();
        SmartPointer<Helper> h2(h);
        SmartPointer<Helper> h3(h.Get());
        SmartPointer<Helper> h4(h.GetPointer());
        std::cout << h->GetReferenceCount() << std::endl;
    }
}

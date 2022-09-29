
/*
* 1. 虚函数声明为inline
*/

#define TEST1

#ifdef TEST1

// 虚函数被声明为inline时，是否真正被inline了
// https://www.cnblogs.com/helloweworld/archive/2013/06/03/3116069.html

class A
{
public:
    inline virtual void Func() {}
};

int main()
{

}
#endif // TEST1
/*
 * 1.
 * 2. deque 双端队列 double ended queue
 * 3. priority_queue 优先级队列
 * 4. stack 栈
 * 5. queue 单端队列
 */

#define TEST5

#ifdef TEST2

#include <deque>
#include <list>

int main()
{
    std::deque<int> d1 { 1, 2, 3, 4, 5 };           // d1 = 1,2,3,4,5
    std::deque<int> d2(3, 5);                       // d2 = 5,5,5
    std::deque<int> d3(d1.begin(), d1.begin() + 2); // d3 = 1,2

    d2.emplace(d2.begin() + 1, 2);                  // 开始的下一个位置添加一个2：5,2,5,5
    d2.emplace_back(3);                             // 末尾添加一个3：5,2,5,5,3
    d2.emplace_front(4);                            // 首部添加一个4：4,5,2,5,5,3

    d2.push_back(6);                                // 末尾添加一个6：{4,5,2,5,5,3,6}
    d2.insert(d2.end(), 1);                         // 末尾添加一个1：{4,5,2,5,5,3,6,1}
    d2.insert(d2.begin(), { 7, 8, 9 });             // 首部添加一组7,8,9:{7,8,9,4,5,2,5,5,3,6,1}

    auto ret1 = d2.front();
    auto ret2 = d2.back();
    auto ret3 = *d2.begin();
    auto ret4 = d2[5];    // 从0开始第5个
    auto ret5 = d2.at(2); // 从0开始第2个

    // auto r1 = d2.at(100); //崩溃
    // auto r2 = d2[100];    //中断

    d2.pop_back();                        // 移除最后一个元素
    d2.pop_front();                       // 移除第一个元素
    d2.erase(d2.begin());                 // 移除指定位置的一个元素（移除第一个元素）
    d2.erase(d2.begin(), d2.begin() + 2); // 移除指定范围的n个元素（移除前两个元素）
    d2.clear();                           // 清空元素

    int test = 0;
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <map>
#include <queue>

class A
{
public:
    A(int x)
    {
        a = x;
    }

    int a;

    bool operator<(const A& aa) const // 后置const必须有
    {
        return aa.a < this->a;
    }
};

int main()
{
    std::priority_queue<A> p3;

    p3.emplace(A(1));
    p3.emplace(A(5));
    p3.emplace(A(2));
    p3.emplace(A(9));
    p3.emplace(A(4));
    p3.emplace(A(3));

    while (!p3.empty())
    {
        std::cout << p3.top().a << "\n";
        p3.pop();
    }

    std::cout << "========\n";

    std::priority_queue<std::pair<int, int>> p2; // 按pair的first排序

    p2.emplace(1, 1);
    p2.emplace(3, 3);
    p2.emplace(7, 7);
    p2.emplace(8, 2);
    p2.emplace(5, 9);

    while (!p2.empty())
    {
        std::cout << p2.top().first << " " << p2.top().second << "\n";
        p2.pop();
    }

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <vector>

int main()
{
    // std::stack 包含一个【容器】成员变量，实际的元素就是存储在这个容器中，默认容器是 std::deque
    // std::stack 往往被称为：容器适配器(container adapter)
    // 只能先进后出(First In Last Out, FILO)，除了最顶端元素，没有任何方法可以存取其他元素，不允许有遍历行为
    {
        std::stack<int> stack {};
        stack.emplace(1);       // 将1插入到栈顶，实际调用的是成员容器的 emplace_back()
        stack.push(2);          // 将2插入到栈顶，实际调用的是成员容器的 push_back()
        auto top = stack.top(); // 获取栈顶元素（2），并不弹出，实际调用的是成员容器的 back()
        stack.pop();            // 弹出栈顶元素（2），实际调用的是成员容器的 pop_back
    }

    {
        std::stack<int, std::vector<int>> vecStack {}; // vector的类型和stack的第一个类型必须一样，否则编译失败
        std::stack<int, std::list<int>> listStack {};
        std::stack<int, std::queue<int>> queueStack {};
        std::stack<int, std::stack<int>> statckStack {};

        std::vector<int> vec { 1, 2, 3, 4, 5 };
        std::stack stack(vec); // 栈顶是5，栈底是1
    }

    {
        std::stack<int> stack1({ 1, 2, 3, 4 }); // ok 相当于先使用{}构造 std::deque，再使用 std::deque 构造 std::stack
        // std::stack<int> stack2{1, 2, 3, 4};  // error
        // std::stack<int> stack3(1, 2, 3, 4);  // error
    }

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <deque>
#include <list>
#include <queue>
#include <vector>

int main()
{
    // std::queue 包含一个【容器】成员变量，实际的元素就是存储在这个容器中，默认容器是 std::deque
    // std::queue 往往被称为：容器适配器(container adapter)
    // 只能先进先出(First In First Out, FIFO)，除了最顶端取出、最底端插入外，没有任何方法可以存取其他元素，不允许有遍历行为
    {
        std::queue<int> queue {};
        queue.push(1);              // 队尾插入一个元素
        queue.emplace(2);           // 队尾插入一个元素
        auto back  = queue.back();  // 2  返回队尾元素，不弹出
        auto front = queue.front(); // 1  返回队首元素，不弹出
        queue.pop();                // 抛出队首元素
    }

    // 其他行为和 std::stack 类似，它们都是容器适配器
    {
        std::queue<int, std::vector<int>> vecQueue {};
        std::queue<int, std::list<int>> listQueue {};

        std::vector vec { 1, 2, 3, 4, 5 };
        std::queue queue(vec); // 队尾是5，队首是1
    }

    return 0;
}
#endif // TEST5

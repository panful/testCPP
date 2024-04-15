/**
 * 1. std::list
 * 2. std::forward_list 没有size()函数
 */

// https://www.cnblogs.com/mmmmmmmmm/p/14812567.html

#define TEST2

#ifdef TEST1

#include <algorithm>
#include <iostream>
#include <list>

int main()
{
    // 插入
    std::list<int> List1;
    List1.insert(List1.begin(), { 1, 2 });    // 1,2
    List1.emplace(List1.end(), std::move(3)); // 1,2,3
    List1.emplace_back(4);                    // 1,2,3,4
    List1.emplace_front(5);                   // 5,1,2,3,4
    List1.push_back(6);                       // 5,1,2,3,4,6
    List1.push_front(7);                      // 7,5,1,2,3,4,6

    // 删除
    std::list<int> List2 { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    List2.pop_back();                                                   // 0,1,2,3,4,5,6,7
    List2.pop_front();                                                  // 1,2,3,4,5,6,7
    List2.erase(List2.begin());                                         // 2,3,4,5,6,7
    List2.erase(std::find(List2.begin(), List2.end(), 5), List2.end()); // 2,3,4
    List2.remove(2);                                                    // 3,4，返回值是移出的元素个数，没有移出则返回0
    List2.remove_if([](int n) { return n == 4; });                      // 3
    List2.clear();                                                      // 清空

    // 查找
    std::list<int> List3 { 1, 2, 3, 4, 5 };
    int front                   = List3.front(); // 1
    int back                    = List3.back();  // 5
    std::list<int>::iterator it = std::find(List3.begin(), List3.end(), 2);

    // 修改
    std::list<int> List4 { 2, 3, 4 };
    std::list<int> List5 { 7, 8, 9 };
    List4.swap(List5);

    std::list<int> List6 { 1, 2, 3, 4, 5, 6 };
    List5.remove(3);
    List6.emplace_back(7);

    // 归并
    std::list<int> List7 { 8, 7, 7, 9 };
    List7.sort();
    std::list<int> List8 { 5, 3, 3, 2 };
    List8.sort();
    List7.merge(List8);

    // 排序
    List7.sort(std::greater<int>()); // 降序
    List7.sort(std::less<int>());    // 升序

    // 移除相邻重复元素
    List7.unique();

    // 逆序
    List7.reverse();

    // 转移元素
    std::list<int> List9 { 1, 2, 3, 4, 5 };
    std::list<int> List10 { 7, 8, 9 };
    auto it9 = List9.begin();
    std::advance(it9, 2);      // 迭代器指向的元素向后移动两位，指向3
    List9.splice(it9, List10); // List9:1,2,7,8,9,3,4,5;List10为空

    // 转移元素
    std::list<int> List11 { 1, 2, 3 };
    std::list<int> List12 { 5, 6, 7, 8, 9 };
    auto it12 = List12.begin();
    std::advance(it12, 2);
    List11.splice(List11.begin(), List12, it12, List12.end()); // List11:7,8,9,1,2,3;Lis12:5,6

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <forward_list>
#include <iostream>

int main()
{
    // 在元素头部原位构造元素
    {
        std::forward_list<int> fl;
        fl.emplace_front(1);
        fl.emplace_front(2);
        fl.emplace_front(3);
        for (auto const elem : fl)
        {
            std::cout << elem << ' '; // 3,2,1 注意此处不是 1,2,3
        }
    }

    // 在指定位置后边原位构造元素
    {
        std::forward_list<int> fl;
        fl.emplace_after(fl.cbefore_begin(), 1);
        fl.emplace_after(fl.cbefore_begin(), 2);
        fl.emplace_after(fl.cbefore_begin(), 3);

        for (auto const elem : fl)
        {
            std::cout << elem << ' '; // 3,2,1
        }
    }
}

#endif // TEST2

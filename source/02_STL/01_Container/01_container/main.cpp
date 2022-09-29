/*
* 1. list vector set 互相转换
*/

#define TEST1

#ifdef TEST1

// 指针转vector请看 03_vector:TEST4（指针转list,set等也是类似做法）

#include <set>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <unordered_map>

int main()
{
    // list 转 vector/set
    {
        std::list<int> List{ 1,2,3,4,5,5,0 };
        std::vector<int> Vec;
        std::set<int> Set;
        Set.insert(List.begin(), List.end()); //去重且排序 0,1,2,3,4,5
        Vec.insert(Vec.begin(), List.begin(), List.end()); //大小顺序不变 1,2,3,4,5,5,0
    }

    // vector 转 list/set
    {
        std::vector<int> Vec{ 1,2,3,4,5,5,0 };
        std::list<int> List;
        std::set<int> Set;
        List.insert(List.begin(), Vec.begin(), Vec.end()); //大小顺序不变 1,2,3,4,5,5,0
        Set.insert(Vec.begin(), Vec.end()); // 去重且排序 0,1,2,3,4,5
    }

    // set 转 vector/list
    {
        std::set<int> Set{ 1,2,3,4,5,5,0 }; // 0,1,2,3,4,5
        std::list<int> List;
        std::vector<int> Vec;
        List.insert(List.begin(), Set.begin(), Set.end()); //0,1,2,3,4,5
        Vec.insert(Vec.begin(), Set.begin(), Set.end());   //0,1,2,3,4,5
    }


}

#endif // TEST1

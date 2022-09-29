/*
* 1. std::forward_list没有size()函数
* 2. 自定义链表
*/

#define TEST2

#ifdef TEST1

#include <list>
#include <forward_list>
#include <iostream>

int main()
{
    std::list<int> l1;
    std::forward_list<int> l2; // 没有size()
    l1.size();
    
}

#endif // TEST1

#ifdef TEST2

#include <unordered_map>
#include <vector>
#include <map>
using namespace std;

struct ListNode {
    int val{ 0 };
    ListNode* next{ nullptr };
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
    ListNode(initializer_list<int> list)
    {
        ListNode* node = nullptr;
        auto it = list.begin();
        while (it != list.end())
        {
            if (!next)
            {
                val = *it++;
                if (it != list.end())
                    next = node = new ListNode(*it++);
            }
            else
            {
                node->next = new ListNode(*it++);
                node = node->next;
            }
        }
    }
};

int main()
{
    ListNode* l1 = new ListNode({ 9,9,9,9,9,9,9 });
    ListNode* l2 = new ListNode({ 9,9,9,9 });
}

#endif // TEST2
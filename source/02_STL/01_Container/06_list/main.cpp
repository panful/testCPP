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
    {
        std::list<int> myList;
        auto nSize = myList.size();
        auto isEmpty = myList.empty();
    }

    {
        std::forward_list<int> myList;
        //myList.size(); // 没有size()函数
        auto isEmpty = myList.empty();
    }

    {
        std::list<int> myList1{ 0,1,2,3 };
        std::list<int> myList2{ 4,5,6,7,8 };

        myList1.splice(myList1.begin(), myList2);
    }
}

#endif // TEST1

#ifdef TEST2

#include <list>
#include <initializer_list>
#include <iostream>

struct ListNode 
{
    int val{ 0 };

    ListNode* next{ nullptr };

    ListNode() : val(0), next(nullptr) {}

    ListNode(int x) : val(x), next(nullptr) {}

    ListNode(int x, ListNode* next) : val(x), next(next) {}

    ListNode(std::initializer_list<int> list)
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

#define PRINT_LIST(list_node) 


int main()
{
    {
        std::list<int> l1;
    }

    {
        ListNode* l1 = new ListNode({ 1,2,3,4,5 });
        while (auto next = l1->next)
        {
            std::cout << next->val << '\n';
            l1->next = l1->next->next;
        }
        ListNode l2{ 1 };
        ListNode l3{ 1,2,3,4 };
        ListNode l4{ 1,l1 };
    }


    return 0;
}

#endif // TEST2
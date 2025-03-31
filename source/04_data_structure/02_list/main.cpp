/**
 * 1. 单向链表
 * 2. 翻转链表
 */

#define TEST2

#ifdef TEST1

#include <iostream>

// 定义链表节点结构
template <typename T>
struct Node
{
    T data;        // 节点存储的数据
    Node<T>* next; // 指向下一个节点的指针

    // 构造函数
    Node(T val)
        : data(val)
        , next(nullptr)
    {
    }
};

// 定义链表类
template <typename T>
class LinkedList
{
private:
    Node<T>* head; // 链表头指针

public:
    // 构造函数
    LinkedList()
        : head(nullptr)
    {
    }

    // 析构函数
    ~LinkedList()
    {
        Node<T>* current = head;
        Node<T>* nextNode;
        while (current != nullptr)
        {
            nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    // 在链表末尾添加节点
    void append(T val)
    {
        Node<T>* newNode = new Node<T>(val);
        if (head == nullptr)
        {
            head = newNode;
        }
        else
        {
            Node<T>* temp = head;
            while (temp->next != nullptr)
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    // 在链表头部添加节点
    void prepend(T val)
    {
        Node<T>* newNode = new Node<T>(val);
        newNode->next    = head;
        head             = newNode;
    }

    // 删除链表中第一个值为val的节点
    void remove(T val)
    {
        if (head == nullptr)
            return;

        // 如果要删除的是头节点
        if (head->data == val)
        {
            Node<T>* temp = head;
            head          = head->next;
            delete temp;
            return;
        }

        Node<T>* temp = head;
        while (temp->next != nullptr && temp->next->data != val)
        {
            temp = temp->next;
        }

        if (temp->next == nullptr)
            return;

        Node<T>* nodeToDelete = temp->next;
        temp->next            = temp->next->next;
        delete nodeToDelete;
    }

    // 打印链表
    void print() const
    {
        Node<T>* temp = head;
        while (temp != nullptr)
        {
            std::cout << temp->data << " -> ";
            temp = temp->next;
        }
        std::cout << "nullptr" << std::endl;
    }
};

int main()
{
    LinkedList<int> intList;

    intList.append(10);
    intList.append(20);
    intList.append(30);
    intList.prepend(5);

    std::cout << "list: ";
    intList.print();

    intList.remove(20);
    std::cout << "remove 20: ";
    intList.print();

    LinkedList<std::string> strList;

    strList.append("Hello");
    strList.append("World");
    strList.prepend("Hi");

    std::cout << "string list: ";
    strList.print();

    strList.remove("World");
    std::cout << "remove World: ";
    strList.print();

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <iostream>

// 定义链表节点结构
template <typename T>
struct Node
{
    T data;        // 节点存储的数据
    Node<T>* next; // 指向下一个节点的指针

    // 构造函数
    Node(T val)
        : data(val)
        , next(nullptr)
    {
    }
};

// 定义链表类
template <typename T>
class LinkedList
{
private:
    Node<T>* head; // 链表头指针

public:
    // 构造函数
    LinkedList()
        : head(nullptr)
    {
    }

    // 析构函数
    ~LinkedList()
    {
        Node<T>* current = head;
        Node<T>* nextNode;
        while (current != nullptr)
        {
            nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    // 在链表末尾添加节点
    void append(T val)
    {
        Node<T>* newNode = new Node<T>(val);
        if (head == nullptr)
        {
            head = newNode;
        }
        else
        {
            Node<T>* temp = head;
            while (temp->next != nullptr)
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    // 打印链表
    void print() const
    {
        Node<T>* temp = head;
        while (temp != nullptr)
        {
            std::cout << temp->data << " -> ";
            temp = temp->next;
        }
        std::cout << "nullptr" << std::endl;
    }

    // 翻转链表（迭代法）
    void reverseIterative()
    {
        Node<T>* prev    = nullptr; // 前一个节点
        Node<T>* current = head;    // 当前节点
        Node<T>* next    = nullptr; // 下一个节点

        while (current != nullptr)
        {
            next          = current->next; // 保存下一个节点
            current->next = prev;          // 当前节点指向前一个节点
            prev          = current;       // 前一个节点移动到当前节点
            current       = next;          // 当前节点移动到下一个节点
        }

        head = prev;                       // 更新头节点
    }

    // 翻转链表（递归法）
    void reverseRecursive()
    {
        head = reverseRecursiveHelper(head);
    }

private:
    // 递归辅助函数
    Node<T>* reverseRecursiveHelper(Node<T>* node)
    {
        if (node == nullptr || node->next == nullptr)
        {
            return node;                                       // 基本情况：空链表或只有一个节点
        }

        Node<T>* newHead = reverseRecursiveHelper(node->next); // 递归翻转剩余部分
        node->next->next = node;                               // 当前节点的下一个节点指向当前节点
        node->next       = nullptr;                            // 当前节点的下一个节点置空
        return newHead;                                        // 返回新的头节点
    }
};

int main()
{
    LinkedList<int> list;

    list.append(1);
    list.append(2);
    list.append(3);
    list.append(4);

    std::cout << "origin list: ";
    list.print();

    // 使用迭代法翻转链表
    list.reverseIterative();
    std::cout << "iterative: ";
    list.print();

    // 使用递归法翻转链表
    list.reverseRecursive();
    std::cout << "recursive: ";
    list.print();

    return 0;
}

#endif // TEST2

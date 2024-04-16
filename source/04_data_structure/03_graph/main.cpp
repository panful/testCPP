/**
 * 1. 有向无环图 DAG
 *
 * 有向图
 * 无向图
 * 简单图
 * 多重图
 * 完全图
 * 子图
 * 连通图
 * 带权图
 */

#define TEST1

#ifdef TEST1

#include <iostream>
#include <memory>
#include <string>
#include <vector>

template <typename T>
struct Node
{
    T data {};
    std::vector<std::shared_ptr<Node>> children;

    constexpr Node() noexcept = default;

    constexpr Node(T&& val) noexcept
        : data(val)
    {
    }

    void AddChild(const std::shared_ptr<Node>& node)
    {
        children.emplace_back(node);
    }
};

/// @brief 遍历所有路径
/// @tparam T 
/// @param current 
/// @param path 
template <typename T>
void TraversePath(const std::shared_ptr<Node<T>>& current, std::string path = "")
{
    path += std::to_string(current->data);
    if (current->children.empty())
    {
        // 叶子节点
        std::cout << path << '\n';
    }
    else
    {
        for (const auto& child : current->children)
        {
            TraversePath(child, path);
        }
    }
}

int main()
{
    //     0
    //    / \
    //   1   2
    //    \ /
    //     3
    //    / \
    //   4   5
    //  / \ /
    // 6   7

    auto root  = std::make_shared<Node<int>>(0);
    auto node1 = std::make_shared<Node<int>>(1);
    auto node2 = std::make_shared<Node<int>>(2);
    auto node3 = std::make_shared<Node<int>>(3);
    auto node4 = std::make_shared<Node<int>>(4);
    auto node5 = std::make_shared<Node<int>>(5);
    auto node6 = std::make_shared<Node<int>>(6);
    auto node7 = std::make_shared<Node<int>>(7);

    root->AddChild(node1);
    root->AddChild(node2);
    node1->AddChild(node3);
    node2->AddChild(node3);
    node3->AddChild(node4);
    node3->AddChild(node5);
    node4->AddChild(node6);
    node4->AddChild(node7);
    node5->AddChild(node7);

    TraversePath(root);
}

#endif // TEST1

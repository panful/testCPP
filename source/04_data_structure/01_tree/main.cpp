
/**
 * 01. 二叉树 Binary tree
 * 02. 二叉树前序遍历、中序遍历、后序遍历
 * 03. 深度优先遍历二叉树 DFS 广度优先遍历二叉树 BFS
 *
 * 平衡二叉树
 * 线索二叉树
 * 完全二叉树
 * 满二叉树
 * 斜二叉树
 * 红黑树
 */

#define TEST3

#ifdef TEST1

#include <memory>

template <typename T>
struct BTree
{
    T node {};
    std::unique_ptr<BTree<T>> left {};
    std::unique_ptr<BTree<T>> right {};

    constexpr BTree() noexcept = default;

    constexpr explicit BTree(T&& val) noexcept
        : node(val)
    {
    }

    constexpr BTree(T&& val, std::unique_ptr<BTree<T>> l, std::unique_ptr<BTree<T>> r) noexcept
        : node(val)
        , left(std::move(l))
        , right(std::move(r))
    {
    }

    constexpr void SetLeft(std::unique_ptr<BTree<T>> l) noexcept
    {
        left = std::move(l);
    }

    constexpr void SetRight(std::unique_ptr<BTree<T>> r) noexcept
    {
        right = std::move(r);
    }

    constexpr std::unique_ptr<BTree<T>>& GetLeft() noexcept
    {
        return left;
    }

    constexpr std::unique_ptr<BTree<T>>& GetRight() noexcept
    {
        return right;
    }
};

int main()
{
    {
        BTree<int> root {};
        root.node  = 1;
        root.left  = std::make_unique<BTree<int>>(2);
        root.right = std::make_unique<BTree<int>>(3);

        root.GetLeft()->left   = std::make_unique<BTree<int>>(4);
        root.GetRight()->right = std::make_unique<BTree<int>>(5);

        root.GetRight()->GetRight()->left = std::make_unique<BTree<int>>(6, std::make_unique<BTree<int>>(7), std::make_unique<BTree<int>>(8));

        auto&& r = root.GetRight()->GetRight()->GetLeft()->GetRight();
        r->SetLeft(std::make_unique<BTree<int>>(9));
    }
}

#endif // TEST1

#ifdef TEST2

#include <iostream>
#include <memory>

template <typename T>
struct BTree
{
    T node {};
    std::unique_ptr<BTree<T>> left {};
    std::unique_ptr<BTree<T>> right {};

    constexpr explicit BTree(T&& val) noexcept
        : node(val)
    {
    }
};

// 前序遍历，遍历顺序：根节点 -> 左子树 -> 右子树
void PreTraversal(std::unique_ptr<BTree<int>>& ptr)
{
    if (ptr)
    {
        std::cout << ptr->node << ' ';
        PreTraversal(ptr->left);
        PreTraversal(ptr->right);
    }
}

// 中序遍历，遍历顺序：左子树 -> 根节点 -> 右子树
void MidTraversal(std::unique_ptr<BTree<int>>& ptr)
{
    if (ptr)
    {
        MidTraversal(ptr->left);
        std::cout << ptr->node << ' ';
        MidTraversal(ptr->right);
    }
}

// 后序遍历，遍历顺序：左子树 -> 右子树 -> 根节点
void PostTraversal(std::unique_ptr<BTree<int>>& ptr)
{
    if (ptr)
    {
        PostTraversal(ptr->left);
        PostTraversal(ptr->right);
        std::cout << ptr->node << ' ';
    }
}

int main()
{
    // 构建一个简单的二叉树
    //       1
    //      / \
    //     2   3
    //    / \
    //   4   5
    //      / \
    //     6   7
    auto root                = std::make_unique<BTree<int>>(1);
    root->left               = std::make_unique<BTree<int>>(2);
    root->right              = std::make_unique<BTree<int>>(3);
    root->left->left         = std::make_unique<BTree<int>>(4);
    root->left->right        = std::make_unique<BTree<int>>(5);
    root->left->right->left  = std::make_unique<BTree<int>>(6);
    root->left->right->right = std::make_unique<BTree<int>>(7);

    std::cout << "Pre:\t";
    PreTraversal(root);
    std::cout << "\nMid:\t";
    MidTraversal(root);
    std::cout << "\nPost:\t";
    PostTraversal(root);
}

#endif // TEST2

#ifdef TEST3

#include <iostream>
#include <memory>
#include <queue>

template <typename T>
struct BTree
{
    T node {};
    std::unique_ptr<BTree<T>> left {};
    std::unique_ptr<BTree<T>> right {};

    constexpr explicit BTree(T&& val) noexcept
        : node(val)
    {
    }
};

// 深度优先遍历，前序、后序遍历都属于深度优先遍历的实现方式
void DFS(std::unique_ptr<BTree<int>>& ptr)
{
    if (ptr)
    {
        // 此处使用前序遍历
        std::cout << ptr->node << ' ';
        DFS(ptr->left);
        DFS(ptr->right);
    }
}

// 广度优先遍历，一层一层遍历（层序遍历）
void BFS(std::unique_ptr<BTree<int>>& ptr)
{
    if (ptr)
    {
        std::queue<std::unique_ptr<BTree<int>>> tmp_queue; // 先进先出
        tmp_queue.push(std::move(ptr));

        while (!tmp_queue.empty())
        {
            auto current_node = std::move(tmp_queue.front());
            tmp_queue.pop();
            std::cout << current_node->node << ' ';

            if (current_node->left)
            {
                tmp_queue.push(std::move(current_node->left));
            }
            if (current_node->right)
            {
                tmp_queue.push(std::move(current_node->right));
            }
        }
    }
}

int main()
{
    // 构建一个简单的二叉树
    //       1
    //      / \
    //     2   3
    //    / \
    //   4   5
    //      / \
    //     6   7
    auto root                = std::make_unique<BTree<int>>(1);
    root->left               = std::make_unique<BTree<int>>(2);
    root->right              = std::make_unique<BTree<int>>(3);
    root->left->left         = std::make_unique<BTree<int>>(4);
    root->left->right        = std::make_unique<BTree<int>>(5);
    root->left->right->left  = std::make_unique<BTree<int>>(6);
    root->left->right->right = std::make_unique<BTree<int>>(7);

    std::cout << "DFS:\t";
    DFS(root);
    std::cout << "\nBFS:\t";
    BFS(root);
}

#endif // TEST3

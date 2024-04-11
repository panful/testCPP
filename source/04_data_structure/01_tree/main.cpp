
/**
 * 01. 二叉树 Binary tree
 * 02. 二叉树前序遍历、中序遍历、后序遍历
 * 03. 深广度优先遍历二叉树 DFS BFS
 *
 * 平衡二叉树
 * 线索二叉树
 * 完全二叉树
 * 满二叉树
 * 斜二叉树
 * 红黑树
 */

#define TEST1

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

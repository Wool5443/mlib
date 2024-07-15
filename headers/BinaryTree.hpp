#ifndef MLIB_BINARY_TREE_HPP
#define MLIB_BINARY_TREE_HPP

#include "Utils.hpp"

namespace mlib {

template<typename T>
struct BinaryTreeNode
{
    T value{};
    BinaryTreeNode* left   = nullptr;
    BinaryTreeNode* right  = nullptr;
    BinaryTreeNode* parent = nullptr;

    BinaryTreeNode() noexcept {}
    BinaryTreeNode(const T& value) noexcept
        : value(value) {}
    BinaryTreeNode(const T& value,
                   BinaryTreeNode* left, BinaryTreeNode* right) noexcept
        : value(value), left(left), right(right)
    {
        if (left)
            left->parent  = this;
        if (right)
            right->parent = this;
    }
    Utils::Error SetLeft(BinaryTreeNode* node)
    {
        left         = node;
        node->parent = this;

        return Utils::Error();
    }
    Utils::Error SetRight(BinaryTreeNode* node)
    {
        right        = node;
        node->parent = this;

        return Utils::Error();
    }

    static Utils::Result<BinaryTreeNode*> New(T value = {})
    {
        auto node = new BinaryTreeNode(value);

        if (!node)
            return { nullptr, Utils::ERROR_NO_MEMORY };
        return { node, Utils::Error() };
    }
private:
    size_t id = getNewId();

    inline size_t getNewId()
    {
        static size_t id = 1;
        return id++;
    }
};

template<typename T>
class BinaryTree
{
private:
    BinaryTreeNode<T> root;
public:
    Utils::Error Error;
public:
    ~BinaryTree()
    {
        Error = recDtor(root.left);
        if (Error) return;
        Error = recDtor(root.right);
    }
private:
    static Utils::Error recDtor(BinaryTreeNode<T>* node)
    {
        if (node->left)
            RETURN_ERROR(recDtor(node->left));
        if (node->right)
            RETURN_ERROR(recDtor(node->right));

        delete node;

        return Utils::Error();
    }
};

} // namespace mlib

#endif

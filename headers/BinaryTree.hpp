#ifndef MLIB_BINARY_TREE_HPP
#define MLIB_BINARY_TREE_HPP

/**
 * @file BinaryTree.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains the implementation of a templated
 * binary tree
 *
 * @version 2.0
 * @date 20-07-2024
 *
 * @copyright Copyright (c) 2024
 */

#include <fstream>
#include "String.hpp"

namespace mlib {

/** @struct BinaryTreeNode
 * @brief Represents a tree node
 *
 * @tparam T value type
 */
template<typename T>
struct BinaryTreeNode final
{
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
public:
    T               value{};
    BinaryTreeNode* left   = nullptr; /**< ptr to left child */
    BinaryTreeNode* right  = nullptr; /**< ptr to right child */
    BinaryTreeNode* parent = nullptr; /**< ptr to parent */
    std::size_t     id     = getNewId(); /** id of the node */
    err::ErrorCode  error  = err::EVERYTHING_FINE; /** error which may occur in ctor */

    static const std::size_t BAD_ID = (std::size_t)-1;
///////////////////////////////////////////////////////////////////////////////
//
//                              SETTERS
//
///////////////////////////////////////////////////////////////////////////////
    /**
     * @brief Set the left child
     *
     * @param [in] node child
     *
     * @return err::ErrorCode
     */
    err::ErrorCode SetLeft(BinaryTreeNode* node)
    {
        RETURN_ERROR_IF(error);

        if (left)
            left->parent = nullptr;

        left         = node;
        node->parent = this;

        return err::EVERYTHING_FINE;
    }

    /**
     * @brief Set the right child
     *
     * @param [in] node child
     *
     * @return err::ErrorCode
     */
    err::ErrorCode SetRight(BinaryTreeNode* node)
    {
        RETURN_ERROR_IF(error);

        if (right)
            right->parent = nullptr;

        right        = node;
        node->parent = this;

        return err::EVERYTHING_FINE;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief The valid way of creating a node
     *
     * @param [in] value init value
     * @param [in] left left child
     * @param [in] right right child
     *
     * @return err::Result<BinaryTreeNode*> new node
     */
    static err::Result<BinaryTreeNode*> New(T&& value = {},
                                            BinaryTreeNode* left  = nullptr,
                                            BinaryTreeNode* right = nullptr)
    {
        auto node = new BinaryTreeNode(std::move(value), left, right);

        if (!node)
            RETURN_ERROR_RESULT_IF(err::ERROR_NO_MEMORY, nullptr, BinaryTreeNode*);

        if (left)
            left->parent  = node;
        if (right)
            right->parent = node;

        return node;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Clone the node
     *
     * @return err::Result<BinaryTreeNode*> copy
     */
    err::Result<BinaryTreeNode*> Clone()
    {
        err::Result<BinaryTreeNode*> left;
        err::Result<BinaryTreeNode*> right;

        if (left)
        {
            left = left->Clone();
            RETURN_ERROR_RESULT_IF(left, nullptr, BinaryTreeNode*);
        }

        if (right)
        {
            right = right->Clone();
            RETURN_ERROR_RESULT_IF(right, nullptr, BinaryTreeNode*, left->Delete());
        }

        return BinaryTreeNode::New(value, left.value, right.value);
    }

    /**
     * @brief Recursively deletes the node
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Delete()
    {
        if (id == BAD_ID)
            RETURN_ERROR_IF(err::ERROR_BAD_ID);
        if (left)
            RETURN_ERROR_IF(left->Delete());
        if (right)
            RETURN_ERROR_IF(right->Delete());

        delete this;

        return err::EVERYTHING_FINE;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR AND =
//
///////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Construct a new Binary Tree Node object
     *
     * @param [in] value init value
     * @param [in] left left child
     * @param [in] right right child
     */
    BinaryTreeNode(const T& value,
                   BinaryTreeNode* left, BinaryTreeNode* right) noexcept
        : value(value), left(left), right(right)
    {
        if (left)
            left->parent  = this;
        if (right)
            right->parent = this;
    }

    /**
     * @brief Construct a new Binary Tree Node object
     *
     * @param [in] value init value
     * @param [in] left left child
     * @param [in] right right child
     */
    BinaryTreeNode(T&& value,
                   BinaryTreeNode* left, BinaryTreeNode* right) noexcept
        : value(std::move(value)), left(left), right(right)
    {
        if (left)
            left->parent  = this;
        if (right)
            right->parent = this;
    }

    BinaryTreeNode(const BinaryTreeNode& other)            = delete;
    BinaryTreeNode(BinaryTreeNode&& other)                 = delete;
    BinaryTreeNode& operator=(const BinaryTreeNode& other) = delete;
    BinaryTreeNode& operator=(BinaryTreeNode&& other)      = delete;

    ~BinaryTreeNode() = default;

    std::size_t getNewId()
    {
        static std::size_t id = 1;
        return id++;
    }
};

/**
 * @brief Represents a binary tree
 *
 * @tparam T value type
 * @tparam MaxSize max depth
 */
template<typename T, std::size_t MaxSize = 1000>
class BinaryTree final
{
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
public:
    BinaryTreeNode<T>* root = nullptr; ///< root
private:
    str           m_dumpFolder{};
    std::ofstream m_htmlDumpFile{};
///////////////////////////////////////////////////////////////////////////////
//
//                              SETTERS/GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Get error
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Error() const noexcept { return root ? root->error : err::ERROR_NO_ROOT; }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR AND =
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new Binary Tree object
     * This ctor creates an empty Tree without a root
     */
    BinaryTree() noexcept {}

    /**
     * @brief Construct a new Binary Tree object
     *
     * @param [in] root
     */
    BinaryTree(BinaryTreeNode<T>* root) noexcept
        : root(root) {}

    /**
     * @brief Construct a new Binary Tree object
     * This ctor creates a valid tree with a root
     *
     * @param [in] value root init value
     */
    BinaryTree(const T& value)
    {
        auto _root = BinaryTreeNode<T>::New(value);
        if (_root.error)
        {
            LOG_ERROR(_root.error);
            return;
        }

        root  = _root.value;
    }

    /**
     * @brief Construct a new Binary Tree object
     * This ctor creates a valid tree with a root
     *
     * @param [in] value root init value
     */
    BinaryTree(T&& value)
    {
        auto _root = BinaryTreeNode<T>::New(std::move(value));
        if (_root.error)
        {
            LOG_ERROR(_root.error);
            return;
        }

        root  = _root.value;
    }

    /**
     * @brief Construct a new Binary Tree object
     *
     * @param [in] other
     */
    BinaryTree(const BinaryTree& other)
    {
        auto newRoot = other.root->Clone();

        if (newRoot.error)
        {
            LOG_ERROR(newRoot.error);
            return;
        }

        root = newRoot.value;
    }

    /**
     * @brief Copy assign
     *
     * @param [in] other
     *
     * @return BinaryTree&
     */
    BinaryTree& operator=(const BinaryTree& other)
    {
        auto newRoot = other.root->Clone();

        if (newRoot.error)
        {
            LOG_ERROR(newRoot.error);
            return *this;
        }

        err::ErrorCode error = root->Delete();
        if (error)
        {
            LOG_ERROR(error);
            return *this;
        }

        root = newRoot.value;

        return *this;
    }

    /**
     * @brief Move assign
     *
     * @param [in] other
     *
     * @return BinaryTree&
     */
    BinaryTree& operator=(BinaryTree&& other)
    {
        std::swap(root, other.root);
        std::swap(m_dumpFolder, other.m_dumpFolder);
        m_htmlDumpFile.close();
        std::swap(m_htmlDumpFile, other.m_htmlDumpFile);
    }

    /**
     * @brief Destroy the Binary Tree object
     */
    ~BinaryTree()
    {
        if (!root)
            return;

        err::ErrorCode error = root->Delete();
        if (error)
        {
            LOG_ERROR(error);
            return;
        }

        root = nullptr;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
    /**
     * @brief Construct a new LinkedList object
     *
     * @param value init root value
     *
     * @return err::Result<Tree>
     */
    static err::Result<BinaryTree> New(const T& value)
    {
        BinaryTree tree(value);
        return { tree, tree.Error() };
    }

    /**
     * @brief Construct a new Tree object
     *
     * @param value init root value
     *
     * @return err::Result<Tree>
     */
    static err::Result<BinaryTree> New(T&& value)
    {
        BinaryTree tree(value);
        return { tree, tree.Error() };
    }

    /**
     * @brief Construct a new Tree object
     * by copying
     *
     * @param other tree to copy
     *
     * @return err::Result<Tree>
     */
    static err::Result<BinaryTree> New(const BinaryTree& other)
    {
        BinaryTree tree(other);
        return { tree, tree.Error() };
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
#define FONT_SIZE "10"
#define FONT_NAME "\"Fira Code Bold\""
#define NODE_COLOR_OP "\"#f18f8f\""
#define NODE_COLOR_NUM "\"#eee7a0\""
#define NODE_COLOR_VAR "\"#a7e989\""
#define BACK_GROUND_COLOR "\"#de97d4\""
#define TREE_COLOR "\"#ff7be9\""
#define NODE_COLOR "\"#fae1f6\""
#define NODE_FRAME_COLOR "\"#000000\""
#define ROOT_COLOR "\"#c95b90\""
#define FREE_HEAD_COLOR "\"#b9e793\""

    /**
     * @brief Initializes dump files and writes
     * the header of the html log file
     *
     * @param [in] dumpFolder where to put dumps
     *
     * @return err::ErrorCode
     */
    err::ErrorCode InitDump(const char* dumpFolder)
    {
        HardAssert(dumpFolder, err::ERROR_NULLPTR);

        m_dumpFolder = dumpFolder;

        String dumpFile{m_dumpFolder};
        dumpFile += "/dump.html";

        m_htmlDumpFile.open(dumpFile);
        if (!m_htmlDumpFile)
            RETURN_ERROR_IF(err::ERROR_BAD_FILE);

        m_htmlDumpFile <<
        "<style>\n"
        ".content {\n"
        "max-width: 500px;\n"
        "margin: auto;\n"
        "}\n"
        "</style>\n"
        "<body>\n"
        "<div class=\"content\">";

        return err::EVERYTHING_FINE;
    }

    /**
     * @brief Ends logging
     */
    void FinishDump()
    {
        m_htmlDumpFile << "</div>\n</body>\n";
        m_htmlDumpFile.close();
    }

    /**
     * @brief Call only after InitDump
     * Dumps the tree
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Dump()
    {
        static       std::size_t dumpIteration = 0;

        static const std::size_t numStrMaxLength = 21;
        char iterString[numStrMaxLength] = "";
        sprintf(iterString, "%zu", dumpIteration);

        m_htmlDumpFile <<
        "<h1>Iteration " << dumpIteration <<"</h1>\n"
        "<style>\n"
        ".content {\n"
        "max-width: 500px;\n"
        "margin: auto;\n"
        "}\n"
        "</style>\n";

        String outGraphPath = m_dumpFolder;
        outGraphPath += "/dot/iter";
        outGraphPath += iterString;
        outGraphPath += ".dot";

        std::ofstream outGraphFile(outGraphPath);

        outGraphFile <<
        "digraph\n"
        "{\n"
        "rankdir = TB;\n"
        "node[shape = record, color = " NODE_FRAME_COLOR ", fontname = " FONT_NAME ", fontsize = " FONT_SIZE "];\n"
        "bgcolor = " BACK_GROUND_COLOR ";\n";

        outGraphFile <<
        "TREE[rank = \"min\", style = \"filled\", fillcolor = " TREE_COLOR ", "
                        "label = \"{Tree|Error: " << GetErrorName(Error()) << "|"
                        "<root>Root}\"];"
        "\nNODE_" << root << "[style = \"filled\", "
        "fillcolor = " NODE_COLOR ", "
        "label = \"{Value:\\n|" << root->value
        << "|id:\\n";

        if (root->id == BinaryTreeNode<T>::BAD_ID)
            outGraphFile << "BAD_ID";
        else
            outGraphFile << root->id;

        outGraphFile << "|{<left>Left|<right>Right}}\"];\n";

        std::size_t MAX_DEPTH = MaxSize;

        if (root->left)
            RETURN_ERROR_IF(recBuildCellTemplatesGraph(root->left,
                         outGraphFile, 0, MAX_DEPTH));
        if (root->right)
            RETURN_ERROR_IF(recBuildCellTemplatesGraph(root->right,
                         outGraphFile, 0, MAX_DEPTH));

        RETURN_ERROR_IF(recDrawGraph(root, outGraphFile, 0, MAX_DEPTH));

        outGraphFile << "\n"
        << "TREE:root->NODE_" << root <<"\n"
        << "}\n";

        outGraphFile.close();

        String outImgPath = m_dumpFolder;
        outImgPath += "/img/iter";
        outImgPath += iterString;
        outImgPath += ".png";

        String command = "dot ";
        command += outGraphPath;
        command += " -T png -o ";
        command += outImgPath;

        system(command);

        static const std::size_t maxPathLength = 256;
        char absoluteOutImgPath[maxPathLength];
        realpath(outImgPath, absoluteOutImgPath);

        m_htmlDumpFile << "<img src = \"" << absoluteOutImgPath << "\"/>\n";

        dumpIteration++;

        return err::EVERYTHING_FINE;
    }
private:
    static err::ErrorCode recBuildCellTemplatesGraph(const BinaryTreeNode<T>* node,
                                                     std::ofstream& outGraphFile,
                                                     std::size_t curDepth,
                                                     std::size_t maxDepth)
    {
        std::size_t nodeId = node->id;

        if (curDepth > maxDepth)
            RETURN_ERROR_IF(err::ERROR_BAD_RECURSION);

        outGraphFile << "NODE_" << node << "[style = \"filled\", "
        "fillcolor = " NODE_COLOR ", "
        "label = \"{Value:\\n" << node->value <<
        "|id:\\n";

        if (node->id == BinaryTreeNode<T>::BAD_ID)
            outGraphFile << "BAD_ID";
        else
            outGraphFile << node->id;

        outGraphFile << "|{<left>left|<right>right}}\"];\n";

        if (node->left)
            RETURN_ERROR_IF(recBuildCellTemplatesGraph(node->left,
                         outGraphFile, curDepth + 1, maxDepth));
        if (node->right)
            RETURN_ERROR_IF(recBuildCellTemplatesGraph(node->right,
                         outGraphFile, curDepth + 1, maxDepth));

        return err::EVERYTHING_FINE;
    }

    static err::ErrorCode recDrawGraph(const BinaryTreeNode<T>* node,
                                       std::ofstream& outGraphFile,
                                       std::size_t curDepth,
                                       std::size_t maxDepth)
    {
        if (curDepth > maxDepth)
            RETURN_ERROR_IF(err::ERROR_BAD_RECURSION);

        if (node->left)
        {
            outGraphFile << "NODE_" << node << ":left->NODE_" << node->left << ";\n";
            RETURN_ERROR_IF(recDrawGraph(node->left,
                         outGraphFile, curDepth + 1, maxDepth));
        }
        if (node->right)
        {
            outGraphFile << "NODE_" << node << ":right->NODE_" << node->right << ";\n";
            RETURN_ERROR_IF(recDrawGraph(node->right,
                         outGraphFile, curDepth + 1, maxDepth));
        }

        return err::EVERYTHING_FINE;
    }
#undef FONT_SIZE
#undef FONT_NAME
#undef NODE_COLOR_OP
#undef NODE_COLOR_NUM
#undef NODE_COLOR_VAR
#undef BACK_GROUND_COLOR
#undef TREE_COLOR
#undef NODE_COLOR
#undef NODE_FRAME_COLOR
#undef ROOT_COLOR
#undef FREE_HEAD_COLOR
};

} // namespace mlib

#endif

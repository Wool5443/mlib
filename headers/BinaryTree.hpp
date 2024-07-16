#ifndef MLIB_BINARY_TREE_HPP
#define MLIB_BINARY_TREE_HPP

//!@file
/**
 * @file BinaryTree.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains implementation of a binary
 *        tree using templates
 * @version 1.0
 * @date 16-07-2024
 * 
 * @copyright Copyright (c) 2024
 */

#include <fstream>
#include "Utils.hpp"
#include "String.hpp"

namespace mlib {

/** @struct BinaryTreeNode
 * @brief Represents a tree node
 *
 * @tparam T value type
 */
template<typename T>
struct BinaryTreeNode
{
    T               value{};
    BinaryTreeNode* left   = nullptr; /**< ptr to left child */
    BinaryTreeNode* right  = nullptr; /**< ptr to right child */
    BinaryTreeNode* parent = nullptr; /**< ptr to parent */
    std::size_t     id     = getNewId(); /** id of the node */
    Utils::Error    error  = Utils::Error(); /** errors which may occur in ctor */

    /**
     * @brief Construct a new Binary Tree Node object
     */
    BinaryTreeNode() noexcept {}

    /**
     * @brief Construct a new Binary Tree Node object
     *
     * @param [in] value init value
     */
    BinaryTreeNode(const T& value) noexcept
        : value(value) {}

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
     * @brief BinaryTreeNode copy constructore
     *
     * @param [in] other node to copy
     */
    BinaryTreeNode(const BinaryTreeNode& other)
        : value(other.value)
    {
        auto left = other.left->Clone();
        if (!left)
        {
            error = left;
            return;
        }

        auto right = other.left->Clone();
        if (!right)
        {
            error = right;
            return;
        }

        left  = left;
        right = right;
    }

    /**
     * @brief BinaryTreeNode move constructor
     *
     * @param [in] other node to move
     */
    BinaryTreeNode(BinaryTreeNode&& other)
        : value(std::move(other.value)),
          left(other.left), right(other.right),
          parent(other.parent),
          id(other.id)
    {
        other.left   = nullptr;
        other.right  = nullptr;
        other.parent = nullptr;
    }

    /**
     * @brief Copy assignment is forbidden
     *
     * @param [in] other node to copy
     * @return BinaryTreeNode&
     */
    BinaryTreeNode& operator=(const BinaryTreeNode& other) = delete;
    BinaryTreeNode& operator=(BinaryTreeNode&& other)
    {
        value  = std::move(other.value);
        left   = other.left;
        right  = other.right;
        parent = other.parent;
        id     = other.id;

        return *this;
    }

    /**
     * @brief Set the left child
     * 
     * @param [in] node child
     * @return Utils::Error 
     */
    Utils::Error SetLeft(BinaryTreeNode* node)
    {
        left         = node;
        node->parent = this;

        return Utils::Error();
    }
    /**
     * @brief Set the right child
     * 
     * @param [in] node child
     * @return Utils::Error 
     */
    Utils::Error SetRight(BinaryTreeNode* node)
    {
        right        = node;
        node->parent = this;

        return Utils::Error();
    }

    /**
     * @brief Clone the node
     * 
     * @return Utils::Result<BinaryTreeNode*> copy
     */
    Utils::Result<BinaryTreeNode*> Clone()
    {
        Utils::Result<BinaryTreeNode*> left  = {};
        Utils::Result<BinaryTreeNode*> right = {};

        if (left)
            left = left->Clone();
        RETURN_RESULT(left);
        if (right)
            right = right->Clone();
        RETURN_RESULT(right);

        auto node = BinaryTreeNode::New(value, left.value, right.value);

        return node;
    }

    /**
     * @brief Allocates memory for the node and returns a pointer
     *
     * @param [in] value init value
     * @param [in] left left child
     * @param [in] right right child
     * @return Utils::Result<BinaryTreeNode*> new node
     */
    static Utils::Result<BinaryTreeNode*> New(const T& value = {},
                                              BinaryTreeNode* left  = nullptr,
                                              BinaryTreeNode* right = nullptr)
    {
        auto node = new BinaryTreeNode(value, left, right);

        if (!node)
            return { nullptr, CREATE_ERROR(Utils::ERROR_NO_MEMORY) };

        if (left)
            left->parent  = node;
        if (right)
            right->parent = node;

        return { node, Utils::Error() };
    }

    Utils::Error Delete()
    {
        if (this->left)
            RETURN_ERROR(this->left->Delete());
        if (this->right)
            RETURN_ERROR(this->right->Delete());

        delete this;

        return Utils::Error();
    }
private:
    inline std::size_t getNewId()
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
class BinaryTree
{
public:
    BinaryTreeNode<T>* root  = nullptr; ///< root
    Utils::Error       error{}; ///< error from the ctor
private:
    String<>      m_logFolder{};
    std::ofstream m_htmlLogFile{};

    static const std::size_t BAD_ID = Utils::SIZET_POISON;
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
        error = _root;
        root  = _root;
    }

    ~BinaryTree()
    {
        if (root)
            error = root->Delete();
    }
    BinaryTree(const BinaryTree& other)            = delete;
    // BinaryTree(BinaryTree&& other)                 = delete;
    BinaryTree& operator=(const BinaryTree& other) = delete;
    BinaryTree& operator=(BinaryTree&& other)      = delete;
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
     * @brief Initializes log files and writes
     * the header of the html log file
     * 
     * @param [in] logFolder where to put logs
     */
    void StartLogging(const char* logFolder)
    {
        HardAssert(logFolder, Utils::ERROR_NULLPTR);

        m_logFolder = logFolder;

        m_htmlLogFile.open(m_logFolder + "/log.html");

        m_htmlLogFile <<
        "<style>\n"
        ".content {\n"
        "max-width: 500px;\n"
        "margin: auto;\n"
        "}\n"
        "</style>,\n"
        "<body>\n"
        "<div class=\"content\">";
    }

    /**
     * @brief Ends logging
     */
    void EndLogging()
    {
        m_htmlLogFile << "</div>\n</body>\n";
        m_htmlLogFile.close();
    }

    /**
     * @brief Call only after StartLogging
     * Dumps a tree
     *
     * @return Utils::Error
     */
    Utils::Error Dump()
    {
        static const std::size_t NUM_STR_MAX_LENGTH = 21;
        static std::size_t dumpIteration = 0;

        char iterString[NUM_STR_MAX_LENGTH] = "";
        sprintf(iterString, "%zu", dumpIteration);

        m_htmlLogFile <<
        "<h1>Iteration " << dumpIteration <<"</h1>\n"
        "<style>\n"
        ".content {\n"
        "max-width: 500px;\n"
        "margin: auto;\n"
        "}\n"
        "</style>,\n";

        String outGraphPath = m_logFolder;
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
                        "label = \"{Tree|Error: " << error.GetErrorName() << "|"
                        "<root>Root}\"];"
        "\nNODE_" << root << "[style = \"filled\", "
        "fillcolor = " NODE_COLOR ", "
        "label = \"{Value:\\n|" << root->value <<
        "|{<left>Left|<right>Right}}\"];\n";

        std::size_t MAX_DEPTH = MaxSize;

        if (root->left)
            RETURN_ERROR(recBuildCellTemplatesGraph(root->left,  outGraphFile, 0, MAX_DEPTH));
        if (root->right)
            RETURN_ERROR(recBuildCellTemplatesGraph(root->right, outGraphFile, 0, MAX_DEPTH));

        RETURN_ERROR(recDrawGraph(root, outGraphFile, 0, MAX_DEPTH));
        outGraphFile << "\n"
        << "TREE:root->NODE_" << root <<"\n"
        << "}\n";

        outGraphFile.close();

        String outImgPath = m_logFolder;
        outImgPath += "/img/iter";
        outImgPath += iterString;
        outImgPath += ".png";

        String command = "dot ";
        command += outGraphPath;
        command += " -T png -o ";
        command += outImgPath;

        system(command);

        m_htmlLogFile << "<img src = \"" << outImgPath << "\"/>\n";

        dumpIteration++;

        return Utils::Error();
    }
private:
    static Utils::Error recBuildCellTemplatesGraph(const BinaryTreeNode<T>* node,
                                                   std::ofstream& outGraphFile,
                                                   std::size_t curDepth,
                                                   std::size_t maxDepth)
    {
        std::size_t nodeId = node->id;
        
        if (curDepth > maxDepth)
            return CREATE_ERROR(Utils::ERROR_BAD_TREE);

        outGraphFile << "NODE_" << node << "[style = \"filled\", "
        "fillcolor = " NODE_COLOR ", "
        "label = \"{Value:\\n" << node->value <<
        "|id:\\n";

        if (node->id == BAD_ID)
            outGraphFile << "BAD_ID";
        else
            outGraphFile << node->id;

        outGraphFile << "|{<left>left|<right>right}}\"];\n";
        
        if (node->left)
            RETURN_ERROR(recBuildCellTemplatesGraph(
                node->left,  outGraphFile, curDepth + 1, maxDepth
            ));
        if (node->right)
            RETURN_ERROR(recBuildCellTemplatesGraph(
                node->right, outGraphFile, curDepth + 1, maxDepth
            ));

        return Utils::Error();
    }

    static Utils::Error recDrawGraph(const BinaryTreeNode<T>* node,
                                     std::ofstream& outGraphFile,
                                     std::size_t curDepth,
                                     std::size_t maxDepth)
    {
        if (curDepth > maxDepth)
            return CREATE_ERROR(Utils::ERROR_BAD_TREE);

        if (node->left)
        {
            outGraphFile << "NODE_" << node << ":left->NODE_" << node->left << ";\n";
            RETURN_ERROR(recDrawGraph(node->left, outGraphFile, curDepth + 1, maxDepth));
        }
        if (node->right)
        {
            outGraphFile << "NODE_" << node << ":right->NODE_" << node->right << ";\n";
            RETURN_ERROR(recDrawGraph(node->right, outGraphFile, curDepth + 1, maxDepth));
        }

        return Utils::Error();
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

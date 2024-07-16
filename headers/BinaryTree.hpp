#ifndef MLIB_BINARY_TREE_HPP
#define MLIB_BINARY_TREE_HPP

#include <fstream>
#include "Utils.hpp"
#include "String.hpp"

namespace mlib {

template<typename T>
struct BinaryTreeNode
{
    T               Value{};
    BinaryTreeNode* Left   = nullptr;
    BinaryTreeNode* Right  = nullptr;
    BinaryTreeNode* Parent = nullptr;
    std::size_t     id     = getNewId();
    Utils::Error    Error  = Utils::Error();

    BinaryTreeNode() noexcept {}

    BinaryTreeNode(const T& value) noexcept
        : Value(value) {}

    BinaryTreeNode(const T& value,
                   BinaryTreeNode* left, BinaryTreeNode* right) noexcept
        : Value(value), Left(left), Right(right)
    {
        if (left)
            left->Parent  = this;
        if (right)
            right->Parent = this;
    }

    BinaryTreeNode(const BinaryTreeNode& other)
        : Value(other.Value)
    {
        auto left = other.Left->Clone();
        if (!left)
        {
            Error = left;
            return;
        }

        auto right = other.Left->Clone();
        if (!right)
        {
            Error = right;
            return;
        }

        Left  = left;
        Right = right;
    }

    BinaryTreeNode(BinaryTreeNode&& other)
        : Value(std::move(other.Value)),
          Left(other.Left), Right(other.Right),
          Parent(other.Parent),
          id(other.id)
    {
        other.Left   = nullptr;
        other.Right  = nullptr;
        other.Parent = nullptr;
    }

    BinaryTreeNode& operator=(const BinaryTreeNode& other) = delete;
    BinaryTreeNode& operator=(BinaryTreeNode&& other)
    {
        Value  = std::move(other.Value);
        Left   = other.Left;
        Right  = other.Right;
        Parent = other.Parent;
        id     = other.id;

        return *this;
    }

    Utils::Error SetLeft(BinaryTreeNode* node)
    {
        Left         = node;
        node->Parent = this;

        return Utils::Error();
    }
    Utils::Error SetRight(BinaryTreeNode* node)
    {
        Right        = node;
        node->Parent = this;

        return Utils::Error();
    }

    Utils::Result<BinaryTreeNode*> Clone()
    {
        Utils::Result<BinaryTreeNode*> left  = {};
        Utils::Result<BinaryTreeNode*> right = {};

        if (Left)
            left = Left->Clone();
        RETURN_RESULT(left);
        if (Right)
            right = Right->Clone();
        RETURN_RESULT(right);

        auto node = BinaryTreeNode::New(Value, left.value, right.value);

        return node;
    }

    static Utils::Result<BinaryTreeNode*> New(const T& value = {},
                                              BinaryTreeNode* left  = nullptr,
                                              BinaryTreeNode* right = nullptr)
    {
        auto node = new BinaryTreeNode(value, left, right);

        if (!node)
            return { nullptr, CREATE_ERROR(Utils::ERROR_NO_MEMORY) };

        return { node, Utils::Error() };
    }
private:
    inline std::size_t getNewId()
    {
        static std::size_t id = 1;
        return id++;
    }
};

template<typename T, std::size_t MaxSize = 1000>
class BinaryTree
{
public:
    BinaryTreeNode<T>* Root  = nullptr;
    Utils::Error       Error{};
private:
    String<>      m_logFolder{};
    std::ofstream m_htmlLogFile{};

    static const std::size_t BAD_ID = Utils::SIZET_POISON;
public:
    BinaryTree() noexcept {}

    BinaryTree(BinaryTreeNode<T>* root) noexcept
        : Root(root) {}

    BinaryTree(const T& value)
    {
        auto root = BinaryTreeNode<T>::New(value);
        if (!root)
        {
            Error = root;
            return;
        }
        Root = root.value;
    }

    ~BinaryTree()
    {
        Error = recDtor(Root->Left);
        if (Error) return;
        Error = recDtor(Root->Right);
    }
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
    Utils::Error StartLogging(const char* logFolder)
    {
        SoftAssert(logFolder, Utils::ERROR_NULLPTR);

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

        return Utils::Error();
    }

    Utils::Error EndLogging()
    {
        m_htmlLogFile << "</div>\n</body>\n";
        m_htmlLogFile.close();

        return Utils::Error();
    }

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
                        "label = \"{Tree|Error: " << Error.GetErrorName() << "|"
                        "<root>Root}\"];"
        "\nNODE_" << &Root << "[style = \"filled\", "
        "fillcolor = " NODE_COLOR ", "
        "label = \"{Value:\\n|" << Root.Value <<
        "|{<left>Left|<right>Right}}\"];\n";

        std::size_t MAX_DEPTH = MaxSize;

        if (Root->Left)
            RETURN_ERROR(recBuildCellTemplatesGraph(Root->Left,  outGraphFile, 0, MAX_DEPTH));
        if (Root->Right)
            RETURN_ERROR(recBuildCellTemplatesGraph(Root->Right, outGraphFile, 0, MAX_DEPTH));

        RETURN_ERROR(recDrawGraph(Root, outGraphFile, 0, MAX_DEPTH));
        outGraphFile << "\n"
        << "TREE:root->NODE_" << Root <<"\n"
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
        "label = \"{Value:\\n" << node->Value <<
        "|id:\\n";

        if (node->id == BAD_ID)
            outGraphFile << "BAD_ID";
        else
            outGraphFile << node->id;

        outGraphFile << "|{<left>left|<right>right}}\"];\n";
        
        if (node->Left)
            RETURN_ERROR(recBuildCellTemplatesGraph(
                node->Left,  outGraphFile, curDepth + 1, maxDepth
            ));
        if (node->Right)
            RETURN_ERROR(recBuildCellTemplatesGraph(
                node->Right, outGraphFile, curDepth + 1, maxDepth
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

        if (node->Left)
        {
            outGraphFile << "NODE_" << node << ":left->NODE_" << node->Left << ";\n";
            RETURN_ERROR(recDrawGraph(node->Left, outGraphFile, curDepth + 1, maxDepth));
        }
        if (node->Right)
        {
            outGraphFile << "NODE_" << node << ":right->NODE_" << node->Right << ";\n";
            RETURN_ERROR(recDrawGraph(node->Right, outGraphFile, curDepth + 1, maxDepth));
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
private:
    static Utils::Error recDtor(BinaryTreeNode<T>* node)
    {
        if (node->Left)
            RETURN_ERROR(recDtor(node->Left));
        if (node->Right)
            RETURN_ERROR(recDtor(node->Right));

        delete node;

        return Utils::Error();
    }
};

} // namespace mlib

#endif

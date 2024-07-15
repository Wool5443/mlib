#ifndef MLIB_BINARY_TREE_HPP
#define MLIB_BINARY_TREE_HPP

#include <fstream>
#include "Utils.hpp"

namespace mlib {

template<typename T>
struct BinaryTreeNode
{
    T               Value{};
    BinaryTreeNode* Left   = nullptr;
    BinaryTreeNode* Right  = nullptr;
    BinaryTreeNode* Parent = nullptr;

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

    static Utils::Result<BinaryTreeNode*> New(T value = {})
    {
        auto node = new BinaryTreeNode(value);

        if (!node)
            return { nullptr, CREATE_ERROR(Utils::ERROR_NO_MEMORY) };
        return { node, Utils::Error() };
    }
private:
    std::size_t id = getNewId();

    inline std::size_t getNewId()
    {
        static std::size_t id = 1;
        return id++;
    }
};

template<typename T, std::size_t MaxSize>
class BinaryTree
{
public:
    BinaryTreeNode<T> Root = {};
    Utils::Error Error     = Utils::Error();
private:
    String<>      m_logFolder{};
    std::ofstream m_htmlLogFile{};
public:
    ~BinaryTree()
    {
        Error = recDtor(Root.Left);
        if (Error) return;
        Error = recDtor(Root.Right);
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

    Utils::Error Dump()
    {
        static const std::size_t NUM_STR_MAX_LENGTH = 21;
        static std::size_t dumpIteration = 0;

        char iterString[NUM_STR_MAX_LENGTH] = "";
        sprintf(iterString, "%zu", dumpIteration);

        SoftAssert(Root, Utils::ERROR_NO_ROOT);

        if (m_htmlLogFile)
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

        outGraphFile << "TREE[rank = \"min\", style = \"filled\", fillcolor = " TREE_COLOR ", "
                        "label = \"{Tree|Error: " << Error.GetErrorName() << "|"
                        "<root>Root}\"];";

        outGraphFile << "\nNODE_" << &Root << "[style = \"filled\", "
                        "fillcolor = " NODE_COLOR ", ";

        outGraphFile << "label = \"{Value:\\n|" << Root.Value;
        outGraphFile << "|{<left>Left|<right>Right}}\"];\n";

        std::size_t MAX_DEPTH = MAX_TREE_SIZE;

        if (this->root->left)
            RETURN_ERROR(_recBuildCellTemplatesGraph(this->root->left,  outGraphFile, 0, MAX_DEPTH));
        if (this->root->right)
            RETURN_ERROR(_recBuildCellTemplatesGraph(this->root->right, outGraphFile, 0, MAX_DEPTH));

        RETURN_ERROR(_recDrawGraph(this->root, outGraphFile, 0, MAX_DEPTH));
        fprintf(outGraphFile, "\n");
        fprintf(outGraphFile, "TREE:root->NODE_%p\n", this->root);

        fprintf(outGraphFile, "}\n");
        fclose(outGraphFile);

        char command[MAX_COMMAND_LENGTH] = "";
        sprintf(command, "dot %s -T png -o %s/Iteration%zu.png", outGraphPath, IMG_FOLDER.buf, dumpIteration);
        system(command);

        if (m_htmlLogFile)
            fprintf(m_htmlLogFile, "<img src = \"%s/Iteration%zu.png\"/>\n", IMG_FOLDER.buf, dumpIteration);

        dumpIteration++;

        return Error();
    }

    static Error _recBuildCellTemplatesGraph(TreeNode* node, FILE* outGraphFile,
                                                std::size_t curDepth, const std::size_t maxDepth)
    {
        SoftAssert(node, ERROR_NULLPTR);

        std::size_t nodeId = node->id;
        
        if (curDepth > maxDepth)
            return Error();

        fprintf(outGraphFile, "\nNODE_%p[style = \"filled\", ", node);
        switch (NODE_TYPE(node))
        {
            case OPERATION_TYPE:
                fprintf(outGraphFile, "fillcolor = " NODE_COLOR_OP ", ");
                break;
            case NUMBER_TYPE:
                fprintf(outGraphFile, "fillcolor = " NODE_COLOR_NUM ", ");
                break;
            case NAME_TYPE:
                fprintf(outGraphFile, "fillcolor = " NODE_COLOR_VAR ", ");
                break;
            default:
                return CREATE_ERROR(CREATE_ERROR(ERROR_BAD_VALUE));
        }

        fprintf(outGraphFile, "label = \"{Value:\\n");
        RETURN_ERROR(_printTreeElement(outGraphFile, node->value));
        fprintf(outGraphFile, "|id:\\n");

        if (node->id == BAD_ID)
            fprintf(outGraphFile, "BAD_ID");
        else
            fprintf(outGraphFile, "%zu", node->id);

        #ifndef NDEBUG
        fprintf(outGraphFile, "|node count:\\n%zu", node->nodeCount);
        #endif
        fprintf(outGraphFile, "|{<left>left|<right>right}}\"];\n");
        
        if (node->left)
            RETURN_ERROR(_recBuildCellTemplatesGraph(node->left,  outGraphFile, curDepth + 1, maxDepth));
        if (node->right)
            RETURN_ERROR(_recBuildCellTemplatesGraph(node->right, outGraphFile, curDepth + 1, maxDepth));

        return Error();
    }

    #undef FONT_SIZE
    #undef FONT_NAME
    #undef BACK_GROUND_COLOR
    #undef NODE_COLOR
    #undef NODE_FRAME_COLOR
    #undef ROOT_COLOR
    #undef FREE_HEAD_COLOR

    static Error _recDrawGraph(TreeNode* node, FILE* outGraphFile, std::size_t curDepth, const std::size_t maxDepth)
    {
        SoftAssert(node, ERROR_NULLPTR);
        if (curDepth > maxDepth)
            return Error();

        if (node->left)
        {
            fprintf(outGraphFile, "NODE_%p:left->NODE_%p;\n", node, node->left);
            RETURN_ERROR(_recDrawGraph(node->left, outGraphFile, curDepth + 1, maxDepth));
        }
        if (node->right)
        {
            fprintf(outGraphFile, "NODE_%p:right->NODE_%p;\n", node, node->right);
            RETURN_ERROR(_recDrawGraph(node->right, outGraphFile, curDepth + 1, maxDepth));
        }
        return Error();
    }

    Error Tree::StartLogging(const char* logFolder)
    {
        SoftAssert(logFolder, ERROR_NULLPTR);

        LOG_FOLDER.Create(logFolder);

        DOT_FOLDER.Create(&LOG_FOLDER);
        DOT_FOLDER.Append("/dot");

        IMG_FOLDER.Create(&LOG_FOLDER);
        IMG_FOLDER.Append("/img");

        char m_htmlLogFile_PATH[MAX_PATH_LENGTH];
        sprintf(m_htmlLogFile_PATH, "%s/log.html", logFolder);

        m_htmlLogFile = fopen(HTML_FILE_PATH, "w");
        SoftAssert(m_htmlLogFile, ERROR_BAD_FILE);

        fprintf(m_htmlLogFile, 
            "<style>\n"
            ".content {\n"
            "max-width: 500px;\n"
            "margin: auto;\n"
            "}\n"
            "</style>,\n"
            "<body>\n"
            "<div class=\"content\">");

        return Error();
    }

    Utils::Error EndLogging()
    {
        if (m_htmlLogFile)
        {
            fprintf(m_htmlLogFile, "</div>\n</body>\n");
            fclose(m_htmlLogFile);
        }
        m_htmlLogFile = NULL;

        LOG_FOLDER.Destructor();
        DOT_FOLDER.Destructor();
        IMG_FOLDER.Destructor();

        return Error();
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

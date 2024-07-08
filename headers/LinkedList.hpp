#ifndef MLIB_LINKED_LIST_HPP
#define MLIB_LINKED_LIST_HPP

#include <iostream>
#include <fstream>
#include "GeneralBuffer.hpp"

namespace mlib {

template<typename T, size_t DefaultCapacity = 8, size_t GrowFactor = 2>
class LinkedList
{
private:
    Buffer<T,      DefaultCapacity, GrowFactor> m_data;
    Buffer<size_t, DefaultCapacity, GrowFactor> m_next;
    Buffer<size_t, DefaultCapacity, GrowFactor> m_prev;
    size_t m_freeHead;

    static String        sm_logFolder;
    static std::ofstream sm_htmlLogFile;
public:
    inline size_t Head() const noexcept { return m_next[0]; }
    inline size_t Tail() const noexcept { return m_prev[0]; }
public:
    inline size_t       Length() const noexcept { return m_data.Length; }
    inline Utils::Error Error()  const noexcept
    {
        if (m_data.Error) return m_data.Error;
        if (m_next.Error) return m_next.Error;
        return m_prev.Error;
    }
public:
    LinkedList()
        : LinkedList(0) {}

    LinkedList(size_t hintLength)
        : m_data(hintLength), m_next(hintLength), m_prev(hintLength),
          m_freeHead(1) {}

          T& operator[](size_t index)       & noexcept { return m_data[index]; }
    const T& operator[](size_t index) const & noexcept { return m_data[index]; }
public:
    void StartLogging(const char* logFolder) noexcept
    {
        HardAssert(logFolder, Utils::ErrorCode::ERROR_BAD_FILE);
        sm_logFolder = logFolder;

        String htmlFilePath = logFolder;

        htmlFilePath += "/log.html";

        sm_htmlLogFile.open(htmlFilePath);

        sm_htmlLogFile <<
            "<style>\n"
            ".content {\n"
            "max-width: 500px;\n"
            "margin: auto;\n"
            "}\n"
            "</style>,\n"
            "<body>\n"
            "<div class=\"content\">";
    }

    void EndLogging() noexcept
    {
        sm_htmlLogFile.close();

        return Error();
    }

    #define FONT_SIZE "10"
    #define FONT_NAME "\"Fira Code Bold\""
    #define BACK_GROUND_COLOR "\"#de97d4\""
    #define TREE_COLOR "\"#ff7be9\""
    #define NODE_COLOR "\"#fae1f6\""
    #define NODE_FRAME_COLOR "\"#000000\""
    #define ROOT_COLOR "\"#c95b90\""
    #define FREE_HEAD_COLOR "\"#b9e793\""
    
    constexpr size_t FREE_ELEM = Utils::SIZET_POISON;

    Utils::Error Dump() const noexcept
    {
        static const size_t NUM_STR_MAX_LENGTH = 21;
        static size_t       dumpIteration      = 0;

        char iterString[NUM_STR_MAX_LENGTH] = "";
        sprintf(iterString, "%zu", dumpIteration);

        Utils::Error error = Error();

        String outTextPath = sm_logFolder + "/txt/iter" + iterString
                             + ".txt";

        String outGraphPath = sm_logFolder + "/dot/iter" + iterString
                             + ".txt";

        sm_htmlLogFile << "<h1>Iteration" << iterString << "</h1>\n<pre>\n";

        RETURN_ERROR(_dumpListText(this, error, outTextPath));

        sm_htmlLogFile << "</pre>\n";

        RETURN_ERROR(_dumpListGraph(this, outGraphPath));

        String outImgPath = sm_logFolder + "/img/iter" + iterString
                             + ".png";

        String command = "dot " + outGraphPath + " -T png -o" + outImgPath;

        system(command);

        sm_htmlLogFile << "<img src = \"" << outImgPath << "\"/>\n";

        dumpIteration++;

        return Error();
    }

private:
    #define PRINT_LOG(...)                  \
    do                                      \
    {                                       \
        outTextFile    << __VA_ARGS__;      \
        sm_htmlLogFile << __VA_ARGS__;      \
    } while (0)

    Utils::Error dumpListText(const String<>& outTextPath, Utils::Error error)
    {
        std::ofstream outTextFile{outTextPath};
        if (!outTextFile)
            return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_FILE);

        PRINT_LOG("List[" << this << "]\n");
        PRINT_LOG("List condition - " << error.GetErrorName() << "[" <<
                   static_cast<int>(error) << "]\n");

        PRINT_LOG("{\n");
        PRINT_LOG("    length = " << Length << "\n");
        PRINT_LOG("    capacity = " << m_data.GetCapacity() << "\n");
        PRINT_LOG("    head = " << Head() << "\n");
        PRINT_LOG("    tail = " << Tail() << "\n");
        PRINT_LOG("    free head = " << m_freeHead << "\n");
        PRINT_LOG("    list:\n", "");
        {
            size_t curEl = Head();
            size_t index = 1;
            while (curEl != 0 && index <= Length * 2)
            {
                PRINT_LOG("    *[" << index << "] = " << m_data[curEl] << "\n");
                curEl = m_next[curEl];
                index++;
            }
        }

        PRINT_LOG("\n    data[" << m_data << "]\n");
        for (size_t i = 0, end = m_data.GetCapacity(); i < end; i++)
        {
            PRINT_LOG("    *[" << i << "] = " << m_data[i] << "\n");
        }

        PRINT_LOG("\n    prev[" << m_prev << "]\n");
        for (size_t i = 0, end = m_prev.GetCapacity(); i < end; i++)
        {
            if (m_prev[i] != FREE_ELEM)
                PRINT_LOG("    *[" << i << "] = " << m_prev[i] << "\n");
            else
                PRINT_LOG("     [" << i << "] = FREE\n");
        }

        PRINT_LOG("\n    next[" << m_next << "]\n");
        for (size_t i = 0, end = m_next.GetCapacity(); i < end; i++)
        {
            if (m_next[i] != FREE_ELEM)
                PRINT_LOG("    *[" << i << "] = " << m_prev[i] << "\n");
            else
                PRINT_LOG("     [" << i << "] = BAD\n");
        }
            
        PRINT_LOG("}\n");

        return Error();
    }

    #undef PRINT_LOG

    Utils::Error dumpListGraph(const String<>& outGraphPath)
    {
        std::ofstream outGraphFile{outGraphPath};
        if (!outGraphFile)
            return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_FILE);

        outGraphFile <<
        "digraph\n"
        "{\n"
        "rankdir = LR;\n"
        "node[shape = record, color = " NODE_FRAME_COLOR ", fontname = " FONT_NAME ", fontsize = " FONT_SIZE "];\n"
        "bgcolor = " BACK_GROUND_COLOR ";\n"

        "ROOT[style = \"filled\", fillcolor = " ROOT_COLOR ", "
        "label = \"ROOT|{<head>head = " << Head() << "|<tail>tail = " <<
        Tail() << "}\"];\n"
        "FREE_HEAD[style = \"filled\", fillcolor = " FREE_HEAD_COLOR ", "
        "label = \"FREE HEAD|<freeHead>freeHead = " << m_freeHead << "\"];\n";
        
        for (size_t i = 1, end = m_data.GetCapacity(); i < end; i++)
        {
            outGraphFile <<
            "CELL_%zu[style = \"filled\", fillcolor = " NODE_COLOR ", ", i;
            outGraphFile << "label = \"index = " << i << "|";

            outGraphFile << "value\\n" << m_data[i] << "|";

            if (m_prev[i] == FREE_ELEM)
                outGraphFile << "{prev = FREE|";
            else
                outGraphFile << "{prev = " << m_prev[i] << "|";

            if (m_next[i] == FREE_ELEM)
                outGraphFile << "{next = BAD|";
            else
                outGraphFile << "{next = " << m_next[i] << "|";
        }

        fprintf(outGraphFile, "ROOT->CELL_1");
        for (size_t i = 2; i < list->capacity; i++)
            fprintf(outGraphFile, "->CELL_%zu", i);

        fprintf(outGraphFile, " [weight = 1000000000, color = " BACK_GROUND_COLOR "];\n");
        
        if (*list->head)
            fprintf(outGraphFile, "ROOT:head->CELL_%zu [style = \"bold\", color = white];\n", *list->head);
        
        if (*list->tail)
            fprintf(outGraphFile, "ROOT:tail->CELL_%zu [style = \"bold\", color = white];\n", *list->tail);

        if (*list->head != *list->tail)
        {
            fprintf(outGraphFile, "CELL_%zu", *list->head);
            size_t index = list->next[*list->head];
            while (index != 0)
            {
                fprintf(outGraphFile, "->CELL_%zu", index);
                index = list->next[index];
            }
            fprintf(outGraphFile, "[style = \"bold\", color = white];\n");
        }

        if (list->freeHead)
            fprintf(outGraphFile, "FREE_HEAD:freeHead->CELL_%zu[style = \"bold\", color = white];\n", list->freeHead);

        fprintf(outGraphFile, "}\n");

        fclose(outGraphFile);

        return Error();
    }
    #undef FONT_SIZE
    #undef FONT_NAME
    #undef BACK_GROUND_COLOR
    #undef NODE_COLOR
    #undef NODE_FRAME_COLOR
    #undef ROOT_COLOR
    #undef FREE_HEAD_COLOR
};

} // namespace mlib

#endif

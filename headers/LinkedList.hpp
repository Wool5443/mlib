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

    String<>      sm_logFolder{};
    std::ofstream sm_htmlLogFile{};
public:
    inline size_t Head() const noexcept { return m_next[0]; }
    inline size_t Tail() const noexcept { return m_prev[0]; }
public:
    inline size_t       Length()   const noexcept { return m_data.Length; }
    inline size_t       Capacity() const noexcept { return m_data.GetCapacity(); }
    inline Utils::Error Error()    const noexcept
    {
        if (m_data.Error) return m_data.Error;
        if (m_next.Error) return m_next.Error;
        return m_prev.Error;
    }
private:
    inline void updateLength(size_t newLength)
    {
        m_data.Length = newLength;
        m_next.Length = newLength;
        m_prev.Length = newLength;
    }

    inline Utils::Error realloc(size_t newLength)
    {
        RETURN_ERROR(m_data.Realloc(newLength));
        RETURN_ERROR(m_next.Realloc(newLength));
        return m_prev.Realloc(newLength);
    }
public:
    LinkedList()
        : LinkedList(0) {}

    LinkedList(size_t hintLength)
        : m_data(hintLength), m_next(hintLength), m_prev(hintLength),
          m_freeHead(1)
    {
        for (size_t i = 1, end = m_next.GetCapacity() - 1; i < end; i++)
            m_next[i] = i + 1;
        for (size_t i = 1, end = m_prev.GetCapacity(); i < end; i++)
            m_prev[i] = FREE_ELEM;
    }

          T& operator[](size_t index)       & noexcept { return m_data[index]; }
    const T& operator[](size_t index) const & noexcept { return m_data[index]; }
public:
    Utils::Error InsertAfter(T value, size_t index)
    {
        if (index >= m_data.GetCapacity())
            return CREATE_ERROR(Utils::ErrorCode::ERROR_INDEX_OUT_OF_BOUNDS);
        if (m_prev[index] == FREE_ELEM)
            return CREATE_ERROR(Utils::ErrorCode::ERROR_INDEX_OUT_OF_BOUNDS);

        realloc(Length() + 1);

        size_t insertIndex    = m_freeHead;
        m_freeHead            = m_next[m_freeHead];

        m_data[insertIndex]   = value;

        m_prev[insertIndex]   = index;
        m_next[insertIndex]   = m_next[index];

        m_prev[m_next[index]] = insertIndex;
        m_next[index]         = insertIndex;

        return Error();
    }

    Utils::Error InsertBefore(T value, size_t index) noexcept
    {
        return InsertAfter(value, m_prev[index]);
    }

    Utils::Error PushBack(T value) noexcept
    {
        return InsertAfter(value, Tail());
    }

    Utils::Error PushFront(T value) noexcept
    {
        return InsertBefore(value, Head());
    }

    Utils::Result<T> Pop(size_t index) noexcept
    {
        if (index < 1 || index >= m_data.GetCapacity() ||
            m_prev[index] == FREE_ELEM)
            return { {}, CREATE_ERROR(Utils::ErrorCode::ERROR_INDEX_OUT_OF_BOUNDS) };

        const T& value = m_data[index];

        m_next[m_prev[index]] = m_next[index];
        m_prev[m_next[index]] = m_prev[index];

        m_prev[index] = FREE_ELEM;
        m_next[index] = m_freeHead;
        m_freeHead    = index;

        return { value, Error() };
    }

    Utils::Result<T> Pop() noexcept
    {
        Pop(Tail());
    }

    Utils::Result<T> GetValueByItsOrderInTheList(size_t index)
    {
        if (index < 1 || index >= m_data.GetCapacity() ||
            m_prev[index] == FREE_ELEM)
            return { FREE_ELEM, CREATE_ERROR(Utils::ErrorCode::ERROR_INDEX_OUT_OF_BOUNDS) };

        ERR_DUMP_RET_RES(this, 0);
        
        size_t curEl = Head();
        size_t i = 1;

        while (i < index && curEl)
        {
            curEl = m_next[curEl];
            i++;
        }

        return { curEl, curEl ? Error() : CREATE_ERROR(Utils::ErrorCode::ERROR_NOT_FOUND) };
    }
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
        sm_htmlLogFile << "</div>\n</body>\n";
        sm_htmlLogFile.close();
    }

    #define FONT_SIZE "10"
    #define FONT_NAME "\"Fira Code Bold\""
    #define BACK_GROUND_COLOR "\"#de97d4\""
    #define TREE_COLOR "\"#ff7be9\""
    #define NODE_COLOR "\"#fae1f6\""
    #define NODE_FRAME_COLOR "\"#000000\""
    #define ROOT_COLOR "\"#c95b90\""
    #define FREE_HEAD_COLOR "\"#b9e793\""
    
    constexpr static size_t FREE_ELEM = Utils::SIZET_POISON;

    Utils::Error Dump() noexcept
    {
        static const size_t NUM_STR_MAX_LENGTH = 21;
        static size_t       dumpIteration      = 0;

        char iterString[NUM_STR_MAX_LENGTH] = "";
        sprintf(iterString, "%zu", dumpIteration);

        Utils::Error error = Error();

        String outTextPath = sm_logFolder;
        outTextPath += "/txt/iter";
        outTextPath += iterString;
        outTextPath += ".txt";

        String outGraphPath = sm_logFolder;
        outGraphPath += "/dot/iter";
        outGraphPath += iterString;
        outGraphPath += ".dot";

        sm_htmlLogFile << "<h1>Iteration" << iterString << "</h1>\n<pre>\n";

        RETURN_ERROR(dumpListText(outTextPath, error));

        sm_htmlLogFile << "</pre>\n";

        RETURN_ERROR(dumpListGraph(outGraphPath));

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
        PRINT_LOG("    length = " << Length() << "\n");
        PRINT_LOG("    capacity = " << m_data.GetCapacity() << "\n");
        PRINT_LOG("    head = " << Head() << "\n");
        PRINT_LOG("    tail = " << Tail() << "\n");
        PRINT_LOG("    free head = " << m_freeHead << "\n");
        PRINT_LOG("    list:\n");
        {
            size_t curEl = Head();
            size_t index = 1;
            while (curEl != 0 && index <= Length() * 2)
            {
                PRINT_LOG("    *[" << index << "] = " << m_data[curEl] << "\n");
                curEl = m_next[curEl];
                index++;
            }
        }

        PRINT_LOG("\n    data[" << m_data.RawPtr() << "]\n");
        for (size_t i = 0, end = m_data.GetCapacity(); i < end; i++)
        {
            PRINT_LOG("    *[" << i << "] = " << m_data[i] << "\n");
        }

        PRINT_LOG("\n    prev[" << m_prev.RawPtr() << "]\n");
        for (size_t i = 0, end = m_prev.GetCapacity(); i < end; i++)
        {
            if (m_prev[i] != FREE_ELEM)
                PRINT_LOG("    *[" << i << "] = " << m_prev[i] << "\n");
            else
                PRINT_LOG("     [" << i << "] = FREE\n");
        }

        PRINT_LOG("\n    next[" << m_next.RawPtr() << "]\n");
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

        size_t head = Head(), tail = Tail();

        outGraphFile <<
        "digraph\n"
        "{\n"
        "rankdir = LR;\n"
        "node[shape = record, color = " NODE_FRAME_COLOR ", fontname = " FONT_NAME ", fontsize = " FONT_SIZE "];\n"
        "bgcolor = " BACK_GROUND_COLOR ";\n"
        "ROOT[style = \"filled\", fillcolor = " ROOT_COLOR ", "
        "label = \"ROOT|{<head>head = " << head << "|<tail>tail = " <<
        tail << "}\"];\n"
        "FREE_HEAD[style = \"filled\", fillcolor = " FREE_HEAD_COLOR ", "
        "label = \"FREE HEAD|<freeHead>freeHead = " << m_freeHead << "\"];\n";
        
        for (size_t i = 1, end = m_data.GetCapacity(); i < end; i++)
        {
            outGraphFile <<
            "CELL_" << i << "[style = \"filled\", fillcolor = " NODE_COLOR ", ";
            outGraphFile << "label = \"index = " << i << "|";

            outGraphFile << "value\\n" << m_data[i] << "|";

            if (m_prev[i] == FREE_ELEM)
                outGraphFile << "{prev = FREE|";
            else
                outGraphFile << "{prev = " << m_prev[i] << "|";

            if (m_next[i] == FREE_ELEM)
                outGraphFile << "next = BAD}\"];\n";
            else
                outGraphFile << "next = " << m_next[i] << "}\"];\n";
        }

        outGraphFile << "ROOT->CELL_1";
        for (size_t i = 2, end = m_data.GetCapacity(); i < end; i++)
            outGraphFile << "->CELL_" << i;

        outGraphFile << " [weight = 1000000000, color = " BACK_GROUND_COLOR "];\n";
        
        if (head)
            outGraphFile << "ROOT:head->CELL_" << head << " [style = \"bold\", color = white];\n";
        
        if (tail)
            outGraphFile << "ROOT:tail->CELL_" << tail << " [style = \"bold\", color = white];\n";

        if (head != tail)
        {
            outGraphFile << "CELL_" << head;
            size_t index = m_next[head];
            while (index)
            {
                outGraphFile << "->CELL_" << index;
                index = m_next[index];
            }
            outGraphFile << "[style = \"bold\", color = white];\n";
        }

        if (m_freeHead)
            outGraphFile << "FREE_HEAD:freeHead->CELL_" << m_freeHead << "[style = \"bold\", color = white];\n";

        outGraphFile << "}\n";

        outGraphFile.close();

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

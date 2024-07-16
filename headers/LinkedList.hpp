#ifndef MLIB_LINKED_LIST_HPP
#define MLIB_LINKED_LIST_HPP

#include <fstream>
#include "GeneralBuffer.hpp"

namespace mlib {

template<typename T, std::size_t DefaultCapacity = 8, std::size_t GrowFactor = 2>
class LinkedList
{
private:
    Buffer<T,      DefaultCapacity, GrowFactor> m_data;
    Buffer<std::size_t, DefaultCapacity, GrowFactor> m_next;
    Buffer<std::size_t, DefaultCapacity, GrowFactor> m_prev;
    std::size_t m_freeHead;

    String<>      m_logFolder{};
    std::ofstream m_htmlLogFile{};
public:
    inline std::size_t Head() const noexcept { return m_next[0]; }
    inline std::size_t Tail() const noexcept { return m_prev[0]; }
public:
    inline std::size_t       Length()   const noexcept { return m_data.Length; }
    inline std::size_t       Capacity() const noexcept { return m_data.GetCapacity(); }
    inline Utils::Error Error()    const noexcept
    {
        if (m_data.Error) return m_data.Error;
        if (m_next.Error) return m_next.Error;
        return m_prev.Error;
    }
public:
    LinkedList()
        : LinkedList(0) {}

    LinkedList(std::size_t hintLength)
        : m_data(hintLength), m_next(hintLength), m_prev(hintLength),
          m_freeHead(1)
    {
        for (std::size_t i = 1, end = m_next.GetCapacity() - 1; i < end; i++)
            m_next[i] = i + 1;
        for (std::size_t i = 1, end = m_prev.GetCapacity(); i < end; i++)
            m_prev[i] = FREE_ELEM;
    }

          T& operator[](std::size_t index)       & noexcept { return m_data[index]; }
    const T& operator[](std::size_t index) const & noexcept { return m_data[index]; }
public:
    Utils::Error InsertAfter(const T& value, std::size_t index)
    {
        if (index >= m_data.GetCapacity())
            return CREATE_ERROR(Utils::ERROR_INDEX_OUT_OF_BOUNDS);
        if (m_prev[index] == FREE_ELEM)
            return CREATE_ERROR(Utils::ERROR_INDEX_OUT_OF_BOUNDS);

        if (!m_freeHead)
            RETURN_ERROR(realloc(Length() + 1));
        else
            updateLength(Length() + 1);

        std::size_t insertIndex    = m_freeHead;
        m_freeHead            = m_next[m_freeHead];

        m_data[insertIndex]   = value;

        m_prev[insertIndex]   = index;
        m_next[insertIndex]   = m_next[index];

        m_prev[m_next[index]] = insertIndex;
        m_next[index]         = insertIndex;

        return Error();
    }

    Utils::Error InsertBefore(const T& value, std::size_t index) noexcept
    {
        return InsertAfter(value, m_prev[index]);
    }

    Utils::Error PushBack(const T& value) noexcept
    {
        return InsertAfter(value, Tail());
    }

    Utils::Error PushFront(const T& value) noexcept
    {
        return InsertBefore(value, Head());
    }

    Utils::Result<T> Pop(std::size_t index) noexcept
    {
        if (index < 1 || index >= m_data.GetCapacity() ||
            m_prev[index] == FREE_ELEM)
            return { {}, CREATE_ERROR(Utils::ERROR_INDEX_OUT_OF_BOUNDS) };

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
        return Pop(Tail());
    }

    Utils::Result<T> GetValueByItsOrderInTheList(std::size_t index)
    {
        if (index < 1 || index >= m_data.GetCapacity() ||
            m_prev[index] == FREE_ELEM)
            return { FREE_ELEM, CREATE_ERROR(Utils::ERROR_INDEX_OUT_OF_BOUNDS) };

        ERR_DUMP_RET_RES(this, 0);
        
        std::size_t curEl = Head();
        std::size_t i = 1;

        while (i < index && curEl)
        {
            curEl = m_next[curEl];
            i++;
        }

        return { curEl, curEl ? Error() : CREATE_ERROR(Utils::ERROR_NOT_FOUND) };
    }

    Utils::Result<size_t> Find(const T& value) const noexcept
    {
        size_t curEl = Head();

        while (curEl && m_data[curEl] != value)
            curEl = m_next[curEl];

        return { curEl, curEl ? Utils::Error() :
                                CREATE_ERROR(Utils::ERROR_NOT_FOUND) };
    }
public:
    void StartLogging(const char* logFolder) noexcept
    {
        HardAssert(logFolder, Utils::ERROR_BAD_FILE);
        m_logFolder = logFolder;

        String htmlFilePath = logFolder;

        htmlFilePath += "/log.html";

        m_htmlLogFile.open(htmlFilePath);

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

    void EndLogging() noexcept
    {
        m_htmlLogFile << "</div>\n</body>\n";
        m_htmlLogFile.close();
    }

    #define FONT_SIZE "10"
    #define FONT_NAME "\"Fira Code Bold\""
    #define BACK_GROUND_COLOR "\"#de97d4\""
    #define TREE_COLOR "\"#ff7be9\""
    #define NODE_COLOR "\"#fae1f6\""
    #define NODE_FRAME_COLOR "\"#000000\""
    #define ROOT_COLOR "\"#c95b90\""
    #define FREE_HEAD_COLOR "\"#b9e793\""
    
    constexpr static std::size_t FREE_ELEM = Utils::SIZET_POISON;

    Utils::Error Dump() noexcept
    {
        static const std::size_t NUM_STR_MAX_LENGTH = 21;
        static std::size_t       dumpIteration      = 0;

        char iterString[NUM_STR_MAX_LENGTH] = "";
        sprintf(iterString, "%zu", dumpIteration);

        Utils::Error error = Error();

        String outTextPath = m_logFolder;
        outTextPath += "/txt/iter";
        outTextPath += iterString;
        outTextPath += ".txt";

        String outGraphPath = m_logFolder;
        outGraphPath += "/dot/iter";
        outGraphPath += iterString;
        outGraphPath += ".dot";

        m_htmlLogFile << "<h1>Iteration" << iterString << "</h1>\n<pre>\n";

        RETURN_ERROR(dumpListText(outTextPath, error));

        m_htmlLogFile << "</pre>\n";

        RETURN_ERROR(dumpListGraph(outGraphPath));

        String outImgPath = m_logFolder;
        outImgPath += "/img/iter";
        outImgPath += iterString;
        outImgPath += ".png";

        String command = "dot " + outGraphPath + " -T png -o" + outImgPath;

        system(command);

        m_htmlLogFile << "<img src = \"" << outImgPath << "\"/>\n";

        dumpIteration++;

        return Error();
    }
private:
    inline void updateLength(std::size_t newLength)
    {
        m_data.Length = newLength;
        m_next.Length = newLength;
        m_prev.Length = newLength;
    }

    inline Utils::Error realloc(std::size_t newLength)
    {
        std::size_t oldCapacity = Capacity();

        RETURN_ERROR(m_data.Realloc(newLength));
        RETURN_ERROR(m_next.Realloc(newLength));
        RETURN_ERROR(m_prev.Realloc(newLength));

        std::size_t newCapcity = Capacity();

        for (std::size_t i = oldCapacity; i < newCapcity - 1; i++)
            m_next[i] = i + 1;
        for (std::size_t i = oldCapacity; i < newCapcity; i++)
            m_prev[i] = FREE_ELEM;

        m_freeHead = oldCapacity;

        return Utils::Error();
    }
private:
    #define PRINT_LOG(...)                  \
    do                                      \
    {                                       \
        outTextFile    << __VA_ARGS__;      \
        m_htmlLogFile << __VA_ARGS__;       \
    } while (0)

    Utils::Error dumpListText(const String<>& outTextPath, Utils::Error error)
    {
        std::ofstream outTextFile{outTextPath};
        if (!outTextFile)
            return CREATE_ERROR(Utils::ERROR_BAD_FILE);

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
            std::size_t curEl = Head();
            std::size_t index = 1;
            while (curEl != 0 && index <= Length() * 2)
            {
                PRINT_LOG("    *[" << index << "] = " << m_data[curEl] << "\n");
                curEl = m_next[curEl];
                index++;
            }
        }

        PRINT_LOG("\n    data[" << m_data.RawPtr() << "]\n");
        for (std::size_t i = 0, end = m_data.GetCapacity(); i < end; i++)
        {
            PRINT_LOG("    *[" << i << "] = " << m_data[i] << "\n");
        }

        PRINT_LOG("\n    prev[" << m_prev.RawPtr() << "]\n");
        for (std::size_t i = 0, end = m_prev.GetCapacity(); i < end; i++)
        {
            if (m_prev[i] != FREE_ELEM)
                PRINT_LOG("    *[" << i << "] = " << m_prev[i] << "\n");
            else
                PRINT_LOG("     [" << i << "] = FREE\n");
        }

        PRINT_LOG("\n    next[" << m_next.RawPtr() << "]\n");
        for (std::size_t i = 0, end = m_next.GetCapacity(); i < end; i++)
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
            return CREATE_ERROR(Utils::ERROR_BAD_FILE);

        std::size_t head = Head(), tail = Tail();

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
        
        for (std::size_t i = 1, end = m_data.GetCapacity(); i < end; i++)
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
        for (std::size_t i = 2, end = m_data.GetCapacity(); i < end; i++)
            outGraphFile << "->CELL_" << i;

        outGraphFile << " [weight = 1000000000, color = " BACK_GROUND_COLOR "];\n";
        
        if (head)
            outGraphFile << "ROOT:head->CELL_" << head << " [style = \"bold\", color = white];\n";
        
        if (tail)
            outGraphFile << "ROOT:tail->CELL_" << tail << " [style = \"bold\", color = white];\n";

        if (head != tail)
        {
            outGraphFile << "CELL_" << head;
            std::size_t index = m_next[head];
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

#ifndef MLIB_LINKED_LIST_HPP
#define MLIB_LINKED_LIST_HPP

/**
 * @file LinkedList.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief This file contains an implementation of a
 * templated LinkedList
 *
 * Elements are stored in an array, thus, this
 * list is very cache-friendly
 *
 * @version 2.0
 * @date 20-07-2024
 *
 * @copyright Copyright (c) 2024
 */

#include <cstdint>
#include <fstream>
#include "GeneralBuffer.hpp"

namespace mlib {

/** @class LinkedList
 * @brief LinkedList class using arrays
 *
 * @tparam T value type
 * @tparam DefaultCapacity
 * @tparam GrowFactor
 */
template<typename T, std::size_t DefaultCapacity = 8, std::size_t GrowFactor = 2>
class LinkedList final
{
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
private:
    Buffer<T>           m_data{1};
    Buffer<std::size_t> m_next{1};
    Buffer<std::size_t> m_prev{1};

    str           m_dumpFolder{};
    std::ofstream m_htmlDumpFile{};

    std::size_t   m_freeHead = 0;
public:
    std::size_t   length = 1; ///< length
                              ///< Notice that there is always a fictional
                              ///< element
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Return the head
     *
     * @return std::size_t index of the head
     */
     std::size_t  Head()    const noexcept { return m_next[0]; }
    /**
     * @brief Return the tail
     *
     * @return std::size_t index of the tail
     */
     std::size_t  Tail()    const noexcept { return m_prev[0]; }

    /**
     * @brief Return error of the underlying buffers
     *
     * @return err::ErrorCode error
     */
     err::ErrorCode Error() const noexcept
    {
        if (m_data.error) return m_data.error;
        if (m_next.error) return m_next.error;
        return m_prev.error;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a new Linked List object
     */
    LinkedList() noexcept = default;

    /**
     * @brief Construct a new Linked List object
     * and ensures that the capacity is enougth
     * for capacity elements, thus, avoiding
     * reallocations
     *
     * @param [in] capacity length to ensure big enough capacity
     */
    explicit LinkedList(std::size_t capacity) noexcept
        : m_data(capacity), m_next(capacity), m_prev(capacity),
          m_freeHead(1)
    {
        if (auto err = Error())
        {
            LOG_ERROR(err);
            return;
        }

        for (std::size_t i = 1, end = m_next.GetCapacity() - 1; i < end; i++)
            m_next[i] = i + 1;
        for (std::size_t i = 1, end = m_prev.GetCapacity(); i < end; i++)
            m_prev[i] = FREE_ELEM;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Inserts an element after index
     *
     * @param [in] value
     * @param [in] index
     *
     * @return err::ErrorCode
     */
    err::ErrorCode InsertAfter(T&& value, std::size_t index)
    {
        if (index >= m_data.GetCapacity())
            RETURN_ERROR(err::ERROR_INDEX_OUT_OF_BOUNDS);
        if (m_prev[index] == FREE_ELEM)
            RETURN_ERROR(err::ERROR_INDEX_OUT_OF_BOUNDS);

        if (!m_freeHead)
            RETURN_ERROR(realloc(length + 1));

        std::size_t insertIndex = m_freeHead;
        m_freeHead              = m_next[m_freeHead];

        m_data[insertIndex]     = std::move(value);

        m_prev[insertIndex]     = index;
        m_next[insertIndex]     = m_next[index];

        m_prev[m_next[index]]   = insertIndex;
        m_next[index]           = insertIndex;

        length++;

        return {};
    }

    /**
     * @brief Inserts an element after index
     *
     * @param [in] value
     * @param [in] index
     *
     * @return err::ErrorCode
     */
    err::ErrorCode InsertAfter(const T& value, std::size_t index)
    {
        T val = value;
        return InsertAfter(std::move(val), index);
    }

    /**
     * @brief Inserts an element before index
     *
     * @param [in] value
     * @param [in] index
     *
     * @return err::ErrorCode
     */
    err::ErrorCode InsertBefore(const T& value, std::size_t index) noexcept
    {
        return InsertAfter(value, m_prev[index]);
    }

    /**
     * @brief Inserts an element before index
     *
     * @param [in] value
     * @param [in] index
     *
     * @return err::ErrorCode
     */
    err::ErrorCode InsertBefore(T&& value, std::size_t index) noexcept
    {
        return InsertAfter(std::move(value), m_prev[index]);
    }

    /**
     * @brief Inserts an element at the end
     *
     * @param [in] value
     *
     * @return err::ErrorCode
     */
    err::ErrorCode PushBack(const T& value) noexcept
    {
        return InsertAfter(value, Tail());
    }

    /**
     * @brief Inserts an element at the end
     *
     * @param [in] value
     *
     * @return err::ErrorCode
     */
    err::ErrorCode PushBack(T&& value) noexcept
    {
        return InsertAfter(std::move(value), Tail());
    }

    /**
     * @brief Inserts an element at the front
     *
     * @param [in] value
     *
     * @return err::ErrorCode
     */
    err::ErrorCode PushFront(const T& value) noexcept
    {
        return InsertBefore(value, Head());
    }

    /**
     * @brief Inserts an element at the front
     *
     * @param [in] value
     *
     * @return err::ErrorCode
     */
    err::ErrorCode PushFront(T&& value) noexcept
    {
        return InsertBefore(std::move(value), Head());
    }

    /**
     * @brief Pops an element at index
     *
     * @param [in] index where to pop
     *
     * @return err::Result<T> value result
     */
    err::Result<T> Pop(std::size_t index) noexcept
    {
        RETURN_ERROR_RESULT(Error(), {}, T);

        if (index < 1 || index >= m_data.GetCapacity() ||
            m_prev[index] == FREE_ELEM)
            RETURN_ERROR_RESULT(err::ERROR_INDEX_OUT_OF_BOUNDS, {}, T);

        T value = std::move(m_data[index]);

        m_next[m_prev[index]] = m_next[index];
        m_prev[m_next[index]] = m_prev[index];

        m_prev[index] = FREE_ELEM;
        m_next[index] = m_freeHead;
        m_freeHead    = index;

        return value;
    }

    /**
     * @brief Pops an element at the end
     *
     * @return err::Result<T> value result
     */
    err::Result<T> PopBack() noexcept
    {
        return Pop(Tail());
    }

    /**
     * @brief Pops an element at the front
     *
     * @return err::Result<T> value result
     */
    err::Result<T> PopFront() noexcept
    {
        return PopFront(Head());
    }

    /**
     * @brief Slowly iterates over the list until it gets
     * to the element you want(don't use it)
     *
     * @param [in] index index if we untangle the list
     *
     * @return err::Result<std::size_t> correct index
     */
    err::Result<std::size_t> GetValueByItsOrderInTheList(std::size_t index)
    const noexcept
    {
        if (index < 1 || index >= m_data.GetCapacity() ||
            m_prev[index] == FREE_ELEM)
            RETURN_ERROR_RESULT(err::ERROR_INDEX_OUT_OF_BOUNDS, SIZE_MAX, std::size_t);

        std::size_t curEl = Head();
        std::size_t i     = 1;

        while (i < index && curEl)
        {
            curEl = m_next[curEl];
            i++;
        }

        if (!curEl)
            RETURN_ERROR_RESULT(err::ERROR_NOT_FOUND, SIZE_MAX, std::size_t);

        return curEl;
    }

    /**
     * @brief Finds an element in the list
     *
     * @param [in] value
     *
     * @return err::Result<size_t> index result
     */
    err::Result<size_t> Find(const T& value) const noexcept
    {
        size_t curEl = Head();

        while (curEl && m_data[curEl] != value)
            curEl = m_next[curEl];

        if (!curEl)
            RETURN_ERROR_RESULT(err::ERROR_NOT_FOUND, SIZE_MAX, std::size_t);

        return curEl;
    }

    /**
     * @brief Knowing pointer to an element return its index
     * in the list
     *
     * @param [in] elem
     *
     * @return std::size_t
     */
    std::size_t GetIndexFromPointer(const T* elem) const noexcept
    {
        return elem - m_data.RawPtr();
    }
public:
    /**
     * @brief Initializes dump files and writes
     * the header of the html log file
     *
     * @param [in] logFolder where to put logs
     *
     * @return err::ErrorCode
     */
    err::ErrorCode InitDump(const char* logFolder) noexcept
    {
        HardAssert(logFolder, err::ERROR_BAD_FILE);
        m_dumpFolder = logFolder;

        String htmlFilePath = m_dumpFolder;
        htmlFilePath += "/dump.html";

        m_htmlDumpFile.open(htmlFilePath);
        if (!m_htmlDumpFile)
            RETURN_ERROR(err::ERROR_BAD_FILE);

        m_htmlDumpFile <<
            "<style>\n"
            ".content {\n"
            "max-width: 500px;\n"
            "margin: auto;\n"
            "}\n"
            "</style>,\n"
            "<body>\n"
            "<div class=\"content\">";

        return err::EVERYTHING_FINE;
    }

    /**
     * @brief Ends dumping
     */
    void FinishDump() noexcept
    {
        m_htmlDumpFile << "</div>\n</body>\n";
        m_htmlDumpFile.close();
    }

    #define FONT_SIZE "10"
    #define FONT_NAME "\"Fira Code Bold\""
    #define BACK_GROUND_COLOR "\"#de97d4\""
    #define TREE_COLOR "\"#ff7be9\""
    #define NODE_COLOR "\"#fae1f6\""
    #define NODE_FRAME_COLOR "\"#000000\""
    #define ROOT_COLOR "\"#c95b90\""
    #define FREE_HEAD_COLOR "\"#b9e793\""

    static const std::size_t FREE_ELEM = SIZE_MAX;

    /**
     * @brief Call only after InitDump
     * Dumps the list
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Dump() noexcept
    {
        static std::size_t       dumpIteration = 0;

        static const std::size_t numStrMaxLength = 21;
        char iterString[numStrMaxLength] = "";
        sprintf(iterString, "%zu", dumpIteration);

        err::ErrorCode error = Error();

        String outTextPath = m_dumpFolder;
        outTextPath += "/txt/iter";
        outTextPath += iterString;
        outTextPath += ".txt";

        String outGraphPath = m_dumpFolder;
        outGraphPath += "/dot/iter";
        outGraphPath += iterString;
        outGraphPath += ".dot";

        m_htmlDumpFile << "<h1>Iteration" << iterString << "</h1>\n<pre>\n";

        RETURN_ERROR(dumpListText(outTextPath, error));

        m_htmlDumpFile << "</pre>\n";

        RETURN_ERROR(dumpListGraph(outGraphPath));

        String outImgPath = m_dumpFolder;
        outImgPath += "/img/iter";
        outImgPath += iterString;
        outImgPath += ".png";

        String command = "dot " + outGraphPath + " -T png -o" + outImgPath;

        system(command);

        static const std::size_t maxPathLength = 256;
        char absoluteOutImgPath[maxPathLength];
        if (!realpath(outImgPath, absoluteOutImgPath))
            RETURN_ERROR(err::ERROR_BAD_FILE);

        m_htmlDumpFile << "<img src = \"" << absoluteOutImgPath << "\"/>\n";

        dumpIteration++;

        return err::EVERYTHING_FINE;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              INDEXING AND ITERATORS
//
///////////////////////////////////////////////////////////////////////////////
private:
    template<typename ListType>
    struct iteratorBase
    {
        ListType    l;
        std::size_t i;

        iteratorBase& operator++()
        {
            i = l.m_next[i];
            return *this;
        }

        iteratorBase& operator--()
        {
            i = l.m_prev[i];
            return *this;
        }

        iteratorBase operator++(int dummy)
        {
            iteratorBase copy = *this;
            i                 = l.m_next[i];
            return copy;
        }

        iteratorBase operator--(int dummy)
        {
            iteratorBase copy = *this;
            i                 = l.m_prev[i];
            return copy;
        }

        bool operator==(const iteratorBase& other) const noexcept
        {
            return &l == &other.l && i == other.i;
        }

        bool operator!=(const iteratorBase& other) const noexcept
        {
            return !(*this == other);
        }
    };
public:
    struct iterator: public iteratorBase<LinkedList&>
    {
        T& operator*()
        {
            return this->l.m_data[this->i];
        }
    };

    struct constIterator : public iteratorBase<const LinkedList&>
    {
        const T& operator*()
        {
            return this->l.m_data[this->i];
        }
    };

          T& operator[](std::size_t index)       & noexcept { return m_data[index]; }
    const T& operator[](std::size_t index) const & noexcept { return m_data[index]; }

    /**
     * @brief Returns the start of a list
     *
     * @return iterator
     */
    iterator      begin()        & noexcept { return { *this, Head() }; }

    /**
     * @brief Returns the start of a const list
     *
     * @return constIterator
     */
    constIterator begin()  const & noexcept { return { *this, Head() }; }

    /**
     * @brief Returns the start of a const list
     *
     * @return constIterator
     */
    constIterator cbegin() const & noexcept { return { *this, Head() }; }

    /**
     * @brief Returns the end of a list
     *
     * @return iterator
     */
    iterator      end()          & noexcept { return { *this, 0 }; }

    /**
     * @brief Returns the end of a const list
     *
     * @return constIterator
     */
    constIterator end()    const & noexcept { return { *this, 0 }; }

    /**
     * @brief Returns the end of a const list
     *
     * @return constIterator
     */
    constIterator cend()   const & noexcept { return { *this, 0 }; }
private:
    err::ErrorCode realloc(std::size_t newLength)
    {
        std::size_t oldCapacity = m_data.GetCapacity();

        RETURN_ERROR(m_data.Realloc(newLength));
        RETURN_ERROR(m_next.Realloc(newLength));
        RETURN_ERROR(m_prev.Realloc(newLength));

        std::size_t newCapcity = m_data.GetCapacity();

        for (std::size_t i = oldCapacity; i < newCapcity - 1; i++)
            m_next[i] = i + 1;
        for (std::size_t i = oldCapacity; i < newCapcity; i++)
            m_prev[i] = FREE_ELEM;

        m_freeHead = oldCapacity;

        return err::EVERYTHING_FINE;
    }

    #define PRINT_DUMP(...)                 \
    do                                      \
    {                                       \
        outTextFile    << __VA_ARGS__;      \
        m_htmlDumpFile << __VA_ARGS__;      \
    } while (0)

    err::ErrorCode dumpListText(const String& outTextPath, err::ErrorCode error)
    {
        std::ofstream outTextFile{outTextPath};
        if (!outTextFile)
            RETURN_ERROR(err::ERROR_BAD_FILE);

        PRINT_DUMP("List[" << this << "]\n");
        PRINT_DUMP("List condition - " << GetErrorName(error) << "[" <<
                   static_cast<int>(error) << "]\n");

        PRINT_DUMP("{\n");
        PRINT_DUMP("    length = " << length << "\n");
        PRINT_DUMP("    capacity = " << m_data.GetCapacity() << "\n");
        PRINT_DUMP("    head = " << Head() << "\n");
        PRINT_DUMP("    tail = " << Tail() << "\n");
        PRINT_DUMP("    free head = " << m_freeHead << "\n");
        PRINT_DUMP("    list:\n");
        {
            std::size_t curEl = Head();
            std::size_t index = 1;
            while (curEl != 0 && index <= length * 2)
            {
                PRINT_DUMP("    *[" << index << "] = " << m_data[curEl] << "\n");
                curEl = m_next[curEl];
                index++;
            }
        }

        PRINT_DUMP("\n    data[" << m_data.RawPtr() << "]\n");
        for (std::size_t i = 0, end = m_data.GetCapacity(); i < end; i++)
        {
            PRINT_DUMP("    *[" << i << "] = " << m_data[i] << "\n");
        }

        PRINT_DUMP("\n    prev[" << m_prev.RawPtr() << "]\n");
        for (std::size_t i = 0, end = m_prev.GetCapacity(); i < end; i++)
        {
            if (m_prev[i] != FREE_ELEM)
                PRINT_DUMP("    *[" << i << "] = " << m_prev[i] << "\n");
            else
                PRINT_DUMP("     [" << i << "] = FREE\n");
        }

        PRINT_DUMP("\n    next[" << m_next.RawPtr() << "]\n");
        for (std::size_t i = 0, end = m_next.GetCapacity(); i < end; i++)
        {
            if (m_next[i] != FREE_ELEM)
                PRINT_DUMP("    *[" << i << "] = " << m_prev[i] << "\n");
            else
                PRINT_DUMP("     [" << i << "] = BAD\n");
        }

        PRINT_DUMP("}\n");

        return Error();
    }

    #undef PRINT_DUMP

    err::ErrorCode dumpListGraph(const String& outGraphPath)
    {
        std::ofstream outGraphFile{outGraphPath};
        if (!outGraphFile)
            RETURN_ERROR(err::ERROR_BAD_FILE);

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

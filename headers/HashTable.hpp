#ifndef MLIB_HASHTABLE_HPP
#define MLIB_HASHTABLE_HPP

#include <ostream>

#include "Utils.hpp"
#include "Hash.hpp"

namespace mlib {

template<typename Key, typename Val, typename GetHash = Hash<Key>>

class HashTable
{
public:
    struct HashTableElement
    {
        Key key;
        Val val;

        friend std::ostream& operator<<(std::ostream& out,
                                        const HashTableElement& el) noexcept
        {
            return out << "{ " << el.key << ": " <<
                   el.val << " }";
        }

        bool operator==(const HashTableElement& other) const noexcept
        {
            return key == other.key;
        }

        bool operator!=(const HashTableElement& other) const noexcept
        {
            return key != other.key;
        }
    };

    using Container = LinkedList<HashTableElement>;
///////////////////////////////////////////////////////////////////////////////
//
//                              FIELDS
//
///////////////////////////////////////////////////////////////////////////////
private:
    Vector<Container> m_containers{};
    std::size_t       m_length = 0;
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Get error
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Error() const noexcept
    {
        return m_containers.Error();
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR AND =
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an empty Hash Table
     */
    HashTable() noexcept = default;

    /**
     * @brief Construct a Hash Table of some size
     *
     * @param [in] size
     */
    HashTable(std::size_t size)
        : m_containers(size)
    {
        if (auto error = Error())
            LOG_ERROR(error);
        m_containers.length = size;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Create a Hash Table of some size
     *
     * @param [in] size
     * @return err::Result<HashTable>
     */
    static err::Result<HashTable> New(std::size_t size) noexcept
    {
        HashTable tab(size);
        LOG_ERROR_IF(tab.Error());
        return { tab, tab.Error() };
    }

    /**
     * @brief Copy a Hash Table
     *
     * @param [in] other
     * @return err::Result<HashTable>
     */
    static err::Result<HashTable> New(const HashTable& other) noexcept
    {
        HashTable tab(other);
        LOG_ERROR_IF(tab.Error());
        return { tab, tab.Error() };
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Add a key-value pair to the table if it is not there yet
     *
     * @param keyVal key-value pair
     * @return err::ErrorCode
     */
    err::ErrorCode Add(HashTableElement&& keyVal)
    {
        RETURN_ERROR(realloc());

        std::size_t index = getIndex(keyVal.key);
        RETURN_ERROR(m_containers[index].Error());

        auto found = findInContainer(m_containers[index], keyVal.key);

        if (found)
            return found;

        err::ErrorCode error = m_containers[index].PushBack(std::move(keyVal));
        RETURN_ERROR(error);

        m_length++;

        return err::EVERYTHING_FINE;
    }

    /**
     * @brief Remove a table entry and return its value
     *
     * @param [in] key
     *
     * @return err::Result<Val>
     */
    err::Result<Val> Pop(const Key& key)
    {
        RETURN_ERROR_RESULT(Error(), {});

        std::size_t index = getIndex(key);
        RETURN_ERROR_RESULT(m_containers[index].Error(), {});

        auto& container = m_containers[index];

        auto found = findInContainer(container, key);

        RETURN_ERROR_RESULT(found, {});

        err::Result<HashTableElement> result = container.Pop(found.value);

        LOG_ERROR_IF(result);

        return { result.value.val, result.error };
    }

    /**
     * @brief Remove a key from the table
     *
     * @param key
     * @return err::ErrorCode
     */
    err::ErrorCode Remove(const Key& key)
    {
        return Pop(key).error;
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              INDEXING AND ITERATORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    Val* operator[](const Key& key) & noexcept
    {
        if (auto error = Error())
        {
            if (error == err::ERROR_UNINITIALIZED)
                return nullptr;
            LOG_ERROR(error);
            return nullptr;
        }

        std::size_t index = getIndex(key);
        if (auto error = m_containers[index].Error())
        {
            LOG_ERROR(error);
            return nullptr;
        }

        auto found = findInContainer(m_containers[index], key);

        if (!found)
            return nullptr;

        return &m_containers[index][found.value].val;
    }

    const Val* operator[](const Key& key) const & noexcept
    {
        return (*this)[key];
    }
private:
    err::Result<std::size_t>
    findInContainer(const Container& container, const Key& key)
    const noexcept
    {
        for (const auto& keyVal : container)
            if (keyVal.key == key)
                return container.GetIndexFromPointer(&keyVal);
        return err::ERROR_NOT_FOUND;
    }
private:
    uint64_t getIndex(const Key& key)
    {
        HardAssert(m_containers.length > 0, err::ERROR_ZERO_DIVISION);

        return GetHash()(key) % m_containers.length;
    }

    err::ErrorCode realloc()
    {
        static const double whenToResizeFactor   = 1.0 / 3.0;
        static const std::size_t growNumerator   = 3;
        static const std::size_t growDenominator = 2;
        static const std::size_t minCapacity     = 9;

        std::size_t capacity = m_containers.length;

        if (capacity
            && static_cast<double>(m_length) / capacity
            < whenToResizeFactor)
            return err::EVERYTHING_FINE;

        std::size_t newCapacity = std::max(
            minCapacity,
            capacity * growNumerator / growDenominator
        );

        HashTable newTable{newCapacity};

        for (auto& container : m_containers)
            for (HashTableElement& elem : container)
                RETURN_ERROR(newTable.Add(std::move(elem)));

        *this = std::move(newTable);

        return err::EVERYTHING_FINE;
    }
};

} // namespace mlib

#endif

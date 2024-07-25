#ifndef MLIB_HASHTABLE_HPP
#define MLIB_HASHTABLE_HPP

#include <ostream>

#include "Utils.hpp"
#include "Hash.hpp"

namespace mlib {

template<typename Key, typename Val, typename GetHash = Hash<Key>>

class HashTable
{
    using HashKey = Utils::Pair<HashType, Key>;
public:
    struct HashTableElement
    {
        HashKey hashKey{};
        Val     val{};

        HashTableElement() noexcept = default;

        HashTableElement(Key&& key, Val&& val) noexcept
            : hashKey(GetHash()(key), std::move(key)),
              val(std::move(val)) {}

        friend std::ostream& operator<<(std::ostream& out,
                                        const HashTableElement& el) noexcept
        {
            return out << "{ " << el.key << ": " <<
                   el.val << " }";
        }

        bool operator==(const HashTableElement& other) const noexcept
        {
            return hashKey.hash == other.hashKey.hash
                && hashKey.key  == other.hashKey.key;
        }

        bool operator!=(const HashTableElement& other) const noexcept
        {
            return hashKey.hash != other.hashKey.hash
                || hashKey.key  != other.hashKey.key;
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
     * @param [in] key
     * @param [in] value
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Add(Key&& key, Val&& value)
    {
        return Add(HashTableElement(std::move(key), std::move(value)));
    }

    /**
     * @brief Add a HashTableElement to the table if it is not there yet
     *
     * @param [in] elem
     *
     * @return err::ErrorCode
     */
    err::ErrorCode Add(HashTableElement&& elem)
    {
        RETURN_ERROR(realloc());

        HardAssert(m_containers.length, err::ERROR_ZERO_DIVISION);

        std::size_t index = elem.hashKey.val1 % m_containers.length;
        RETURN_ERROR(m_containers[index].Error());

        auto found = findInContainer(m_containers[index], elem.hashKey);

        if (found)
            return found;

        err::ErrorCode error = m_containers[index].PushBack(std::move(elem));
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

        Utils::Pair<HashType, std::size_t> hashInd = getHashInd(key);
        RETURN_ERROR_RESULT(m_containers[hashInd.val2].Error(), {});

        auto& container = m_containers[hashInd.val2];

        auto found = findInContainer(container, { hashInd.val1, key });

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

        Utils::Pair<HashType, std::size_t> hashInd = getHashInd(key);
        if (auto error = m_containers[hashInd.val2].Error())
        {
            LOG_ERROR(error);
            return nullptr;
        }

        auto found = findInContainer(m_containers[hashInd.val2],
                                     { hashInd.val1, key });

        if (!found)
            return nullptr;

        return &m_containers[hashInd.val2][found.value].val;
    }

    const Val* operator[](const Key& key) const & noexcept
    {
        return (*this)[key];
    }
private:
    err::Result<std::size_t>
    findInContainer(const Container& container, const HashKey& hashKey)
    const noexcept
    {
        for (const auto& tableEnty : container)
            if (tableEnty.hashKey.val1 == hashKey.val1
             && tableEnty.hashKey.val2 == hashKey.val2)
                return container.GetIndexFromPointer(&tableEnty);
        return err::ERROR_NOT_FOUND;
    }
private:
    Utils::Pair<HashType, std::size_t> getHashInd(const Key& key)
    {
        HardAssert(m_containers.length > 0, err::ERROR_ZERO_DIVISION);

        HashType hash = GetHash()(key);

        return { hash, hash % m_containers.length };
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
            {
                std::size_t index = elem.hashKey.val1 % newCapacity;
                RETURN_ERROR(newTable.m_containers[index].PushBack(std::move(elem)));
            }

        *this = std::move(newTable);

        return err::EVERYTHING_FINE;
    }
};

} // namespace mlib

#endif

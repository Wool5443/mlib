#ifndef MLIB_HASHTABLE_HPP
#define MLIB_HASHTABLE_HPP

#include <ostream>

#include "Hash.hpp"

namespace mlib {

template<typename Key, typename Val, std::size_t Size = 8,
         typename Hash = Hash<Key>>

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
    Container m_containers[Size]{};
    String    m_dumpFolder{};
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    err::ErrorCode Error() const noexcept
    {
        return m_dumpFolder.Error();
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR AND =
//
///////////////////////////////////////////////////////////////////////////////
public:
    HashTable() noexcept {}
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    static err::Result<HashTable> New() noexcept
    {
        HashTable tab;
        LOG_ERROR_IF(tab.Error());
        return { tab, tab.Error() };
    }

    static err::Result<HashTable> New(const HashTable& other) noexcept
    {
        HashTable tab(other);
        LOG_ERROR_IF(tab.Error());
        return { tab, tab.Error() };
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              INDEXING AND ITERATORS
//
///////////////////////////////////////////////////////////////////////////////
private:
    static uint64_t getIndex(const Key& key)
    {
        return Hash()(key) % Size;
    }

#define CHECK_CONTAINER(container, poison)              \
do                                                      \
{                                                       \
    if (auto error = (container).Error())               \
    {                                                   \
        LOG_ERROR(error);                               \
        return poison;                                  \
    }                                                   \
} while (0)

public:
    Val* operator[](const Key& key) & noexcept
    {
        std::size_t index = getIndex(key);
        CHECK_CONTAINER(m_containers[index], nullptr);

        auto found = findInContainer(m_containers[index], key);

        if (!found)
            return nullptr;

        return &m_containers[index][found.value].val;
    }

    const Val* operator[](const Key& key) const & noexcept
    {
        return (*this)[key];
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              PUBLIC METHODS
//
///////////////////////////////////////////////////////////////////////////////
public:
    err::ErrorCode Add(const HashTableElement& keyVal)
    {
        std::size_t index = getIndex(keyVal.key);
        RETURN_ERROR(m_containers[index].Error());

        auto found = findInContainer(m_containers[index], keyVal.key);

        if (found)
            return found;

        err::ErrorCode error = m_containers[index].PushBack(keyVal);

        LOG_ERROR_IF(error);

        return error;
    }

    err::ErrorCode Add(HashTableElement&& keyVal)
    {
        std::size_t index = getIndex(keyVal.key);
        RETURN_ERROR(m_containers[index].Error());

        auto found = findInContainer(m_containers[index], keyVal.key);

        if (found)
            return found;

        err::ErrorCode error = m_containers[index].PushBack(std::move(keyVal));

        LOG_ERROR_IF(error);

        return error;
    }

    err::Result<Val> Pop(const Key& key)
    {
        std::size_t index = getIndex(key);
        RETURN_ERROR_RESULT(m_containers[index].Error(), {});

        auto& container = m_containers[index];

        auto found = findInContainer(container, key);

        RETURN_ERROR_RESULT(found, {});

        err::Result<HashTableElement> result = container.Pop(found.value);

        LOG_ERROR_IF(result);

        return { result.value.val, result.error };
    }

    err::ErrorCode Remove(const Key& key)
    {
        return Pop(key).error;
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

#undef CHECK_CONTAINER

};

} // namespace mlib

#endif

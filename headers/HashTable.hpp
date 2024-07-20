#ifndef MLIB_HASHTABLE_HPP
#define MLIB_HASHTABLE_HPP

#include <ostream>

#include "Hash.hpp"

namespace mlib {

template<typename Key, typename Val, typename Hash = CRC32Functor<Key>,
         std::size_t Size = 128>
class HashTable
{
public:
    struct HashTableElement
    {
        Key key;
        Val val;

        friend std::ostream& operator<<(std::ostream&           out,
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
    String<>          m_dumpFolder{};
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    err::ErrorCode Error() const noexcept
    {
        err::ErrorCode error = m_containers.Error();
        if (error) return error;
        return m_dumpFolder.Error();
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              CTOR/DTOR AND =
//
///////////////////////////////////////////////////////////////////////////////
public:
    HashTable() noexcept
       : m_containers(Size) {}
///////////////////////////////////////////////////////////////////////////////
//
//                              RESULT CTORS
//
///////////////////////////////////////////////////////////////////////////////
public:
    static err::Result<HashTable> New() noexcept
    {
        HashTable tab;
        LOG_IF(tab.Error());
        return { tab, tab.Error() };
    }

    static err::Result<HashTable> New(const HashTable& other) noexcept
    {
        HashTable tab(other);
        LOG_IF(tab.Error());
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
        static const Hash h;
        return h(key) % Size;
    }
public:
    Val* operator[](const Key& key) noexcept
    {
        std::size_t index = getIndex(key);

        auto found = m_containers[index].Find({ key, {} });

        if (!found)
        {
            LOG(err::ERROR_NOT_FOUND);
            return nullptr;
        }

        return &m_containers[index][found.value].val;
    }

    const Val* operator[](const Key& key) const noexcept
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

        err::ErrorCode error = m_containers[index].PushBack(keyVal);

        LOG_IF(error);

        return error;
    }

    err::ErrorCode Add(HashTableElement&& keyVal)
    {
        std::size_t index = getIndex(keyVal.key);

        err::ErrorCode error = m_containers[index].PushBack(std::move(keyVal));

        LOG_IF(error);

        return error;
    }

    err::Result<Val> Pop(const Key& key)
    {
        std::size_t index = getIndex(key);

        auto& container = m_containers[index];

        auto found = container.Find({ key, {} });

        RETURN_ERROR_RESULT(found, {});

        err::Result<HashTableElement> result = container.Pop(found.value);

        LOG_IF(result);

        return { result.value.val, result.error };
    }

    err::ErrorCode Remove(const Key& key)
    {
        return Pop(key).error;
    }
};

} // namespace mlib

#endif

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
    String<>          m_logFolder{};
///////////////////////////////////////////////////////////////////////////////
//
//                              GETTERS
//
///////////////////////////////////////////////////////////////////////////////
public:
    Utils::Error Error() const noexcept
    {
        RETURN_ERROR(m_containers.Error());
        return m_logFolder.Error();
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
    static Utils::Result<HashTable> New() noexcept
    {
        HashTable tab;

        return { tab, tab.Error() };
    }

    static Utils::Result<HashTable> New(const HashTable& other) noexcept
    {
        HashTable tab(other);

        return { tab, tab.Error() };
    }
///////////////////////////////////////////////////////////////////////////////
//
//                              INDEXINT AND ITERATORS
//
///////////////////////////////////////////////////////////////////////////////
private:
    static inline uint64_t getIndex(const Key& key)
    {
        Hash h{};
        return h(key) % Size;
    }
public:
    Val* operator[](const Key& key) noexcept
    {
        std::size_t index = getIndex(key);
        
        auto found = m_containers[index].Find({ key, {} });

        if (!found)
            return nullptr;

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
    Utils::Error Add(const HashTableElement& keyVal)
    {
        std::size_t index = getIndex(keyVal.key);

        return m_containers[index].PushBack(keyVal);
    }

    Utils::Error Add(HashTableElement&& keyVal)
    {
        std::size_t index = getIndex(keyVal.key);

        return m_containers[index].PushBack(keyVal);
    }

    Utils::Result<Val> Pop(const Key& key)
    {
        std::size_t index = getIndex(key);

        auto& container = m_containers[index];

        auto found = container.Find({ key, {} });

        RETURN_ERROR_RESULT(found, {});

        Utils::Result<HashTableElement> result = container.Pop(found.value);

        return { result.value.val, result.error };
    }

    Utils::Error Remove(const Key& key)
    {
        return Pop(key).error;
    }
};

} // namespace mlib

#endif

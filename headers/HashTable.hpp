#ifndef MLIB_HASHTABLE_HPP
#define MLIB_HASHTABLE_HPP

#include <vector>
#include <optional>
#include "Error.hpp"

namespace mlib {

template<typename KeyT, typename ValueT>
class HashTable
{
    static const std::size_t DEFAULT_SIZE = 8;
    struct Elem
    {
        std::size_t hash = 0;
        KeyT        key{};
        ValueT      value{};
        bool        exists = false;
    };
    using Vec = std::vector<Elem>;

    std::size_t m_size = 0;
    Vec         m_data{DEFAULT_SIZE};
public:
    using Iterator      = typename Vec::iterator;
    using ConstIterator = typename Vec::const_iterator;

    HashTable() noexcept = default;

    explicit HashTable(std::size_t size) noexcept
        : m_data{size} {}

    void Insert(const KeyT& key, ValueT value)
    {
        return insert(std::hash<KeyT>{}(key), key, value);
    }

    Iterator Find(const KeyT& key)
    {
        return find(std::hash<KeyT>{}(key), key);
    }

    void     Erase (const KeyT& key);

    Iterator begin() noexcept
    {
        return m_data.begin();
    }

    Iterator end() noexcept
    {
        return m_data.end();
    }
private:
    void     insert (std::size_t hashValue, const KeyT& key, ValueT value);
    Iterator find   (std::size_t hashValue, const KeyT& key);
    void     realloc();
};

template<typename KeyT, typename ValueT>
void HashTable<KeyT, ValueT>::insert(std::size_t hashValue, const KeyT& key, ValueT value)
{
    if (Find(key) != end())
        return;
    if (m_size == m_data.capacity())
        realloc();

    std::size_t index = hashValue % m_data.capacity();

    while (m_data[index].exists)
        index = (index + 1) % m_data.capacity();

    m_data[index] = {
        hashValue,
        key,
        std::move(value),
        true,
    };

    m_size++;
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::Iterator
HashTable<KeyT, ValueT>::find(std::size_t hashValue, const KeyT& key)
{
    std::size_t index = hashValue % m_data.capacity();

    if (m_data[index].hash == hashValue && m_data[index].key == key)
        return begin() + index;

    for (std::size_t i = (index + 1) % m_data.capacity();
         i != index; i = (i + 1) % m_data.capacity())
    {
        if (!m_data[i].exists)
            continue;
        if (m_data[i].hash == hashValue && m_data[i].key == key)
            return begin() + i;
    }

    return end();
}

template<typename KeyT, typename ValueT>
void HashTable<KeyT, ValueT>::realloc()
{
    Vec temp{std::move(m_data)};
    temp.resize(temp.capacity() * 3 / 2);

    m_data = Vec{temp.capacity()};
    m_size = 0;

    for (Elem& el : temp)
    {
        if (el.exists)
        {
            insert(el.hash, el.key, el.value);
        }
    }
}

} // namespace mlib

#endif // MLIB_HASHTABLE_HPP

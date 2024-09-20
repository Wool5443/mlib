#include "HashTable.hpp"

namespace mlib {

template<typename KeyT, typename ValueT>
void HashTable<KeyT, ValueT>::Insert(const KeyT& key, ValueT value)
{
    return insert(std::hash<KeyT>{}(key), key, value);
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::Iterator
HashTable<KeyT, ValueT>::Find(const KeyT& key) noexcept
{
    return find(std::hash<KeyT>{}(key), key);
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::ConstIterator
HashTable<KeyT, ValueT>::Find(const KeyT& key) const noexcept
{
    return find(std::hash<KeyT>{}(key), key);
}

template<typename KeyT, typename ValueT>
ValueT&
HashTable<KeyT, ValueT>::operator[](const KeyT& key) noexcept
{
    std::size_t hashValue = std::hash<KeyT>{}(key);
    Iterator elem = find(hashValue, key);

    if (elem != end())
        return elem->value;

    elem = findFreeSpace(hashValue);

    *elem = {
        .hash   = hashValue,
        .key    = key,
        .value  = {},
        .exists = true,
    };

    m_size++;

    return elem->value;
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::Iterator
HashTable<KeyT, ValueT>::begin() noexcept
{
    return m_data.begin();
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::Iterator
HashTable<KeyT, ValueT>::end() noexcept
{
    return m_data.end();
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::ConstIterator
HashTable<KeyT, ValueT>::begin() const noexcept
{
    return m_data.begin();
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::ConstIterator
HashTable<KeyT, ValueT>::end() const noexcept
{
    return m_data.end();
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::ConstIterator
HashTable<KeyT, ValueT>::cbegin() const noexcept
{
    return m_data.cbegin();
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::ConstIterator
HashTable<KeyT, ValueT>::cend() const noexcept
{
    return m_data.cend();
}

template<typename KeyT, typename ValueT>
void
HashTable<KeyT, ValueT>::insert(std::size_t hashValue,
                                const KeyT& key,
                                ValueT value)
                                noexcept
{
    Iterator elem = find(hashValue, key);
    if (elem != end())
    {
        elem->value = std::move(value);
        return;
    }

    elem = findFreeSpace(hashValue);
    *elem = {
        .hash   = hashValue,
        .key    = key,
        .value  = std::move(value),
        .exists = true,
    };

    m_size++;
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::Iterator
HashTable<KeyT, ValueT>::findFreeSpace(std::size_t hashValue) noexcept
{
    realloc();

    std::size_t index = hashValue % m_data.capacity();

    while (m_data[index].exists)
        index = probe(index);

    return begin() + index;
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::Iterator
HashTable<KeyT, ValueT>::find(std::size_t hashValue, const KeyT& key) noexcept
{
    std::size_t index = hashValue % m_data.capacity();

    if (m_data[index].exists
     && m_data[index].hash == hashValue
     && m_data[index].key  == key)
        return begin() + index;

    for (std::size_t i = probe(index);
         i != index; i = probe(i))
    {
        if (!m_data[i].exists)
            continue;
        if (m_data[i].hash == hashValue && m_data[i].key == key)
            return begin() + i;
    }

    return end();
}

template<typename KeyT, typename ValueT>
typename HashTable<KeyT, ValueT>::ConstIterator
HashTable<KeyT, ValueT>::find(std::size_t hashValue, const KeyT& key) const noexcept
{
    return find(hashValue, key);
}

template<typename KeyT, typename ValueT>
void HashTable<KeyT, ValueT>::realloc() noexcept
{
    if (static_cast<double>(m_size) / m_data.capacity() < MAX_LOAD_FACTOR)
        return;

    Vec temp{std::move(m_data)};
    temp.resize(temp.capacity() * 3 / 2);

    m_data = Vec{temp.capacity()};
    m_size = 0;

    for (Elem& el : temp)
    {
        if (el.exists)
        {
            Iterator elem = findFreeSpace(el.hash);
            *elem = el;
            m_size++;
        }
    }
}

template<typename KeyT, typename ValueT>
std::size_t HashTable<KeyT, ValueT>::probe(std::size_t index) const noexcept
{
    return (index + 1) % m_data.capacity();
}

} // namespace mlib

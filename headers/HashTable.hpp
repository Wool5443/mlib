#ifndef MLIB_HASHTABLE_HPP
#define MLIB_HASHTABLE_HPP

#include <vector>

namespace mlib {

template<typename KeyT, typename ValueT>
class HashTable
{
    struct Elem
    {
        std::size_t hash = 0;
        KeyT        key{};
        ValueT      value{};
        bool        exists = false;
    };
    using Vec = std::vector<Elem>;
public:
    using Iterator      = typename Vec::iterator;
    using ConstIterator = typename Vec::const_iterator;

    /**
     * @brief Construct a Hash Table
     */
    HashTable() noexcept = default;

    /**
     * @brief Construct a Hash Table with given size
     *
     * @param [in] size
     */
    explicit HashTable(std::size_t size) noexcept
        : m_data{size} {}

    /**
     * @brief Inserts a key-value pair
     *
     * @param [in] key
     * @param [in] value
     */
    void Insert(const KeyT& key, ValueT value);

    /**
     * @brief Finds an element by key
     *
     * @param [in] key
     * @return Iterator
     */
    Iterator      Find(const KeyT& key)       noexcept;
    /**
     * @brief Finds an element by key
     *
     * @param [in] key
     * @return ConstIterator
     */
    ConstIterator Find(const KeyT& key) const noexcept;

    /**
     * @brief Erases the key-value pair from the table
     *
     * @param [in] key
     * @return void
     */
    void Erase(const KeyT& key) noexcept;

    /**
     * @brief Get elem by the key
     * It there is no such key inserts a default key-value pair
     *
     * @param [in] key
     * @return ValueT&
     */
    ValueT& operator[](const KeyT& key) noexcept;

    Iterator      begin()  noexcept;
    Iterator      end()    noexcept;
    ConstIterator begin()  const noexcept;
    ConstIterator end()    const noexcept;
    ConstIterator cbegin() const noexcept;
    ConstIterator cend()   const noexcept;
private:
    void insert(std::size_t hashValue, const KeyT& key, ValueT value)
    noexcept;

    Iterator find(std::size_t hashValue, const KeyT& key)
    noexcept;

    ConstIterator find(std::size_t hashValue, const KeyT& key)
    const noexcept;

    Iterator findFreeSpace(std::size_t hashValue) noexcept;

    void realloc() noexcept;

    std::size_t probe(std::size_t index) const noexcept;
private:
    static constexpr std::size_t DEFAULT_SIZE    = 8;
    static constexpr double      MAX_LOAD_FACTOR = 0.7;
    std::size_t m_size = 0;
    Vec         m_data{DEFAULT_SIZE};
};

} // namespace mlib

#include "HashTable.tpp"

#endif // MLIB_HASHTABLE_HPP

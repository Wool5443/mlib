#include <iostream>
#include "Tests.hpp"
#include "String.hpp"
#include "Vector.hpp"
#include "LinkedList.hpp"
#include "BinaryTree.hpp"
#include "HashTable.hpp"

using namespace mlib;

Utils::Error Tests::TestString(std::size_t n)
{
    String a{"Hello"};
    String b{"World"};

    String c = a + b;

    String correct = "HelloWorld";

    if (c != correct)
    {
        std::cout << "Got: " << c << ", expected: " << correct << '\n';
        return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
    }

    String bebra;

    for (std::size_t i = 0; i < n; i++)
        bebra += "bebra";

    Utils::Result<std::size_t> countRes = bebra.Count("bebra");
    RETURN_ERROR(countRes.error);

    if (countRes.value != n)
    {
        std::cout << "Got: " << countRes.value << ", expected: " << n << '\n';
        return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
    }

    String wordsStr =
    "Hello, dear readers of this code " // 6
    "I know this is some cursed C++, though, " // 8
    "don't be to critical."; // 4
    std::size_t wordsNum = 18;

    auto words = wordsStr.Split();
    RETURN_ERROR(words);

    for (const String<>& word : words.value)
        std::cout << word << ' ';

    std::cout << '\n';

    if (words.value.length != wordsNum)
    {
        std::cout << "Got: " << words.value.length << ", expected: " << wordsNum << '\n';
        return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
    }

    return Utils::Error();
}

Utils::Error Tests::TestVector(std::size_t n)
{
    Vector<int> vec;

    for (std::size_t i = 0; i < n; i++)
        vec.PushBack((i + 1) * 10);

    for (std::size_t i = 0; i < n; i++)
        if (vec[i] != (int)(i + 1) * 10)
        {
            std::cout << "Got: " << vec[i] << ", expected: " <<
                          (int)(i + 1) * 10 << '\n';
            return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
        }

    auto find50 = vec.Find(50);
    RETURN_ERROR(find50);

    std::cout << vec[find50.value] << '\n';

    return Utils::Error();
}

Utils::Error Tests::TestList(std::size_t n)
{
    LinkedList<int> list;

    list.StartLogging("../listLog");

    for (std::size_t i = 0; i < n; i++)
        list.PushBack((i + 1) * 10);

    list.Dump();

    for (std::size_t i = 1; i < n; i++)
        if (list[i] != (int)(i) * 10)
        {
            std::cout << list[i] << " != " << (int)(i) * 10 << '\n';
            return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
        }

    for (std::size_t i = 0; i < n; i++)
    {
        Utils::Result<int> res = list.PopBack();
        RETURN_ERROR(res);

        if (res.value != ((int)n - (int)i) * 10)
        {
            std::cout << res.value << " != " << (n - i) * 10 << '\n';
            return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
        }
    }

    list.Dump();

    list.EndLogging();

    return Utils::Error();
}

Utils::Error Tests::TestBTree()
{
    BinaryTree<int> tree(0);

    #ifdef LOGGING
    tree.StartLogging("../treeLog");
    #endif

    BinaryTreeNode<int>* root = tree.root;

    root->SetLeft(BinaryTreeNode<int>::New(1).value);
    root->SetRight(BinaryTreeNode<int>::New(2).value);

    #ifdef LOGGING
    tree.Dump();
    tree.EndLogging();
    #endif

    return Utils::Error();
}

Utils::Error Tests::TestHashTable()
{
    HashTable<String<>, int> table;

    RETURN_ERROR(table.Add({ "Hello", 1 }));
    RETURN_ERROR(table.Add({ "World", 2 }));

    std::cout << *table["Hello"] << '\n';
    std::cout << *table["World"] << '\n';

    auto res1 = table.Pop("Hello");
    RETURN_ERROR(res1);

    std::cout << res1.value << '\n';

    return {};
}

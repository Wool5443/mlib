#include <iostream>
#include <string.h>
#include "Tests.hpp"
#include "String.hpp"
#include "Vector.hpp"
#include "LinkedList.hpp"
#include "BinaryTree.hpp"
#include "HashTable.hpp"
#include "Utils.hpp"

using namespace mlib;

err::ErrorCode Tests::TestString(std::size_t n)
{
    String str8bytes{"12345678"}; // works

    std::cout << str8bytes << '\n';

    String a{"Hello"};
    RETURN_ERROR(a.Error());
    String b{"World"};
    RETURN_ERROR(b.Error());

    String c = a + b;
    RETURN_ERROR(c.Error());

    String correct = "HelloWorld";
    RETURN_ERROR(correct.Error());

    if (c != correct)
    {
        std::cout << "Got: " << c << ", expected: " << correct << '\n';
        RETURN_ERROR(err::ERROR_BAD_VALUE);
    }

    String bebra;

    for (std::size_t i = 0; i < n; i++)
        bebra += "bebra";

    err::Result<std::size_t> countRes = bebra.Count("bebra");
    RETURN_ERROR(countRes);

    if (countRes.value != n)
    {
        std::cout << "Got: " << countRes.value << ", expected: " << n << '\n';
        RETURN_ERROR(err::ERROR_BAD_VALUE);
    }

    String wordsStr =
    "Hello, dear readers of this code " // 6
    "I know this is some cursed C++, though, " // 8
    "don't be to critical."; // 4
    RETURN_ERROR(wordsStr.Error());
    std::size_t wordsNum = 18;

    auto words = wordsStr.Split();
    RETURN_ERROR(words);

    for (const String& word : words.value)
        std::cout << word << ' ';

    std::cout << '\n';

    if (words.value.length != wordsNum)
    {
        std::cout << "Got: " << words.value.length << ", expected: " << wordsNum << '\n';
        RETURN_ERROR(err::ERROR_BAD_VALUE);
    }

    return err::EVERYTHING_FINE;
}

err::ErrorCode Tests::TestVector(std::size_t n)
{
    Vector<int> vec;

    for (std::size_t i = 0; i < n; i++)
    {
        vec.PushBack((i + 1) * 10);
        RETURN_ERROR(vec.Error());
    }

    for (std::size_t i = 0; i < n; i++)
        if (vec[i] != (int)(i + 1) * 10)
        {
            std::cout << "Got: " << vec[i] << ", expected: " <<
                          (int)(i + 1) * 10 << '\n';
            RETURN_ERROR(err::ERROR_BAD_VALUE);
        }

    auto find50 = vec.Find(50);
    RETURN_ERROR(find50);

    std::cout << vec[find50.value] << '\n';

    return err::EVERYTHING_FINE;
}

err::ErrorCode Tests::TestList(std::size_t n)
{
    LinkedList<int> list;

    list.InitDump("../dump/list");

    for (std::size_t i = 0; i < n; i++)
    {
        list.PushBack((i + 1) * 10);
        RETURN_ERROR(list.Error());
    }

    list.Dump();

    for (std::size_t i = 1; i < n; i++)
        if (list[i] != (int)(i) * 10)
        {
            std::cout << list[i] << " != " << (int)(i) * 10 << '\n';
            RETURN_ERROR(err::ERROR_BAD_VALUE);
        }

    for (std::size_t i = 0; i < n; i++)
    {
        err::Result<int> res = list.PopBack();
        RETURN_ERROR(res);

        if (res.value != ((int)n - (int)i) * 10)
        {
            std::cout << res.value << " != " << (n - i) * 10 << '\n';
            RETURN_ERROR(err::ERROR_BAD_VALUE);
        }
    }

    list.Dump();

    list.FinishDump();

    return err::EVERYTHING_FINE;
}

err::ErrorCode Tests::TestBTree()
{
    BinaryTree<int> tree(0);

    tree.InitDump("../dump/tree");

    BinaryTreeNode<int>* root = tree.root;

    RETURN_ERROR(root->SetLeft(BinaryTreeNode<int>::New(1).value));
    RETURN_ERROR(root->SetRight(BinaryTreeNode<int>::New(2).value));

    tree.Dump();
    tree.FinishDump();

    return err::EVERYTHING_FINE;
}

err::ErrorCode Tests::TestHashTable()
{
    HashTable<str, int> wordsCountTable;
    RETURN_ERROR(wordsCountTable.Error());

    char* text = mlib::ReadFileToBuf("../tests/Words.txt");
    if (!text)
        RETURN_ERROR(err::ERROR_NO_MEMORY);

    auto wordsRes = Split(text);
    RETURN_ERROR(wordsRes);

    Vector<str>& words = wordsRes.value;
    for (str word : words)
    {
        int* count = wordsCountTable[word];

        if (count)
            *count += 1;
        else
            RETURN_ERROR(wordsCountTable.Add(str{word}, 1));
    }

    std::ofstream out("../tests/HashTableResult.txt");
    if (!out)
        RETURN_ERROR(err::ERROR_BAD_FILE);

    for (str word : wordsRes.value)
    {
        int* count = wordsCountTable[word];

        if (count)
            out << word << ": " << *count << '\n';
        else
            RETURN_ERROR(err::ERROR_BAD_VALUE);
    }

    free(text);
    out.close();

    system("diff ../tests/HashTableResult.txt ../tests/ResultsPython.txt");

    return err::EVERYTHING_FINE;
}

err::ErrorCode Tests::TestHashTableSpeed(std::size_t numberOfTests)
{
    static const std::size_t to_ms = 1000000;

    char* text = mlib::ReadFileToBuf("../tests/Words.txt");
    if (!text)
        RETURN_ERROR(err::ERROR_NO_MEMORY);

    auto wordsRes = Split(text);
    RETURN_ERROR(wordsRes);
    Vector<str>& words = wordsRes.value;

    mlib::Timer timer;
    for (std::size_t i = 0; i < numberOfTests; i++)
    {
        HashTable<str, int> wordsCountTable;
        RETURN_ERROR(wordsCountTable.Error());

        for (str word : words)
        {
            int* count = wordsCountTable[word];

            if (count)
                *count += 1;
            else
                RETURN_ERROR(wordsCountTable.Add(str{word}, 1));
        }

        for (str word : words)
        {
            int* count = wordsCountTable[word];
            *count += 1;
        }
    }

    auto duration = timer.Stop();

    free(text);

    std::cout << "HashTable Speed Test took: "
    << duration.count() / to_ms << "ms\n";

    return err::EVERYTHING_FINE;
}

#include <iostream>
#include <fstream>
#include <string>
#include "Tests.hpp"

using namespace mlib;
using namespace err;

#if 0
err::ErrorCode Tests::TestString(std::size_t n)
{
    String str8bytes{"12345678"}; // works

    std::cout << str8bytes << '\n';

    String a{"Hello"};
    RETURN_ERROR_IF(a.Error());
    String b{"World"};
    RETURN_ERROR_IF(b.Error());

    String c = a + b;
    RETURN_ERROR_IF(c.Error());

    String correct = "HelloWorld";
    RETURN_ERROR_IF(correct.Error());

    if (c != correct)
    {
        std::cout << "Got: " << c << ", expected: " << correct << '\n';
        RETURN_ERROR_IF(err::ERROR_BAD_VALUE);
    }

    String bebra;

    for (std::size_t i = 0; i < n; i++)
        bebra += "bebra";

    err::Result<std::size_t> countRes = bebra.Count("bebra");
    RETURN_ERROR_IF(countRes);

    if (countRes.value != n)
    {
        std::cout << "Got: " << countRes.value << ", expected: " << n << '\n';
        RETURN_ERROR_IF(err::ERROR_BAD_VALUE);
    }

    String wordsStr =
    "Hello, dear readers of this code " // 6
    "I know this is some cursed C++, though, " // 8
    "don't be to critical."; // 4
    RETURN_ERROR_IF(wordsStr.Error());
    std::size_t wordsNum = 18;

    auto words = wordsStr.Split();
    RETURN_ERROR_IF(words);

    for (const String& word : words.value)
        std::cout << word << ' ';

    std::cout << '\n';

    if (words.value.length != wordsNum)
    {
        std::cout << "Got: " << words.value.length << ", expected: " << wordsNum << '\n';
        RETURN_ERROR_IF(err::ERROR_BAD_VALUE);
    }

    return err::EVERYTHING_FINE;
}

err::ErrorCode Tests::TestVector(std::size_t n)
{
    Vector<int> vec;

    for (std::size_t i = 0; i < n; i++)
    {
        vec.PushBack((i + 1) * 10);
        RETURN_ERROR_IF(vec.Error());
    }

    for (std::size_t i = 0; i < n; i++)
        if (vec[i] != (int)(i + 1) * 10)
        {
            std::cout << "Got: " << vec[i] << ", expected: " <<
                          (int)(i + 1) * 10 << '\n';
            RETURN_ERROR_IF(err::ERROR_BAD_VALUE);
        }

    auto find50 = vec.Find(50);
    RETURN_ERROR_IF(find50);

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
        RETURN_ERROR_IF(list.Error());
    }

    list.Dump();

    for (std::size_t i = 1; i < n; i++)
        if (list[i] != (int)(i) * 10)
        {
            std::cout << list[i] << " != " << (int)(i) * 10 << '\n';
            RETURN_ERROR_IF(err::ERROR_BAD_VALUE);
        }

    for (std::size_t i = 0; i < n; i++)
    {
        err::Result<int> res = list.PopBack();
        RETURN_ERROR_IF(res);

        if (res.value != ((int)n - (int)i) * 10)
        {
            std::cout << res.value << " != " << (n - i) * 10 << '\n';
            RETURN_ERROR_IF(err::ERROR_BAD_VALUE);
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

    RETURN_ERROR_IF(root->SetLeft(BinaryTreeNode<int>::New(1).value));
    RETURN_ERROR_IF(root->SetRight(BinaryTreeNode<int>::New(2).value));

    tree.Dump();
    tree.FinishDump();

    return err::EVERYTHING_FINE;
}

err::ErrorCode Tests::TestHashTableSpeed(std::size_t numberOfTests)
{
    static const std::size_t to_ms = 1000000;

    char* text = mlib::ReadFileToBuf("../tests/Words.txt");
    if (!text)
        RETURN_ERROR_IF(err::ERROR_NO_MEMORY);

    auto wordsRes = Split(text);
    RETURN_ERROR_IF(wordsRes);
    Vector<str>& words = wordsRes.value;

    mlib::Timer timer;
    for (std::size_t i = 0; i < numberOfTests; i++)
    {
        HashTable<str, int> wordsCountTable;
        RETURN_ERROR_IF(wordsCountTable.Error());

        for (str word : words)
        {
            int* count = wordsCountTable[word];

            if (count)
                *count += 1;
            else
                RETURN_ERROR_IF(wordsCountTable.Add(str{word}, 1));
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

#endif

err::ErrorCode Tests::TestHashTable()
{
    using Table = HashTable<std::string_view, int>;
    Table wordsCountTable{500000};

    auto textOptional = ReadFileToBuf("../tests/Words.txt");
    if (!textOptional)
        RETURN_ERROR_IF(err::ERROR_BAD_FILE);
    std::string& text = textOptional.value();

    std::vector words = SplitString(text, "\n");

    for (std::string_view word : words)
    {
        Table::Iterator count = wordsCountTable.Find(word);

        if (count != wordsCountTable.end())
            count->value += 1;
        else
            wordsCountTable.Insert(word, 1);
    }

    std::cout << "DONE!!!!" << std::endl;

    std::ofstream out{"../tests/HashTableResult.txt"};
    if (!out)
        RETURN_ERROR_IF(err::ERROR_BAD_FILE);

    for (std::string_view word : words)
    {
        Table::Iterator count = wordsCountTable.Find(word);

        if (count != wordsCountTable.end())
            out << word << ": " << count->value << '\n';
        else
            RETURN_ERROR_IF(err::ERROR_BAD_VALUE);
    }

    out.close();

    system("diff ../tests/HashTableResult.txt ../tests/ResultsPython.txt");

    return err::EVERYTHING_FINE;
}

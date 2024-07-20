# Mlib

Some very basic templated containers plus logging system.

## Install
First add the repo to your project.
```bash
git clone https://github.com/Wool5443/mlib
```
Then add the source and header files to your build system.

## Features
 * String
 * Vector
 * LinkedList
 * HashTable
 * Logger

## How to use

### Logger
```c++
#include <ofstream>
#include <cstddef>
#include "Error.hpp"

/*****************************************************/
LOG_INIT_CONSOLE(); // In the end prints to stdout

// choose either

std::ofstream LOG_STREAM("log.txt");
LOG_INIT_FILE(LOG_STREAM) // In the end prints to LOG_STREAM

LOG_DISABLE() // To disable logging
/*****************************************************/

err::ErrorCode foo()
{
    int* array = new int[10000000];

    if (!array)
        RETURN_ERROR(err::ERROR_NO_MEMORY); // return and log error

    /* some code */

    return err::EVERYTHING_FINE;
}

err::Result<int> bar()
{
    err::ErrorCode error = foo();

    // return and log result with error and poison if error occured
    RETURN_ERROR_RESULT(error, INT_MIN);

    // or

    LOG(error);
    return { INT_MIN, error };

    /* some code */
}

int main()
{
    RETURN_ERROR(foo); // returns and logs if foo fails

    err::Result<int> res = bar();
    RETURN_ERROR(res); // returns and logs if bar failed

    return 0;
}
```

### String

```c++
#include "String.hpp"

LOG_INIT_CONSOLE();

int main()
{
    String a{"Hello"};
    String b{"World"};

    String c = a + ' ' + b;

    std::cout << c << '\n'; //> Hello World

    String bebra;

    for (std::size_t i = 0; i < 50; i++)
        bebra += "bebra";

    err::Result<std::size_t> countRes = bebra.Count("bebra");
    RETURN_ERROR(countRes);

    std::cout << countRes.value << '\n'; //> 50

    String wordsStr =
    "Hello, dear readers of this code " // 6 words
    "I know this is some cursed C++, though, " // 8 words
    "don't be to critical."; // 4 words

    err::Result<Vector<String<>>> words = wordsStr.Split();
    RETURN_ERROR(words);

    for (const String<>& word : words.value)
        std::cout << word << ' ';

    std::cout << '\n';

    err::Result<std::size_t> indRes = wordsStr.Find("dear");
    RETURN_ERROR(indRes);

    std::cout << indRes.value << '\n'; //> 7

    return err::EVERYTHING_FINE;
}
```

### Vector
```c++
#include "Vector.hpp"

LOG_INIT_CONSOLE();

int main()
{
    Vector<int> vec;
    RETURN_ERROR(vec.Error()); // to handle possible allocation error
    // or more safely
    err::Result<Vector<int>> vecRes = Vector<int>::New();
    RETURN_ERROR(vecRes);

    for (std::size_t i = 1; i <= 10; i++)
        vec.PushBack(i);

    for (auto el : vec)
        std::cout << el << ' '; //> 1 2 3 ... 10
    std::cout << '\n';

    auto findRes = vec.Find(4);
    RETURN_ERROR(findRes);

    std::cout << vec[findRes.value] << '\n'; //> 3

    return err::EVERYTHING_FINE;
}
```

### LinkedList
```c++
#include "LinkedList.hpp"

LOG_INIT_CONSOLE();

int main()
{
    LinkedList<int> list;
    RETURN_ERROR(list.Error());

    // or more safely

    err::Result<LinkedList<int>> listRes = LinkedList<int>::New();
    RETURN_ERROR(listRes);

    list.InitDump("../dump/list");

    for (std::size_t i = 1; i <= 10; i++)
        list.PushBack(i);

    list.Dump(); // Dumps a list to an html file

    for (std::size_t i = 1; i <= 10; i++)
    {
        err::Result<int> res = list.PopBack();
        RETURN_ERROR(res);

        std::cout << res.value << ' '; //> 10 9 ... 1
    }

    std::cout << '\n';

    list.Dump();

    list.FinishDump();

    return 0;
}
```

### BinaryTree
```c++
#include "BinaryTree.hpp"

LOG_INIT_CONSOLE();

int main()
{
    BinaryTree<int> tree(0); // Constructs a tree with 0 in root

    BinaryTree<int> emptyTree; // Constructs a tree without any root

    tree.InitDump("../dump/tree");

    BinaryTreeNode<int>* root = tree.root;

    // Creates a new node
    auto leftRes  = BinaryTreeNode<int>::New(1);
    RETURN_ERROR(leftRes);
    auto rightRes = BinaryTreeNode<int>::New(2);
    RETURN_ERROR(rightRes, leftRes.value->Delete());

    root->SetLeft (leftRes.value);
    root->SetRight(rightRes.value);

    tree.Dump();
    tree.Finish();

    return err::EVERYTHING_FINE;
}
```

### HashTable
```c++
#include "HashTable.hpp"
#include "String.hpp"

LOG_INIT_CONSOLE();

int main()
{
    HashTable<String<>, int> table;

    RETURN_ERROR(table.Add({ "Hello", 1 }));
    RETURN_ERROR(table.Add({ "World", 2 }));

    // table[key] = pointer to val or nullptr
    std::cout << *table["Hello"] << '\n'; //> 1
    std::cout << *table["World"] << '\n'; //> 2

    err::Result<int> res1 = table.Pop("Hello");
    RETURN_ERROR(res1);

    std::cout << res1.value << '\n'; //> 1

    return err::EVERYTHING_FINE;
}
```

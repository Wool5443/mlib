# Mlib

Some very basic templated containers plus logging system.

## Install
First add the repo to your project.
```bash
git clone https://github.com/Wool5443/mlib

cd mlib

mkdir build
cd build

cmake ..
ninja -j8

./mlib
```
Then add the source and header files to your build system.

## Features
 * String
 * Vector
 * LinkedList
 * HashTable
 * Logger
 * Error handling as values

Every container uses dynamic allocation and all are easily resizable.

Note that all methods return either ErrorCode or a Result so you should
always handle the error via these macros:
```c++
// ... is for code to do before returning
RETURN_ERROR_IF(error, ...)
RETURN_ERROR_RESULT(error, poison, poisonType, ...)
RETURN_RESULT_IF(result, ...)
```

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
        RETURN_ERROR_IF(err::ERROR_NO_MEMORY); // return and log error

    /* some code */

    return err::EVERYTHING_FINE;
}

err::Result<int> bar()
{
    err::ErrorCode error = foo();

    // return and log result with error and poison if error occured
    RETURN_ERROR_RESULT(error, INT_MIN, int);

    /* some code */

    return 5;
}

int main()
{
    RETURN_ERROR_IF(foo); // returns and logs if foo fails

    err::Result<int> res = bar();
    RETURN_ERROR_IF(res); // returns and logs if bar failed

    LOG("Hello, this is a test log"); // Logs a message

    return 0;
}
```

### Container creation
```c++
#include "String.hpp"

err::ErrorCode foo()
{
    // This creates an empty object without doing any allocations
    String str;
    static_assert(str.Error() == err::ERROR_UNINITIALIZED);

    // Then there are 2 ways of createing a non-empty object

    String strCommonWay("Hello");
    RETURN_ERROR_IF(strCommonWay.Error());

    String strAssign = "Hello";
    RETURN_ERROR_IF(strAssign.Error());

    // Do not forget to check for errors!!!!
}
```

### String

```c++
#include "String.hpp"
using namespace mlib;

LOG_INIT_CONSOLE();

int main()
{
    String a("Hello");
    RETURN_ERROR_IF(a.Error());
    String b("World");
    RETURN_ERROR_IF(b).Error();

    String c = a + ' ' + b;
    RETURN_ERROR_IF(c.Error());

    std::cout << c << '\n'; //> Hello World

    String bebra;

    for (std::size_t i = 0; i < 50; i++)
    {
        bebra += "bebra";
        RETURN_ERROR_IF(bebra.Error());
    }

    err::Result<std::size_t> countRes = bebra.Count("bebra");
    RETURN_ERROR_IF(countRes);

    std::cout << countRes.value << '\n'; //> 50

    String wordsStr =
    "Hello, dear readers of this code " // 6 words
    "I know this is some cursed C++, though, " // 8 words
    "don't be to critical."; // 4 words
    RETURN_ERROR_IF(wordsStr.Error());

    err::Result<Vector<String<>>> words = wordsStr.Split(" ");
    RETURN_ERROR_IF(words);

    for (const String<>& word : words.value)
        std::cout << word << ' ';

    std::cout << '\n';

    err::Result<std::size_t> indRes = wordsStr.Find("dear");
    RETURN_ERROR_IF(indRes);

    std::cout << indRes.value << '\n'; //> 7

    return err::EVERYTHING_FINE;
}
```

### Vector
```c++
#include "Vector.hpp"
using namespace mlib;

LOG_INIT_CONSOLE();

int main()
{
    Vector<int> vec;

    for (std::size_t i = 1; i <= 10; i++)
        RETURN_ERROR_IF(vec.PushBack(i));

    for (auto el : vec)
        std::cout << el << ' '; //> 1 2 3 ... 10
    std::cout << '\n';

    auto findRes = vec.Find(4);
    RETURN_ERROR_IF(findRes);

    std::cout << vec[findRes.value] << '\n'; //> 3

    return err::EVERYTHING_FINE;
}
```

### LinkedList
```c++
#include "LinkedList.hpp"
using namespace mlib;

LOG_INIT_CONSOLE();

int main()
{
    // List actually does allocation since it needs a fictional
    // element in the beginning to be valid.
    LinkedList<int> list;
    RETURN_ERROR_IF(list.Error());

    RETURN_ERROR_IF(list.InitDump("../dump/list"));

    for (std::size_t i = 1; i <= 10; i++)
    {
        list.PushBack(i);
        RETURN_ERROR_IF(list.Error());
    }

    list.Dump(); // Dumps a list to an html file

    for (std::size_t i = 1; i <= 10; i++)
    {
        err::Result<int> res = list.PopBack();
        RETURN_ERROR_IF(res);

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
using namespace mlib;

LOG_INIT_CONSOLE();

int main()
{
    BinaryTree<int> tree(0); // Constructs a tree with 0 in root
    RETURN_ERROR_IF(tree.Error());

    BinaryTree<int> emptyTree; // Constructs a tree without any root

    RETURN_ERROR_IF(tree.InitDump("../dump/tree"));

    BinaryTreeNode<int>* root = tree.root;

    // Creates a new node
    auto leftRes  = BinaryTreeNode<int>::New(1);
    RETURN_ERROR_IF(leftRes);
    auto rightRes = BinaryTreeNode<int>::New(2);
    RETURN_ERROR_IF(rightRes, leftRes.value->Delete());

    RETURN_ERROR_IF(root->SetLeft (leftRes. value), leftRes .value->Delete());
    RETURN_ERROR_IF(root->SetRight(rightRes.value), rightRes.value->Delete());

    RETURN_ERROR_IF(tree.Dump());

    RETURN_ERROR_IF(tree.Finish());

    return err::EVERYTHING_FINE;
}
```

### HashTable
```c++
#include "HashTable.hpp"
#include "String.hpp"
using namespace mlib;

LOG_INIT_CONSOLE();

int main()
{
    HashTable<String<>, int> table;

    RETURN_ERROR_IF(table.Add({ "Hello", 1 }));
    RETURN_ERROR_IF(table.Add({ "World", 2 }));

    // table[key] = pointer to val or nullptr
    std::cout << *table["Hello"] << '\n'; //> 1
    std::cout << *table["World"] << '\n'; //> 2

    err::Result<int> res1 = table.Pop("Hello");
    RETURN_ERROR_IF(res1);

    std::cout << res1.value << '\n'; //> 1

    return err::EVERYTHING_FINE;
}
```

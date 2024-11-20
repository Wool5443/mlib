# MLIB
## What?
MLIB stands for ***Misha's Library***. Here I collect all very useful and
reusable code I wrote and I like.

# Logger

## Features
* **Error handling as value**
* **Logging system**
* **Wrapper of C FILE***

### Error handling & logging system
```c++
#include <iostream>
#include "Error.hpp"

using namespace err;

LOG_INIT_CONSOLE();

err::ErrorCode divideAndPrint(int a, int b)
{
    if (b == 0)
        RETURN_ERROR(ERROR_ZERO_DIVISION);

    int c = a / b;
    std::cout << a << " / " << b << " = " << c << std::endl;

    return err::EVERYTHING_FINE;
}

int main()
{
    divideAndPrint(56, 8);
    divideAndPrint(42, 0);

    LOG_INFO("This is some funny message!!!\n");

    return 0;
}
```
Output:

56 / 8 = 7
This is some funny message!!!

<span style="color: red;">2 items were dumped</span><br>
<span style="color: red;">ERROR_ZERO_DIVISION in /home/twenty/Programming/Logger/examples/Errors.cpp:11 in err::ErrorCode divideAndPrint(int, int)</span>

# Utils

## Features
* **Short int types**
* **Split string**
* **Read from file**
* **Comparing doubles**
* **Performance measuring using TickTimer or Timer**

### Reading from file
```c++
#include "Error.hpp"
#include "Utils.hpp"

using namespace err;
using namespace mlib;
using namespace std;

LOG_INIT_FILE("../../logIO.txt");

int main()
{
    const char* file = "../../file.txt";

    Result<string> text = ReadFileToBuf(file);

    RETURN_ERROR_IF(text);

    Result<vector<string_view>> words = SplitString(*text);

    RETURN_ERROR_IF(words);

    if (words->size() != 923)
        LOG("Wrong number of words!!!: %zu\n", words->size());

    for (auto it = words->rbegin(), end = words->rend(); it != end; ++it)
        std::cout << *it << '\n';

    return 0;
}
```

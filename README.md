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
#include "Logger.hpp"

using namespace mlib;
using namespace err;

ErrorCode divideAndPrint(int a, int b)
{
    if (b == 0)
    {
        GlobalLogError(ERROR_ZERO_DIVISION, "Don't divide by zero!!!");
        return ERROR_ZERO_DIVISION;
    }

    int c = a / b;
    fmt::print("{} / {} = {}\n", a, b, c);

    return EVERYTHING_FINE;
}

int main()
{
    divideAndPrint(56, 8);
    divideAndPrint(42, 0);

    GlobalLogInfo("Funny message");

    Logger logger{"log.txt"};

    logger.LogInfo("Here is a log file");

    SetGlobalLoggerLogFile("globalLog.txt");

    GlobalLogInfo("This message will be in globalLog.txt!");

    return 0;
}
```
Output:

56 / 8 = 7<br>
<span style="color: red;">[ERROR] ERROR_ZERO_DIVISION:6 /home/twenty/Programming/mlib/examples/Errors.cpp:12 in ErrorCode divideAndPrint(int, int)
Don't divide by zero!!!</span><br>

<span style="color: cyan;">[INFO] /home/twenty/Programming/mlib/examples/Errors.cpp:27 in int main()</span><br>
Funny message<br>

```bash
cat log.txt
[INFO] /home/twenty/Programming/mlib/examples/Errors.cpp:31 in int main()
Here is a log file

cat globalLog.txt
[INFO] /home/twenty/Programming/mlib/examples/Errors.cpp:35 in int main()
This message will be in globalLog.txt!
```

# Utils

## Features
* **Short int types**
* **Split string**
* **Read from file**
* **Comparing doubles**
* **Performance measuring using TickTimer or Timer**

### Reading from file
```c++
#include "Result.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

using namespace mlib;
using namespace err;
using namespace std;

int main()
{
    const char* file = "../../file.txt";

    Result<string> text = ReadFileToBuf(file);

    if (!text)
    {
        GlobalLogError(text.Error());
        return text.Error();
    }

    Result<vector<string_view>> words = SplitString(*text);

    if (!words)
    {
        GlobalLogError(words.Error());
        return words.Error();
    }

    if (words->size() != 923)
        GlobalLogError(ERROR_BAD_VALUE, "Wrong number of words!!!: %zu\n", words->size());

    for (auto it = words->rbegin(), end = words->rend(); it != end; ++it)
        std::cout << *it << '\n';

    return 0;
}
```

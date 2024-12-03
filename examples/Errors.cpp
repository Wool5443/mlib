//NOLINTBEGIN

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

//NOLINTEND

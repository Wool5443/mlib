#include "Logger.hpp"

using namespace mlib;
using namespace err;

Error_code divideAndPrint(int a, int b)
{
    if (b == 0)
    {
        global_log_error(ERROR_ZERO_DIVISION, "Don't divide by zero!!!");
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

    global_log_info("Funny message");

    Logger logger{"log.txt"};

    logger.log_info("Here is a log file");

    set_global_logger_log_file("globalLog.txt");

    global_log_info("This message will be in globalLog.txt!");

    return 0;
}

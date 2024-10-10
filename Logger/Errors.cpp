//NOLINTBEGIN

#include <iostream>
//#define LOG_IMMEDIATE
#include "Error.hpp"

using namespace err;

LOG_INIT_CONSOLE();

err::ErrorCode divideAndPrint(int a, int b)
{
    if (b == 0)
        RETURN_ERROR(ERROR_ZERO_DIVISION);

    int c = a / b;
    std::cout << a << " / " << b << " = " << c << std::endl;

    return EVERYTHING_FINE;
}

int main()
{
    divideAndPrint(56, 8);
    divideAndPrint(42, 0);

    LOG("This is some funny message!!!\n");

    return 0;
}

//NOLINTEND

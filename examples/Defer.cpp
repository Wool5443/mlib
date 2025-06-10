#include <fmt/format.h>
#include "Defer.hpp"

int main()
{
    int* numbers = new int[100];
    defer [&] () { delete[] numbers; };


    for (int i = 0; i < 100; i++)
    {
        fmt::println("{}", numbers[i]);
    }
}

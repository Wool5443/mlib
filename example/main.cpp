#include <cassert>
#include "String.hpp"
#include "Vector.hpp"

int main()
{
    using namespace mlib;

    String a = "Hello";

    String b = "World";

    std::cout << a << '\n' << b << '\n';

    Vector<int> vec1;
    Vector<int> vec2(vec1);

    return 0;
}

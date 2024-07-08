#include <cassert>
#include "String.hpp"
#include "Vector.hpp"

template<typename T>
void PrintVector(mlib::Vector<int>& vec)
{
    for (size_t i = 0; i < vec.Length(); i++)
        std::cout << vec[i] << ' ';
    std::cout << '\n';
}

int main()
{
    using namespace mlib;

    String a = "Hello";

    String b = "World";

    std::cout << a << '\n' << b << '\n';


    size_t n = 10;

    Vector<int> vec1;

    for (size_t i = 0; i < n; i++)
        vec1.PushBack(i);

    Vector<int> vec2 = vec1;

    for (size_t i = 0; i < vec1.Length(); i++)
        vec1[i] *= vec1[i];

    PrintVector<int>(vec1);
    PrintVector<int>(vec2);

    return 0;
}

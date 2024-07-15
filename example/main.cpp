#include <cassert>
#include "String.hpp"
#include "Vector.hpp"
#include "LinkedList.hpp"

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

    String string = "Hello my\ndear friends!";

    Vector<String<>> words = string.Split();

    std::cout << string << '\n';

    for (size_t i = 0; i < words.Length(); i++)
        std::cout << words[i] << ' ';
    std::cout << '\n';

    LinkedList<int> list;

    list.StartLogging("../log");

    list.InsertAfter(100, 0);
    list.InsertAfter(200, 1);
    list.InsertAfter(300, 2);

    for (int i = 0; i < 10; i++)
        list.PushBack(i * 100);

    list.Dump().Print();

    list.EndLogging();

    return 0;
}

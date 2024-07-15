#include "Tests.hpp"

using namespace mlib;

Utils::Error Tests::TestString(size_t n)
{
    String a{"Hello"};
    String b{"World"};

    String c = a + b;

    String correct = "HelloWorld";

    if (c != correct)
    {
        std::cout << "Got: " << c << ", expected: " << correct << '\n';
        return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
    }

    String bebra;

    for (size_t i = 0; i < n; i++)
        bebra += "bebra";

    Utils::Result<size_t> countRes = bebra.Count("bebra");
    RETURN_ERROR(countRes.error);

    if (countRes.value != n)
    {
        std::cout << "Got: " << countRes.value << ", expected: " << n << '\n';
        return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
    }

    return Utils::Error();
}

Utils::Error Tests::TestVector(size_t n)
{
    Vector<int> vec;

    for (size_t i = 0; i < n; i++)
        vec.PushBack((i + 1) * 10);

    for (size_t i = 0; i < n; i++)
        if (vec[i] != (int)(i + 1) * 10)
        {
            std::cout << "Got: " << vec[i] << ", expected: " <<
                          (int)(i + 1) * 10 << '\n';
            return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
        }

    return Utils::Error();
}

Utils::Error Tests::TestList(size_t n)
{
    LinkedList<int> list;

    list.StartLogging("../log");

    for (size_t i = 0; i < n; i++)
        list.PushBack((i + 1) * 10);

    list.Dump();

    for (size_t i = 1; i < n; i++)
        if (list[i] != (int)(i) * 10)
        {
            std::cout << list[i] << " != " << (int)(i) * 10 << '\n';
            return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
        }

    for (size_t i = 0; i < n; i++)
    {
        Utils::Result<int> res = list.Pop();
        RETURN_ERROR(res);

        if (res.value != ((int)n - (int)i) * 10)
        {
            std::cout << res.value << " != " << (n - i) * 10 << '\n';
            return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);
        }
    }

    list.Dump();

    list.EndLogging();

    return Utils::Error();
}

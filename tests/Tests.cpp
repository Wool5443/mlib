#include "Tests.hpp"

using namespace mlib;

Utils::Error Tests::TestString(size_t n)
{
    String a{"Hello"};
    String b{"World"};

    String c = a + b;

    String correct = "HelloWorld";

    if (c != correct)
        return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);

    String bebra;

    for (size_t i = 0; i < n; i++)
        bebra += "bebra";

    auto countRes = bebra.Count("bebra");
    RETURN_ERROR(countRes.error);

    if (countRes.value != n)
        return CREATE_ERROR(Utils::ErrorCode::ERROR_BAD_VALUE);

    return Utils::Error();
}

Utils::Error Tests::TestVector(size_t n)
{
    return Utils::Error();
}

Utils::Error Tests::TestList  (size_t n)
{
    return Utils::Error();
}

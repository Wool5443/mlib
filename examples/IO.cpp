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

    std::string_view s1{"8A hello"};
    std::string_view s2{"1!24 hello"};

    auto a = ParseNumber<int>(s1, 16);
    if (a.IsError())
    {
        GlobalLogError(a.Error(), "Could not parse \"{}\"", s1);
        return a.Error();
    }
    fmt::println("a = {}", *a);

    auto b = ParseNumber<int>(s2, 16);
    if (b.IsError())
    {
        GlobalLogError(b.Error(), "Could not parse \"{}\"", s2);
        return b.Error();
    }
    fmt::println("b = {}", *b);

    return 0;
}

#include "Result.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

using namespace mlib;
using namespace err;
using namespace std;

int main()
{
    const char* file = "../../file.txt";

    Result<string> text = read_file(file);

    if (!text)
    {
        global_log_error(text.error());
        return text.error();
    }

    vector<string_view> words = split_string(*text);

    if (words.size() != 923)
    {
        global_log_error(ERROR_BAD_VALUE, "Wrong number of words!!!: %zu\n", words.size());
    }

    for (auto it = words.rbegin(), end = words.rend(); it != end; ++it)
        std::cout << *it << '\n';

    std::string_view s1{"8A hello"};
    std::string_view s2{"1!24 hello"};

    auto a = parse_integer<int>(s1, 16);
    if (a.error())
    {
        global_log_error(a.error(), "Could not parse \"{}\"", s1);
        return a.error();
    }
    fmt::println("a = {}", *a);

    auto b = parse_integer<int>(s2, 16);
    if (b.error())
    {
        global_log_error(b.error(), "Could not parse \"{}\"", s2);
        return b.error();
    }
    fmt::println("b = {}", *b);

    return 0;
}

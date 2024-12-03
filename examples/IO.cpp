//NOLINTBEGIN

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

    return 0;
}

//NOLINTEND

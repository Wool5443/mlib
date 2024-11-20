//NOLINTBEGIN

#include "Error.hpp"
#include "Utils.hpp"

using namespace err;
using namespace mlib;
using namespace std;

LOG_INIT_FILE("../../logIO.txt");

int main()
{
    const char* file = "../../../file.txt";

    Result<string> text = ReadFileToBuf(file);

    RETURN_ERROR_IF(text);

    Result<vector<string_view>> words = SplitString(*text);

    RETURN_ERROR_IF(words);

    if (words->size() != 923)
        LOG_ERROR(ERROR_BAD_VALUE, "Wrong number of words!!!: %zu\n", words->size());

    for (auto it = words->rbegin(), end = words->rend(); it != end; ++it)
        std::cout << *it << '\n';

    return 0;
}

//NOLINTEND

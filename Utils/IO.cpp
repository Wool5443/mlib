#include "Utils.hpp"

using namespace err;
using namespace mlib;
using namespace std;

LOG_INIT_FILE("../../logIO.txt");

int main()
{
    const char* file = "../../file.txt";

    Result<string> text = ReadFileToBuf(file);

    RETURN_ERROR_IF(text);

    Result<vector<string_view>> lines = SplitString(*text, "\n");

    RETURN_ERROR_IF(lines);

    for (auto it = lines->rbegin(), end = lines->rend(); it != end; ++it)
        std::cout << *it << '\n';

    return 0;
}

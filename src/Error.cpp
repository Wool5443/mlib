#include <iostream>
#include "Error.hpp"

namespace {

const char* ERROR_CODE_NAMES[] = {

#define DEF_ERROR(code) \
#code,

#include "ErrorGen.hpp"

#undef DEF_ERROR

};

enum class ConsoleColor
{
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
};

void SetConsoleColor(FILE* file, ConsoleColor color)
{
    fprintf(file, "\033[0;%dm", static_cast<int>(color));
}

void __attribute__((unused)) SetConsoleColor(std::ostream& out, ConsoleColor color)
{
    out << "\033[0;" << (int)color << "m";
}

} // namespace

const char* err::GetErrorName(err::ErrorCode err)
{
    return ERROR_CODE_NAMES[static_cast<std::size_t>(err)];
}

const char* err::Error::GetErrorName() const noexcept
{
    return ERROR_CODE_NAMES[static_cast<std::size_t>(code)];
}

void err::Error::Print(FILE* file) const noexcept
{
    if (file == stderr || file == stdout)
        SetConsoleColor(file, code ? ConsoleColor::RED :
                                     ConsoleColor::GREEN);

    fprintf(file, "%s in %s:%zu in %s\n",
            GetErrorName(),
            this->file,
            line,
            function
    );

    if (file == stderr || file == stdout)
        SetConsoleColor(file, ConsoleColor::WHITE);
}

void err::Logger::PushErrorLogPleaseUseMacro(Error&& error)
{
    if (length == MAX_ERRORS)
        Dump();

    errorStack[length++] = error;
}

void err::Logger::Dump()
{
    for (std::size_t i = 0; i < length; i++)
        errorStack[i].Print(logFile);
    length = 0;
}

err::Logger::~Logger()
{
    if (length > 0)
        std::cerr << '\n' << length << " items were dumped\n";
    Dump();
}

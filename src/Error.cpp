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

void SetConsoleColor(std::ostream& out, ConsoleColor color)
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

void err::Error::Print(std::ostream& out) const noexcept
{
    if (&out == &std::cerr || &out == &std::cout)
        SetConsoleColor(out, code ? ConsoleColor::RED :
                                    ConsoleColor::GREEN);

    if (isMessage)
        out << message << '\n';
    else
        out << GetErrorName() << " in " << file <<
        ":" << line << " in " << function << '\n';

    if (&out == &std::cerr || &out == &std::cout)
        SetConsoleColor(out, ConsoleColor::WHITE);
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
        errorStack[i].Print(logStream);
    length = 0;
}

err::Logger::~Logger()
{
    if (length > 0)
        std::cerr << '\n' << length << " items were dumped\n";
    Dump();
}

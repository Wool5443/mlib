#include <iostream>
#include "Errors.hpp"

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
        SetConsoleColor(std::cout, code ? ConsoleColor::RED :
                                          ConsoleColor::GREEN);

    out << GetErrorName() << " in " << file <<
    ":" << line << " in " << function << '\n';

    if (&out == &std::cerr || &out == &std::cout)
        SetConsoleColor(std::cout, ConsoleColor::WHITE);
}

void err::Logger::PushErrorLogPleaseUseMacro(Error&& error)
{
    if (length == MAX_ERRORS)
        return;

    m_errorStack[length++] = error;
}

void err::Logger::PrintErrors(std::ostream& out)
{
    for (std::size_t i = 0; i < length; i++)
        m_errorStack[i].Print(out);
}

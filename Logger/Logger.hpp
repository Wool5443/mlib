/**
 * @file Logger.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Simple logger
 *
 * @version 3.0
 * @date 03.12.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

// NOLINTBEGIN

#ifndef MLIB_LOGGER_HPP
#define MLIB_LOGGER_HPP

#include <cassert>
#include <chrono>
#include <iomanip>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "include/File.hpp"
#include "include/ConsoleColor.hpp"
#include "include/SourcePosition.hpp"
#include "include/ErrorCode.hpp"

namespace mlib {

class Logger
{
    using TimePoint = std::chrono::system_clock::time_point;
public:
    enum LogType
    {
        INFO,
        DEBUG,
        ERROR,
    };

    virtual ~Logger()
    {
        std::fflush(m_logFile);
    }

    Logger() = default;

    explicit Logger(FILE* logFile = stderr) noexcept
        : m_logFile(logFile)
    {
        std::setbuf(m_logFile, nullptr);
    }

    Logger(const char* logFilePath) noexcept
        : Logger(fopen(logFilePath, "w")) {}

    template<class... Args>
    void Log(LogType type, err::ErrorCode errorCode, SourcePosition position,
             TimePoint time,
             const char* formatString = nullptr, Args&&... args) noexcept
    {
        printType(type);

        std::time_t t = std::chrono::system_clock::to_time_t(time);
        std::tm tm = *std::localtime(&t);

        fmt::print(m_logFile, " {}:", fmt::streamed(std::put_time(&tm, "%d/%m/%Y %T %Z")));

        if (errorCode)
        {
            fmt::print(m_logFile, " {}:{}", err::GetErrorName(errorCode), static_cast<int>(errorCode));
        }

        fmt::println(m_logFile,
                  " {}:{} in {}",
                  position.GetFileName(),
                  position.GetLine(),
                  position.GetFunctionName()
        );

        if (formatString)
        {
            fmt::println(m_logFile, formatString, std::forward<Args>(args)...);
        }

        fmt::print(m_logFile, "\n");

        SetConsoleColor(m_logFile, ConsoleColor::WHITE);
    }

    static Logger& GetGlobalLogger() noexcept
    {
        static Logger globalLogger{stderr};

        return globalLogger;
    }

    static void SetGlobalLoggerLogFile(FILE* newLogFile) noexcept
    {
        assert(newLogFile && "Bad log file!!!");

        Logger& glogger = GetGlobalLogger();

        glogger.~Logger();

        glogger.m_logFile.m_file = newLogFile;

        std::setbuf(glogger.m_logFile, nullptr);
    }

    static void SetGlobalLoggerLogFile(const char* newLogFilePath) noexcept
    {
        SetGlobalLoggerLogFile(fopen(newLogFilePath, "w"));
    }

private:
    File m_logFile = nullptr;

    void printType(LogType type) noexcept
    {
        switch (type)
        {
            case INFO:
                SetConsoleColor(m_logFile, ConsoleColor::CYAN);
                fmt::print(m_logFile, "[INFO]");
                break;
            case DEBUG:
                SetConsoleColor(m_logFile, ConsoleColor::YELLOW);
                fmt::print(m_logFile, "[DEBUG]");
                break;
            case ERROR:
                SetConsoleColor(m_logFile, ConsoleColor::RED);
                fmt::print(m_logFile, "[ERROR]");
                break;
            default:
                fmt::print(m_logFile, "[UNKNOWN LOG TYPE]");
                break;
        }
    }
};

static inline Logger& GetGlobalLogger()
{
    return Logger::GetGlobalLogger();
}


static inline void SetGlobalLoggerLogFile(const char* newLogFilePath)
{
    return Logger::SetGlobalLoggerLogFile(newLogFilePath);
}

static inline void SetGlobalLoggerLogFile(FILE* newLogFile)
{
    return Logger::SetGlobalLoggerLogFile(newLogFile);
}

} // namespace mlib

#define Log(type, errorCode, ...) \
Log(type, errorCode, CURRENT_SOURCE_POSITION(), std::chrono::system_clock::now() __VA_OPT__(, __VA_ARGS__))

#define LogInfo(...) Log(mlib::Logger::INFO, mlib::err::EVERYTHING_FINE __VA_OPT__(, __VA_ARGS__))
#define LogDebug(...) Log(mlib::Logger::DEBUG, mlib::err::EVERYTHING_FINE __VA_OPT__(, __VA_ARGS__))
#define LogError(errorCode, ...) Log(mlib::Logger::ERROR, errorCode __VA_OPT__(, __VA_ARGS__))

#define GlobalLog(...) mlib::Logger::GetGlobalLogger(). Log(__VA_ARGS__)
#define GlobalLogInfo(...) mlib::Logger::GetGlobalLogger(). LogInfo(__VA_ARGS__)
#define GlobalLogDebug(...) mlib::Logger::GetGlobalLogger(). LogDebug(__VA_ARGS__)
#define GlobalLogError(errorCode, ...) mlib::Logger::GetGlobalLogger(). LogError(errorCode __VA_OPT__(, __VA_ARGS__))

#endif // MLIB_LOGGER_HPP

// NOLINTEND

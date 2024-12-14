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
#include <mutex>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "details/File.hpp"
#include "details/ConsoleColor.hpp"
#include "details/SourcePosition.hpp"
#include "details/ErrorCode.hpp"

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

    virtual ~Logger() = default;

    Logger() = default;

    explicit Logger(FILE* logFile = stderr) noexcept
        : m_logFile(logFile)
    {
        std::setbuf(m_logFile, nullptr);
    }

    explicit Logger(const char* logFilePath) noexcept
        : Logger(std::fopen(logFilePath, "w")) {}

    explicit Logger(std::nullptr_t) noexcept {}

    template<class... Args>
    void Log(LogType type, err::ErrorCode errorCode,
             detail::SourcePosition position, TimePoint time,
             const char* formatString = nullptr, Args&&... args) noexcept
    {
        if (!m_logFile) return;

        std::unique_lock lock(m_mutex);

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

        SetConsoleColor(m_logFile, detail::ConsoleColor::WHITE);
    }

    static Logger& GetGlobalLogger() noexcept
    {
        static Logger globalLogger{stderr};

        return globalLogger;
    }

    static void SetGlobalLoggerLogFile(FILE* newLogFile) noexcept
    {
        Logger& glogger = GetGlobalLogger();

        glogger.~Logger();

        glogger.m_logFile.m_file = newLogFile;

        if (newLogFile)
        {
            std::setbuf(newLogFile, nullptr);
        }
    }

    static void SetGlobalLoggerLogFile(const char* newLogFilePath) noexcept
    {
        SetGlobalLoggerLogFile(std::fopen(newLogFilePath, "w"));
    }

    static void SetGlobalLoggerLogFile(std::nullptr_t) noexcept
    {
        SetGlobalLoggerLogFile(static_cast<FILE*>(nullptr));
    }

private:
    detail::File m_logFile{nullptr};
    std::mutex m_mutex{};

    void printType(LogType type) noexcept
    {
        switch (type)
        {
            case INFO:
                SetConsoleColor(m_logFile, detail::ConsoleColor::CYAN);
                fmt::print(m_logFile, "[INFO]");
                break;
            case DEBUG:
                SetConsoleColor(m_logFile, detail::ConsoleColor::YELLOW);
                fmt::print(m_logFile, "[DEBUG]");
                break;
            case ERROR:
                SetConsoleColor(m_logFile, detail::ConsoleColor::RED);
                fmt::print(m_logFile, "[ERROR]");
                break;
            default:
                fmt::print(m_logFile, "[UNKNOWN LOG TYPE]");
                break;
        }
    }
};

inline Logger& GetGlobalLogger()
{
    return Logger::GetGlobalLogger();
}


inline void SetGlobalLoggerLogFile(const char* newLogFilePath)
{
    Logger::SetGlobalLoggerLogFile(newLogFilePath);
}

inline void SetGlobalLoggerLogFile(FILE* newLogFile)
{
    Logger::SetGlobalLoggerLogFile(newLogFile);
}

inline void SetGlobalLoggerLogFile(std::nullptr_t)
{
    Logger::SetGlobalLoggerLogFile(nullptr);
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

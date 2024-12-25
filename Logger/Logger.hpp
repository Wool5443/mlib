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
#include <iomanip> // IWYU pragma: keep
#include <mutex>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "details/File.hpp"
#include "details/ConsoleColor.hpp"
#include "details/SourcePosition.hpp"
#include "details/ErrorCode.hpp"

namespace mlib {

/**
 * @class Logger
 *
 * @brief Simple class for logging. Thread-safe
 *
 * It is possible to create local instances of Logger
 * or one can use a singleton global logger.
 *
 */
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

    /**
     * @brief Logger(FILE* logFile = stderr)
     *
     * @param [in] logFile
     */
    explicit Logger(FILE* logFile = stderr) noexcept
        : m_logFile(logFile)
    {
#ifndef DISABLE_LOGGING
        std::setbuf(m_logFile, nullptr);
#endif // ifndef DISABLE LOGGING
    }

    /**
     * @brief Logger(const char* logFilePath)
     * opens a file at the given path
     *
     * @param [in] logFilePath
     */
    explicit Logger(const char* logFilePath) noexcept
        : Logger(std::fopen(logFilePath, "w")) {}


    /**
     * @brief Disable logger
     */
    explicit Logger(std::nullptr_t) noexcept {}

    /**
     * @brief Sets log file
     *
     * @param [in] newLogFile
     */
    void SetLogFile(FILE* newLogFile) noexcept
    {
#ifndef DISABLE_LOGGING
        m_logFile.Flush();

        m_logFile.m_file = newLogFile;

        if (newLogFile)
        {
            std::setbuf(newLogFile, nullptr);
        }
#endif // ifndef DISABLE LOGGING
    }

    /**
     * @brief Opens a file and sets the logger log file
     *
     * @param [in] newLogFilePath
     */
    void SetLogFile(const char* newLogFilePath) noexcept
    {
#ifndef DISABLE_LOGGING
        SetLogFile(fopen(newLogFilePath, "w"));
#endif // ifndef DISABLE LOGGING
    }

    /**
     * @brief Disable logger
     */
    void SetLogFile(std::nullptr_t) noexcept
    {
#ifndef DISABLE_LOGGING
        SetLogFile(static_cast<FILE*>(nullptr));
#endif // ifndef DISABLE LOGGING
    }

    /**
     * @brief Function that actually logs stuff.
     * Do not use it because source position and time are collected
     * using macros
     *
     * @tparam Args
     *
     * @param [in] type
     * @param [in] errorCode
     * @param [in] position
     * @param [in] time
     * @param [in] formatString
     * @param [in] args
     */
    template<class... Args>
    void Log(LogType type, err::ErrorCode errorCode,
             detail::SourcePosition position, TimePoint time,
             const char* formatString = nullptr, Args&&... args)
    {
#ifndef DISABLE_LOGGING
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
            fmt::println(m_logFile, fmt::runtime(formatString), std::forward<Args>(args)...);
        }

        fmt::print(m_logFile, "\n");

        SetConsoleColor(m_logFile, detail::ConsoleColor::WHITE);
#endif
    }

private:
    detail::File m_logFile{nullptr};
    std::mutex m_mutex{};

    void printType(LogType type) noexcept
    {
#ifndef DISABLE_LOGGING
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
#endif // ifndef DISABLE LOGGING
    }
};

/**
 * @brief Get global logger instance. By default logs to stderr
 *
 * @return Logger& global logger
 */
inline Logger& GetGlobalLogger()
{
#ifndef DISABLE_LOGGING
    static Logger globalLogger{stderr};

    return globalLogger;
#endif // ifndef DISABLE LOGGING
}

/**
 * @brief Sets global logger log file
 *
 * @param [in] newLogFile
 */
inline void SetGlobalLoggerLogFile(FILE* newLogFile)
{
#ifndef DISABLE_LOGGING
    GetGlobalLogger().SetLogFile(newLogFile);
#endif // ifndef DISABLE LOGGING
}

/**
 * @brief Opens a file and sets the globall logger log file
 *
 * @param [in] newLogFilePath
 */
inline void SetGlobalLoggerLogFile(const char* newLogFilePath)
{
#ifndef DISABLE_LOGGING
    GetGlobalLogger().SetLogFile(newLogFilePath);
#endif // ifndef DISABLE LOGGING
}

/**
 * @brief Disables global logger
 */
inline void SetGlobalLoggerLogFile(std::nullptr_t)
{
#ifndef DISABLE_LOGGING
    GetGlobalLogger().SetLogFile(nullptr);
#endif // ifndef DISABLE LOGGING
}

} // namespace mlib

#define Log(type, errorCode, ...) \
Log(type, errorCode, CURRENT_SOURCE_POSITION(), std::chrono::system_clock::now() __VA_OPT__(, __VA_ARGS__))

#define LogInfo(...) Log(mlib::Logger::INFO, mlib::err::EVERYTHING_FINE __VA_OPT__(, __VA_ARGS__))
#define LogDebug(...) Log(mlib::Logger::DEBUG, mlib::err::EVERYTHING_FINE __VA_OPT__(, __VA_ARGS__))
#define LogError(errorCode, ...) Log(mlib::Logger::ERROR, errorCode __VA_OPT__(, __VA_ARGS__))

#ifndef DISABLE_LOGGING

#define GlobalLog(...) mlib::GetGlobalLogger(). Log(__VA_ARGS__)
#define GlobalLogInfo(...) mlib::GetGlobalLogger(). LogInfo(__VA_ARGS__)
#define GlobalLogDebug(...) mlib::GetGlobalLogger(). LogDebug(__VA_ARGS__)
#define GlobalLogError(errorCode, ...) mlib::GetGlobalLogger(). LogError(errorCode __VA_OPT__(, __VA_ARGS__))

#else

#define GlobalLog(...)
#define GlobalLogInfo(...)
#define GlobalLogDebug(...)
#define GlobalLogError(...)

#endif // ifndef DISABLE_LOGGING

#endif // MLIB_LOGGER_HPP

// NOLINTEND

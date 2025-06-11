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

#ifndef MLIB_LOGGER_HPP
#define MLIB_LOGGER_HPP

#include <cassert>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "details/File.hpp"
#include "details/Console_color.hpp"
#include "details/Source_position.hpp"
#include "details/Error_code.hpp"

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
    explicit Logger(FILE* log_file = stderr) noexcept
        : m_log_file(log_file)
    {
#ifndef DISABLE_LOGGING
        setbuf(m_log_file, nullptr);
#endif // ifndef DISABLE LOGGING
    }

    /**
     * @brief Logger(const char* logFilePath)
     * opens a file at the given path
     *
     * @param [in] logFilePath
     */
    explicit Logger(const char* log_file_path) noexcept
        : Logger(std::fopen(log_file_path, "w")) {}


    /**
     * @brief Disable logger
     */
    explicit Logger(std::nullptr_t) noexcept {}

    /**
     * @brief Sets log file
     *
     * @param [in] newLogFile
     */
    void set_log_file(FILE* new_log_file) noexcept
    {
#ifndef DISABLE_LOGGING
        m_log_file.flush();

        m_log_file.m_file = new_log_file;

        if (new_log_file)
        {
            setbuf(new_log_file, nullptr);
        }
#endif // ifndef DISABLE LOGGING
    }

    /**
     * @brief Opens a file and sets the logger log file
     *
     * @param [in] new_log_file_path
     */
    void set_log_file(const char* new_log_file_path) noexcept
    {
#ifndef DISABLE_LOGGING
        set_log_file(fopen(new_log_file_path, "w"));
#endif // ifndef DISABLE LOGGING
    }

    /**
     * @brief Disable logger
     */
    void set_log_file(std::nullptr_t) noexcept
    {
#ifndef DISABLE_LOGGING
        set_log_file(static_cast<FILE*>(nullptr));
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
    void log(
        LogType type, err::Error_code error_code,
        detail::Source_position position, TimePoint time,
        const char* format_string = nullptr, Args&&... args
    )
    {
#ifndef DISABLE_LOGGING
        if (!m_log_file) return;

        std::unique_lock lock(m_mutex);

        printType(type);

        time_t t = std::chrono::system_clock::to_time_t(time);
        tm tm = *std::localtime(&t);

        fmt::print(m_log_file, " {}:", fmt::streamed(std::put_time(&tm, "%d/%m/%Y %T %Z")));

        if (error_code)
        {
            fmt::print(m_log_file, " {}:{}", err::get_error_name(error_code), static_cast<int>(error_code));
        }

        fmt::println(m_log_file,
                     " {}:{} in {}",
                     position.get_file_name(),
                     position.get_line(),
                     position.get_function_name()
        );

        if (format_string)
        {
            fmt::println(m_log_file, fmt::runtime(format_string), std::forward<Args>(args)...);
        }

        fmt::print(m_log_file, "\n");

        set_console_color(m_log_file, detail::Console_color::WHITE);
#endif
    }

private:
    detail::File m_log_file{nullptr};
    std::mutex m_mutex{};

    void printType(LogType type) noexcept
    {
#ifndef DISABLE_LOGGING
        switch (type)
        {
            case INFO:
                set_console_color(m_log_file, detail::Console_color::CYAN);
                fmt::print(m_log_file, "[INFO]");
                break;
            case DEBUG:
                set_console_color(m_log_file, detail::Console_color::YELLOW);
                fmt::print(m_log_file, "[DEBUG]");
                break;
            case ERROR:
                set_console_color(m_log_file, detail::Console_color::RED);
                fmt::print(m_log_file, "[ERROR]");
                break;
            default:
                fmt::print(m_log_file, "[UNKNOWN LOG TYPE]");
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
inline Logger& get_global_logger()
{
#ifndef DISABLE_LOGGING
    static Logger global_logger{stderr};

    return global_logger;
#endif // ifndef DISABLE LOGGING
}

/**
 * @brief Sets global logger log file
 *
 * @param [in] newLogFile
 */
inline void set_global_logger_log_file(FILE* newLogFile)
{
#ifndef DISABLE_LOGGING
    get_global_logger().set_log_file(newLogFile);
#endif // ifndef DISABLE LOGGING
}

/**
 * @brief Opens a file and sets the globall logger log file
 *
 * @param [in] newLogFilePath
 */
inline void set_global_logger_log_file(const char* newLogFilePath)
{
#ifndef DISABLE_LOGGING
    get_global_logger().set_log_file(newLogFilePath);
#endif // ifndef DISABLE LOGGING
}

/**
 * @brief Disables global logger
 */
inline void set_global_logger_log_file(std::nullptr_t)
{
#ifndef DISABLE_LOGGING
    get_global_logger().set_log_file(nullptr);
#endif // ifndef DISABLE LOGGING
}

} // namespace mlib

#define log(type, error_code, ...) \
log(type, error_code, CURRENT_SOURCE_POSITION(), std::chrono::system_clock::now() __VA_OPT__(, __VA_ARGS__))

#define log_info(...) log(mlib::Logger::INFO, mlib::err::EVERYTHING_FINE __VA_OPT__(, __VA_ARGS__))
#define log_debug(...) log(mlib::Logger::DEBUG, mlib::err::EVERYTHING_FINE __VA_OPT__(, __VA_ARGS__))
#define log_error(error_code, ...) log(mlib::Logger::ERROR, error_code __VA_OPT__(, __VA_ARGS__))

#ifndef DISABLE_LOGGING

#define global_log(...) mlib::get_global_logger(). log(__VA_ARGS__)
#define global_log_info(...) mlib::get_global_logger(). log_info(__VA_ARGS__)
#define global_log_debug(...) mlib::get_global_logger(). log_debug(__VA_ARGS__)
#define global_log_error(error_code, ...) mlib::get_global_logger(). log_error(error_code __VA_OPT__(, __VA_ARGS__))

#else

#define global_log(...)
#define global_log_info(...)
#define global_log_debug(...)
#define global_log_error(...)

#endif // ifndef DISABLE_LOGGING

#endif // MLIB_LOGGER_HPP

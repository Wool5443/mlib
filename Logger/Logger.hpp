/**
 * @file Logger.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief A simple logger that supports logging to a file or the console.
 *
 * This header defines a `Logger` class that provides thread-safe logging functionality.
 * The logger supports different log levels (INFO, DEBUG, ERROR) and can log to either the
 * console or a specified log file. The class also includes macros for convenient logging,
 * as well as a global logger for centralized logging management.
 *
 * @version 1.0
 * @date 12.06.2025
 *
 * @copyright Copyright (c) 2025
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
 * @brief A simple thread-safe logger for logging messages to a file or console.
 *
 * The `Logger` class provides functionality to log messages with various log levels
 * (INFO, DEBUG, ERROR) and supports both console and file-based logging. It also provides
 * thread-safe logging by using a mutex to protect concurrent log writes. The log messages
 * include timestamp, error code (if applicable), file name, function name, and line number.
 */
class Logger
{
    using TimePoint = std::chrono::system_clock::time_point;

public:
    /**
     * @enum LogType
     * @brief Specifies the different log levels for logging.
     */
    enum LogType
    {
        INFO,   ///< Informational log messages
        DEBUG,  ///< Debug log messages
        ERROR,  ///< Error log messages
    };

    /**
     * @brief Destructor for the `Logger` class.
     *
     * The destructor cleans up any resources allocated by the logger, including
     * flushing and closing the log file if necessary.
     */
    virtual ~Logger() = default;

    /**
     * @brief Constructs a logger with an optional log file.
     *
     * The constructor initializes the logger to log messages to the specified `FILE*`.
     * If no file is provided, it defaults to `stderr`.
     *
     * @param log_file The log file where messages will be written (default is `stderr`).
     */
    explicit Logger(FILE* log_file = stderr) noexcept
        : m_log_file(log_file)
    {
#ifndef DISABLE_LOGGING
        setbuf(m_log_file, nullptr); // Disable buffering for log file
#endif // ifndef DISABLE_LOGGING
    }

    /**
     * @brief Constructs a logger with a log file path.
     *
     * The constructor opens the specified file path for logging and sets it as the
     * log file.
     *
     * @param log_file_path Path to the log file.
     */
    explicit Logger(const char* log_file_path) noexcept
        : Logger(std::fopen(log_file_path, "w")) {}

    /**
     * @brief Disables the logger.
     *
     * This constructor disables the logger by not associating it with any log file.
     */
    explicit Logger(std::nullptr_t) noexcept {}

    /**
     * @brief Sets a new log file.
     *
     * This function allows changing the log file at runtime.
     * The log file will be flushed, and the new file will be set.
     *
     * @param new_log_file The new log file to set.
     */
    void set_log_file(FILE* new_log_file) noexcept
    {
#ifndef DISABLE_LOGGING
        m_log_file.flush();
        m_log_file.m_file = new_log_file;

        if (new_log_file)
        {
            setbuf(new_log_file, nullptr); // Disable buffering for new log file
        }
#endif // ifndef DISABLE_LOGGING
    }

    /**
     * @brief Sets a log file based on the specified path.
     *
     * This function opens the log file at the specified path and sets it as the
     * current log file.
     *
     * @param new_log_file_path Path to the new log file.
     */
    void set_log_file(const char* new_log_file_path) noexcept
    {
#ifndef DISABLE_LOGGING
        set_log_file(fopen(new_log_file_path, "w"));
#endif // ifndef DISABLE_LOGGING
    }

    /**
     * @brief Disables the logger by setting the log file to `nullptr`.
     */
    void set_log_file(std::nullptr_t) noexcept
    {
#ifndef DISABLE_LOGGING
        set_log_file(static_cast<FILE*>(nullptr));
#endif // ifndef DISABLE_LOGGING
    }

    /**
     * @brief Logs a message with the specified parameters.
     *
     * This function logs a message with the given log type, error code, source position,
     * timestamp, and an optional format string. It is intended to be used internally, as
     * macros are used to collect source position and time.
     *
     * @tparam Args Variadic arguments for the format string.
     * @param type The log type (INFO, DEBUG, ERROR).
     * @param error_code The error code (optional).
     * @param position The source position (file, function, line number).
     * @param time The timestamp when the log message was created.
     * @param format_string The optional format string for the message.
     * @param args The arguments for the format string.
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

        std::unique_lock lock(m_mutex); // Thread-safe logging

        printType(type);

        time_t t = std::chrono::system_clock::to_time_t(time);
        tm tm = *std::localtime(&t);

        // Print timestamp
        fmt::print(m_log_file, " {}:", fmt::streamed(std::put_time(&tm, "%d/%m/%Y %T %Z")));

        // Print error code if available
        if (error_code)
        {
            fmt::print(m_log_file, " {}:{}", err::get_error_name(error_code), static_cast<int>(error_code));
        }

        // Print source position (file, line, function)
        fmt::println(m_log_file,
                     " {}:{} in {}",
                     position.get_file_name(),
                     position.get_line(),
                     position.get_function_name()
        );

        // Print the formatted message
        if (format_string)
        {
            fmt::println(m_log_file, fmt::runtime(format_string), std::forward<Args>(args)...);
        }

        fmt::print(m_log_file, "\n");

        set_console_color(m_log_file, detail::Console_color::WHITE);
#endif
    }

private:
    detail::File m_log_file{nullptr}; ///< The log file stream.
    std::mutex m_mutex{}; ///< Mutex for thread-safe logging.

    /**
     * @brief Prints the log type.
     *
     * This function prints the log type (INFO, DEBUG, ERROR) in the appropriate color
     * and format.
     *
     * @param type The log type to print.
     */
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
#endif // ifndef DISABLE_LOGGING
    }
};

/**
 * @brief Get the global logger instance. By default, logs to `stderr`.
 *
 * This function returns the singleton instance of the logger. It initializes the logger
 * to log messages to `stderr` if not already initialized.
 *
 * @return The global logger instance.
 */
inline Logger& get_global_logger()
{
#ifndef DISABLE_LOGGING
    static Logger global_logger{stderr}; // Global logger initialized to `stderr`
    return global_logger;
#endif // ifndef DISABLE_LOGGING
}

/**
 * @brief Sets the global logger's log file.
 *
 * This function sets the log file for the global logger instance.
 *
 * @param newLogFile The new log file to set.
 */
inline void set_global_logger_log_file(FILE* newLogFile)
{
#ifndef DISABLE_LOGGING
    get_global_logger().set_log_file(newLogFile);
#endif // ifndef DISABLE_LOGGING
}

/**
 * @brief Sets the global logger's log file using a file path.
 *
 * This function opens the log file at the specified path and sets it as the global logger's log file.
 *
 * @param newLogFilePath The path to the log file.
 */
inline void set_global_logger_log_file(const char* newLogFilePath)
{
#ifndef DISABLE_LOGGING
    get_global_logger().set_log_file(newLogFilePath);
#endif // ifndef DISABLE_LOGGING
}

/**
 * @brief Disables the global logger.
 *
 * This function disables the global logger by setting its log file to `nullptr`.
 */
inline void set_global_logger_log_file(std::nullptr_t)
{
#ifndef DISABLE_LOGGING
    get_global_logger().set_log_file(nullptr);
#endif // ifndef DISABLE_LOGGING
}

} // namespace mlib

/**
 * @brief Log macro for logging messages with the specified log type and error code.
 *
 * These macros simplify logging messages with different log levels (INFO, DEBUG, ERROR) by
 * automatically capturing the current source position and timestamp.
 */
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

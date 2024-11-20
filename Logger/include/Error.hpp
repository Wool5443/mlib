//NOLINTBEGIN

#ifndef MLIB_ERRORS_HPP
#define MLIB_ERRORS_HPP

/**
 * @file Error.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Simple error and logging system
 *
 * @version 3.0
 * @date 30.09.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <cassert>
#include <cstdio>
#include <ctime>
#include <iterator>
#include <new> // IWYU pragma: keep
#include <type_traits>
#include <unistd.h>
#include "File.hpp"

#define IS_EMPTY(dummy, ...) ( sizeof( (char[]){#__VA_ARGS__} ) == 1 )

#define GET_FILE_NAME() __FILE__
#define GET_LINE()      __LINE__
#define GET_FUNCTION()  __PRETTY_FUNCTION__

#define CREATE_ERROR(errorCode) \
err::Error((errorCode), GET_FILE_NAME(), GET_LINE(), GET_FUNCTION())

#define _LOG_DONT_USE_(type, color, errorCode, ...)                 \
do                                                                  \
{                                                                   \
    if (!err::LOGGER_) break;                                       \
    mlib::File& _logFile_ = err::LOGGER_->m_logFile;                \
    if (!_logFile_) break;                                          \
    SetConsoleColor(_logFile_, (color));                            \
    fprintf(_logFile_, "[" type "] ");                              \
    CREATE_ERROR(errorCode).Print(_logFile_);                       \
    __VA_OPT__(fprintf(_logFile_, ": ");)                           \
    __VA_OPT__(fprintf(_logFile_, __VA_ARGS__);)                    \
    fprintf(_logFile_, "\n");                                       \
    SetConsoleColor(_logFile_, err::ConsoleColor::WHITE);           \
    err::LOGGER_->m_countItems++;                                   \
} while (0)

/**
  * @brief Log info
  */
#define LOG_INFO(...) \
_LOG_DONT_USE_("INFO", err::ConsoleColor::CYAN, err::EVERYTHING_FINE, __VA_ARGS__)

/**
  * @brief Log debug info
  */
#ifndef NDEBUG
#define LOG_DEBUG(...) \
_LOG_DONT_USE_("DEBUG", err::ConsoleColor::YELLOW, err::EVERYTHING_FINE, __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

/**
 * @brief Log error
 */
#define LOG_ERROR(errorCode, ...) \
_LOG_DONT_USE_("ERROR", err::ConsoleColor::RED, errorCode, __VA_ARGS__)

/**
 * @brief Log error if it indeed is error
 */
#define LOG_ERROR_IF(errorCode, ...)                                \
do                                                                  \
{                                                                   \
    err::ErrorCode _errorCode_ = (errorCode);                       \
    if (_errorCode_)                                                \
        LOG_ERROR(_errorCode_, __VA_ARGS__);                        \
} while (0)

/**
 * @brief Initialize empty logger to disable logging
 */
#define LOG_DISABLE() err::Logger* LOGGER_ = nullptr

/**
 * @brief Initialize logger to log in stderr
 */
#define LOG_INIT_CONSOLE() LOG_INIT_FILE(stderr)

/**
 * @brief Initialize logger to log in fileStream
 */
#define LOG_INIT_FILE(file)                                         \
    err::Logger _logger_{file};                                     \
    err::Logger* err::LOGGER_ = &_logger_

#ifdef NDEBUG
#define ASSERT_PRINT(...)
#define SoftAssert(...)
#define SoftAssertResult(...)
#define HardAssert(...)
#else
#define ASSERT_PRINT(errorCode)                                     \
do                                                                  \
{                                                                   \
    fprintf(stderr, "ASSERTION FAILED: ");                          \
    CREATE_ERROR(errorCode).Print();                                \
    fprintf(stderr, "\n");                                          \
} while (0)

/**
 * @brief Soft assert
 */
#define SoftAssert(expression, errorCode, ...)                      \
do                                                                  \
{                                                                   \
    if (!(expression))                                              \
    {                                                               \
        err::ErrorCode _errorCode_ = errorCode;                     \
        ASSERT_PRINT(_errorCode_);                                  \
        __VA_ARGS__;                                                \
        return _errorCode_;                                         \
    }                                                               \
} while(0)

/**
 * @brief Soft assert for functions returning a result
 */
#define SoftAssertResult(expression, poison, errorCode, ...)        \
do                                                                  \
{                                                                   \
    if (!(expression))                                              \
    {                                                               \
        err::ErrorCode _errorCode_ = errorCode;                     \
        ASSERT_PRINT(_errorCode_);                                  \
        __VA_ARGS__;                                                \
        return { poison, _errorCode_ };                             \
    }                                                               \
} while(0)

/**
 * @brief Hard assert
 */
#define HardAssert(expression, errorCode, ...)                      \
do                                                                  \
{                                                                   \
    if (!(expression))                                              \
    {                                                               \
        err::ErrorCode _errorCode_ = errorCode;                     \
        ASSERT_PRINT(_errorCode_);                                  \
        __VA_ARGS__;                                                \
        exit(_errorCode_);                                          \
    }                                                               \
} while(0)

#endif

/**
 * @brief returns errror and logs
 */
#define RETURN_ERROR(errorCode, ...)                                \
do                                                                  \
{                                                                   \
    err::ErrorCode _errorCode_ = errorCode;                         \
    LOG_ERROR(_errorCode_);                                         \
    __VA_ARGS__;                                                    \
    return _errorCode_;                                             \
} while(0)

/**
 * @brief returns error if it is not EVERYTHING_FINE and logs
 */
#define RETURN_ERROR_IF(errorCode, ...)                             \
do                                                                  \
{                                                                   \
    err::ErrorCode _errorCode_ = errorCode;                         \
    if (_errorCode_)                                                \
        RETURN_ERROR(_errorCode_);                                  \
} while(0)

/**
 * @brief returns result if it contains an error and logs
 */
#define RETURN_RESULT_IF(result, ...)                               \
do                                                                  \
{                                                                   \
    auto _result_ = result;                                         \
    if (!_result_)                                                  \
    {                                                               \
        LOG_ERROR(_result_);                                        \
        __VA_ARGS__;                                                \
        return _result_;                                            \
    }                                                               \
} while(0)

namespace err {

/** @enum ErrorCode
 * @brief Represents possible error codes
 */
enum ErrorCode
{

#define DEF_ERROR(code) \
code,

#include "ErrorGen.hpp"

#undef DEF_ERROR

};

/** @enum ConsoleColor
 * @brief Represents colors for @see SetConsoleColor
 */
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

/**
 * @brief Set the console color
 *
 * @param [in] file stdout or stderr
 * @param [in] color
 */
static void SetConsoleColor(FILE* file, ConsoleColor color) noexcept
{
    assert(file);
    if (isatty(fileno(file)))
        fprintf(file, "\033[0;%dm", static_cast<int>(color));
}

/**
 * @brief Returns a string explaining the error
 *
 * @param [in] code
 *
 * @return const char* error name
 */
static const char* GetErrorName(ErrorCode code)
{
    static constexpr const char* ERROR_CODE_NAMES[] = {

    #define DEF_ERROR(code) \
    #code,

    #include "ErrorGen.hpp"

    #undef DEF_ERROR

    };

    return ERROR_CODE_NAMES[static_cast<size_t>(code)];
}

/** @struct Error
 * @brief Class for handling errors
 */
class Error
{
public:
    /**
     * @brief Construct a Error
     */
    Error() noexcept = default;

    /**
     * @brief Construct a new Error object
     *
     * @param [in] code
     * @param [in] file
     * @param [in] line
     * @param [in] function
     */
    Error(ErrorCode code, const char* file, size_t line,
          const char* function) noexcept
        : m_code(code), m_file(file), m_line(line), m_function(function) {}

    operator bool() const noexcept
    {
        return m_code;
    }
    operator int() const noexcept
    {
        return m_code;
    }
    operator ErrorCode() const noexcept
    {
        return m_code;
    }

    /**
     * @brief Get what the error means
     *
     * @return const char* name
     */
    const char* GetErrorName() const noexcept
    {
        return err::GetErrorName(m_code);
    }

    const char* what() const noexcept
    {
        return GetErrorName();
    }

    /**
     * @brief Prints error into a file
     *
     * @param [in] file where to print
     */
    void Print(FILE* file = stderr) const noexcept
    {
        assert(file);

        struct tm date = {};

        char timeString[std::size("dd-mm-yyyy:hh:mm:ss")];
        std::strftime(timeString, std::size(timeString), "%d-%m-%Y:%H:%M:%S",
                      std::localtime(&m_time));

        fprintf(file, "%s: ", timeString);

        if (m_code) fprintf(file, "%s ", GetErrorName());

        fprintf(file,
                "in %s:%zu in %s",
                m_file,
                m_line,
                m_function
        );
    }
private:
    time_t      m_time = time(NULL);
    ErrorCode   m_code = EVERYTHING_FINE;
    const char* m_file = nullptr;
    size_t      m_line = 0;
    const char* m_function = nullptr;
};

/** @struct Logger
 * @brief Class for a simple logging system
 */
class Logger
{
public:
    /**
     * @brief Construct a Logger
     *
     * @param [in] logFile
     */
    Logger(FILE* logFile) noexcept
        : m_logFile(logFile)
    {
        setbuf(m_logFile, NULL);
    }

    /**
     * @brief Construct a Logger
     *
     * @param [in] filePath
     */
    Logger(const char* filePath) noexcept
        : m_logFile(filePath, "w")
    {
        setbuf(m_logFile, NULL);
    }

    ~Logger() noexcept
    {
        if (m_countItems > 0)
        {
            fprintf(stderr, "\n%zu ", m_countItems);

            if (m_countItems == 1)
                fprintf(stderr, "item ");
            else
                fprintf(stderr, "items ");

            fprintf(stderr, "were dumped\n");
        }
    }

    size_t m_countItems;
    mlib::File m_logFile;
};

extern Logger* LOGGER_;

/** @struct Result
 * @brief Struct for returning errors with values
 *
 * @tparam T type of value
 */
template<typename T>
class Result
{
public:
    /**
     * @brief Construct an error Result
     *
     * @param error
     */
    Result(ErrorCode error) noexcept
        : m_error(error), m_ok(false)
    {
        new(&m_error) ErrorCode{error};
    }

    /**
     * @brief Construct a valid Result
     *
     * @tparam U perfect forwarding
     * @param value
     */
    template<typename U = T>
    Result(U&& value)
        : m_ok(true)
    {
        new(&m_value) T{std::forward<U>(value)};
    }

    operator bool()      const noexcept { return HasValue(); }
    operator ErrorCode() const noexcept { return Error();    }
    operator T() = delete;

    /**
     * @brief Check if it has value
     *
     * @return true
     * @return false
     */
    bool HasValue() const noexcept { return m_ok; }

    /**
     * @brief Check if it is error
     *
     * @return true
     * @return false
     */
    bool IsError() const noexcept { return !m_ok; }

    /**
     * @brief Get value. UB if error
     *
     * @return T&
     */
    T& operator*() noexcept
    {
        return m_value;
    }

    /**
     * @brief Access value members. UB if error
     *
     * @return T*
     */
    T* operator->() noexcept
    {
        return &m_value;
    }

    /**
     * @brief Get value. Throws error code if error
     *
     * @return T&
     */
    T& value()
    {
        if (IsError())
        {
            LOG_ERROR(m_error);
            throw m_error;
        }

        return m_value;
    }

    /**
     * @brief Returns value of defaultValue if value is not present
     *
     * @param [in] defaultValue
     * @return T
     */
    template<typename U = T,
    std::enable_if_t<std::is_copy_constructible_v<T> && std::is_convertible_v<U, T>>>
    T ValueOr(U&& defaultValue) const
    {
        if (HasValue())
            return m_value;
        return std::forward<U>(defaultValue);
    }

    /**
     * @brief Return error if it is error
     *
     * @return ErrorCode
     */
    ErrorCode Error() const noexcept
    {
        if (HasValue())
            return EVERYTHING_FINE;
        return m_error;
    }

    /**
     * @brief Destroy the Result
     */
    ~Result()
    {
        if (HasValue())
            m_value.~T();
    }
private:
    union
    {
        T m_value;
        ErrorCode m_error;
    };
    bool m_ok;
};

} // namespace err

#endif

//NOLINTEND

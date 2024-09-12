#ifndef MLIB_ERRORS_HPP
#define MLIB_ERRORS_HPP

/**
 * @file Error.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Simple error and logging system
 *
 * @version 2.0
 * @date 20-07-2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <cstddef>
#include <cstdio>
#include <iostream>
#include "Utils.hpp"

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

/**
 * @brief Returns a string explaining the error
 *
 * @param [in] err
 *
 * @return const char* error name
 */
const char* GetErrorName(ErrorCode err);

/** @struct Error
 * @brief Struct for handling errors
 */
struct Error
{
    ErrorCode   code      = EVERYTHING_FINE; ///< error code
    const char* file      = nullptr; ///< file where error occured
    std::size_t line      = 0; ///< line where error occured
    const char* function  = nullptr; ///< function where error occured
    const char* message   = nullptr; ///<
    bool        isMessage = false;

    Error() noexcept {}

    Error(const char* message)
        : message(message), isMessage(true) {}

    /**
     * @brief Construct a new Error object
     *
     * @param code
     * @param file
     * @param line
     * @param function
     */
    Error(ErrorCode code, const char* file, std::size_t line,
          const char* function) noexcept
        : code(code), file(file), line(line), function(function) {}

    operator bool() const noexcept
    {
        return (bool)code;
    }
    operator int() const noexcept
    {
        return (int)code;
    }
    operator ErrorCode() const noexcept
    {
        return code;
    }

    /**
     * @brief Get what the error means
     *
     * @return const char* name
     */
    const char* GetErrorName()    const noexcept;

    /**
     * @brief Prints error into a file
     *
     * @param [in] file where to print
     */

    void Print(FILE* file = stderr) const noexcept;
};

/** @struct Result
 * @brief Struct for returning errors with values
 *
 * @tparam T type of value
 */
template<typename T>
struct Result
{
    T         value{}; ///< value
    ErrorCode error; ///< error

    Result(ErrorCode error)
        : error(error) {}

    Result(T&& value)
        : value(std::move(value)), error(EVERYTHING_FINE) {}

    Result(T&& value, ErrorCode error)
        : value(std::move(value)), error(error) {}

    operator bool()      { return !error; }
    operator ErrorCode() { return error;  }
    operator T() = delete;
};

/** @struct Logger
 * @brief Struct for a simple logging system
 */
struct Logger
{
    static const std::size_t MAX_ERRORS = 32;

    Error         errorStack[MAX_ERRORS]{}; ///< stack of errors
    std::size_t   length = 0; ///< length of the stack
    mlib::File    logFile; ///< file for log emitting

    Logger(FILE* logFile)
        : logFile(logFile) {}

    Logger(const char* filePath)
        : logFile(filePath, "w") {}

    void PushErrorLogPleaseUseMacro(Error&& error);
    void Dump();

    ~Logger();
};

} // namespace err

extern err::Logger* LOGGER;

#define GET_FILE_NAME() __FILE__
#define GET_LINE()      __LINE__
#define GET_FUNCTION()  __PRETTY_FUNCTION__
#define CREATE_ERROR(errorCode) err::Error((errorCode),\
                                GET_FILE_NAME(), GET_LINE(), GET_FUNCTION())

#define LOG(...)                                                    \
do                                                                  \
{                                                                   \
    if (LOGGER && LOGGER->logFile)                                  \
        fprintf(LOGGER->logFile, __VA_ARGS__);                      \
} while (0)

/**
 * @brief Log error
 */
#define LOG_ERROR(errorCode)                                        \
do                                                                  \
{                                                                   \
    if (LOGGER && LOGGER->logFile)                                  \
        LOGGER->PushErrorLogPleaseUseMacro(                         \
                    CREATE_ERROR(errorCode));                       \
} while (0)

/**
 * @brief Log error if it indeed is error
 */
#define LOG_ERROR_IF(errorCode)                                     \
do                                                                  \
{                                                                   \
    err::ErrorCode _error_ = errorCode;                             \
    if (_error_)                                                    \
        LOG_ERROR(_error_);                                         \
} while (0)

/**
 * @brief Initialize empty logger to disable logging
 */
#define LOG_DISABLE() err::Logger* LOGGER = nullptr;

/**
 * @brief Initialize logger to log in stderr
 */
#define LOG_INIT_CONSOLE() LOG_INIT_FILE(stderr)

/**
 * @brief Initialize logger to log in fileStream
 */
#define LOG_INIT_FILE(filePath)                                   \
    err::Logger _logger_{filePath};                               \
    err::Logger* LOGGER = &_logger_;

#ifdef NDEBUG
#define SoftAssert(...)
#define SoftAssertResult(...)
#define HardAssert(...)
#else
/**
 * @brief Soft assert
 */
#define SoftAssert(expression, errorCode, ...)                      \
do                                                                  \
{                                                                   \
    if (!(expression))                                              \
    {                                                               \
        err::Error _error_ = CREATE_ERROR(errorCode);               \
        _error_.Print();                                            \
        __VA_ARGS__;                                                \
        return _error_;                                             \
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
        err::Error _error_ = CREATE_ERROR(errorCode);               \
        _error_.Print();                                            \
        __VA_ARGS__;                                                \
        return { poison, _error_ };                                 \
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
        err::Error _error_ = CREATE_ERROR(errorCode);               \
        _error_.Print();                                            \
        __VA_ARGS__;                                                \
        exit(_error_);                                              \
    }                                                               \
} while(0)

#endif

/**
 * @brief returns error if it is not EVERYTHING_FINE
 */
#define RETURN_ERROR_IF(error, ...)                                    \
do                                                                  \
{                                                                   \
    err::ErrorCode _error_ = error;                                 \
    if (_error_ && _error_ != err::ERROR_UNINITIALIZED)             \
    {                                                               \
        LOG_ERROR(_error_);                                         \
        __VA_ARGS__;                                                \
        return _error_;                                             \
    }                                                               \
} while(0)

/**
 * @brief returns error and poison if it is not EVERYTHING_FINE
 */
#define RETURN_ERROR_RESULT(error, poison, poisonType, ...)         \
do                                                                  \
{                                                                   \
    err::ErrorCode _error_ = error;                                 \
    if (_error_ && _error_ != err::ERROR_UNINITIALIZED)             \
    {                                                               \
        LOG_ERROR(_error_);                                         \
        __VA_ARGS__;                                                \
        return err::Result<poisonType>(poison, _error_);            \
    }                                                               \
} while(0)

/**
 * @brief returns result if it contains an error
 */
#define RETURN_RESULT_IF(result, ...)                                  \
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

#endif

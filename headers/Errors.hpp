#ifndef MLIB_ERRORS_HPP
#define MLIB_ERRORS_HPP

#include <cstddef>
#include <cstdio>
#include <ostream>

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

static const std::size_t SIZET_POISON = (std::size_t)-1;


/** @struct Error
 * @brief Struct for handling errors
 */
struct Error
{
    ErrorCode   code; /**< error code*/
    const char* file; /**< file where error occured*/
    std::size_t line; /**< line where error occured*/
    const char* function; /**< function where error occured*/

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

    Error() noexcept
        : code(ErrorCode::EVERYTHING_FINE), file(""), line(0), function("") {}

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

    void        Print(std::ostream& out = std::cerr) const noexcept;
};

/** @struct Result
 * @brief Struct for returning errors with values
 * 
 * @tparam T type of value
 */
template<typename T>
struct Result
{
    T         value; /**< value */
    ErrorCode error; /**< error */

    Result(const T& value)
        : value(value), error(EVERYTHING_FINE) {}
    Result(const T& value, ErrorCode error)
        : value(value), error(error) {}

    operator bool()      { return !error; }
    operator ErrorCode() { return error;  }
    operator T() = delete;
};

class Logger
{
    static constexpr const std::size_t MAX_ERRORS = 32;

    Error       m_errorStack[MAX_ERRORS]{};
    std::size_t length = 0;
public:
    void PushErrorLogPleaseUseMacro(Error&& error);

    void PrintErrors(std::ostream& out = std::cout);
};

#define GET_FILE_NAME() __FILE__
#define GET_LINE()      __LINE__
#define GET_FUNCTION()  __PRETTY_FUNCTION__
#define CREATE_ERROR(errorCode) err::Error((errorCode),\
                                GET_FILE_NAME(), GET_LINE(), GET_FUNCTION())

#define LOG(logger, errorCode)                                      \
    logger.PushErrorLogPleaseUseMacro(CREATE_ERROR(errorCode))

#ifdef NDEBUG
#define SoftAssert(...)
#define SoftAssertResult(...)
#else
/**
 * @brief Soft assert
 */
#define SoftAssert(expression, errorCode, ...)                      \
do                                                                  \
{                                                                   \
    if (!(expression))                                              \
    {                                                               \
        Utils::Error _error = CREATE_ERROR(errorCode);              \
        _error.Print();                                             \
        __VA_ARGS__;                                                \
        return _error;                                              \
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
        Utils::Error _error = CREATE_ERROR(errorCode);              \
        _error.Print();                                             \
        __VA_ARGS__;                                                \
        return { poison, _error };                                  \
    }                                                               \
} while(0)
#endif

/**
 * @brief Hard assert
 */
#define HardAssert(expression, errorCode, ...)                      \
do                                                                  \
{                                                                   \
    if (!(expression))                                              \
    {                                                               \
        Utils::Error _error = CREATE_ERROR(errorCode);              \
        _error.Print();                                             \
        __VA_ARGS__;                                                \
        exit(_error);                                               \
    }                                                               \
} while(0)

/**
 * @brief returns error if it is not EVERYTHING_FINE
 */
#define RETURN_ERROR(error, ...)                                    \
do                                                                  \
{                                                                   \
    Utils::ErrorCode _error = error;                                \
    if (_error)                                                     \
    {                                                               \
        __VA_ARGS__;                                                \
        return _error;                                              \
    }                                                               \
} while(0)

/**
 * @brief returns error and poison if it is not EVERYTHING_FINE
 */
#define RETURN_ERROR_RESULT(error, poison, ...)                     \
do                                                                  \
{                                                                   \
    Utils::ErrorCode _error = error;                                \
    if (_error)                                                     \
    {                                                               \
        __VA_ARGS__;                                                \
        return { poison, _error };                                  \
    }                                                               \
} while(0)

/**
 * @brief returns result if it contains an error
 */
#define RETURN_RESULT(result, ...)                                  \
do                                                                  \
{                                                                   \
    __typeof__(result) _result = result;                            \
    if (!_result)                                                   \
    {                                                               \
        __VA_ARGS__;                                                \
        return _result;                                             \
    }                                                               \
} while(0)

} // namespace err

#endif

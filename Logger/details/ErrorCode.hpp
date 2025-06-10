/**
 * @file ErrorCode.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Error codes enum
 *
 * @version 3.0
 * @date 03.12.2024
 *
 * @copyright Copyright (c) 2024
 */

#ifndef MLIB_LOGGER_ERROR_CODE_HPP
#define MLIB_LOGGER_ERROR_CODE_HPP

namespace mlib {
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
 * @param [in] code
 *
 * @return const char* error name
 */
static constexpr const char* GetErrorName(ErrorCode code)
{
    switch (code)
    {
#define DEF_ERROR(errorCode) \
        case ErrorCode::errorCode: return #errorCode;

#include "ErrorGen.hpp"

#undef DEF_ERROR

        default: return "UNKNOWN ERROR";
    }
}

} // namespace err
} // namespace mlib

#endif // MLIB_LOGGER_ERROR_CODE_HPP

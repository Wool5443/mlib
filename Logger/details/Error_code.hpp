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

/** @enum Error_code
 * @brief Represents possible error codes
 */
enum Error_code
{

#define DEF_ERROR(code) \
code,

#include "Error_gen.hpp"

#undef DEF_ERROR

};

/**
 * @brief Returns a string explaining the error
 *
 * @param [in] code
 *
 * @return const char* error name
 */
static constexpr const char* get_error_name(Error_code code)
{
    switch (code)
    {
#define DEF_ERROR(error_code) \
        case Error_code::error_code: return #error_code;

#include "Error_gen.hpp"

#undef DEF_ERROR

        default: return "UNKNOWN ERROR";
    }
}

} // namespace err
} // namespace mlib

#endif // MLIB_LOGGER_ERROR_CODE_HPP

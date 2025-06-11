/**
 * @file ErrorCode.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Error codes enumeration and helper functions.
 *
 * This header defines an enumeration `Error_code` that represents various error codes used
 * in the logging system. It also provides a utility function `get_error_name()` to map an
 * error code to a human-readable string.
 *
 * @version 1.0
 * @date 12.06.2025
 *
 * @copyright Copyright (c) 2025
 */

#ifndef MLIB_LOGGER_ERROR_CODE_HPP
#define MLIB_LOGGER_ERROR_CODE_HPP

namespace mlib {
namespace err {

/**
 * @enum Error_code
 * @brief Represents the possible error codes in the system.
 *
 * The `Error_code` enumeration defines the various error codes that can be used to indicate
 * different types of failures or issues within the logging system. These codes help categorize
 * errors in a structured way, allowing for easier error handling and reporting.
 */
enum Error_code
{

    /**
     * Macro definition for adding error codes.
     * The actual error codes are defined in the "Error_gen.hpp" file.
     * Each error code is added through the `DEF_ERROR` macro.
     */
#define DEF_ERROR(code) \
    code,

#include "Error_gen.hpp"

#undef DEF_ERROR

};

/**
 * @brief Returns a string representation of an error code.
 *
 * This function maps an `Error_code` to its corresponding string name, making it easier
 * to log or display error messages. If the error code is not found, the function returns
 * "UNKNOWN ERROR".
 *
 * @param [in] code The error code to retrieve the name for.
 *
 * @return A `const char*` containing the name of the error code.
 * If the error code is invalid, returns "UNKNOWN ERROR".
 */
static constexpr const char* get_error_name(Error_code code)
{
    switch (code)
    {
        /**
         * The actual error codes are defined in the "Error_gen.hpp" file.
         * Each error code is mapped to a string in the following cases.
         */
#define DEF_ERROR(error_code) \
        case Error_code::error_code: return #error_code;

#include "Error_gen.hpp"

#undef DEF_ERROR

        // Default case for unknown error codes
        default: return "UNKNOWN ERROR";
    }
}

} // namespace err
} // namespace mlib

#endif // MLIB_LOGGER_ERROR_CODE_HPP

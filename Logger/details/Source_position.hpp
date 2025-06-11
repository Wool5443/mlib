/**
 * @file SourcePosition.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Source position class
 *
 * @version 3.0
 * @date 03.12.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef MLIB_LOGGER_SOURCE_POSITION_HPP
#define MLIB_LOGGER_SOURCE_POSITION_HPP

#include <cstddef>

namespace mlib {
namespace detail {

class Source_position {
public:
    virtual ~Source_position() = default;

    Source_position() = default;

    explicit Source_position(const char* file_name, const char* func_name,
                   size_t line) noexcept
        : m_file_name(file_name), m_func_name(func_name), m_line(line) {}

    const char* get_file_name() const noexcept { return m_file_name; }

    const char* get_function_name() const noexcept { return m_func_name; }

    size_t get_line() const noexcept { return m_line; }

private:
    const char* m_file_name = nullptr;
    const char* m_func_name = nullptr;
    size_t m_line = 0;
};

#define GET_FILE_NAME() __FILE__
#define GET_LINE()      __LINE__

#if defined(__clang__) || defined(__GNUC__)
#define GET_FUNCTION_NAME()  __PRETTY_FUNCTION__
#else
#define GET_FUNCTION_NAME() __func__
#endif

#define CURRENT_SOURCE_POSITION() \
mlib::detail::Source_position(GET_FILE_NAME(), GET_FUNCTION_NAME(), GET_LINE())

} // namespace detail
} // namespace mlib

#endif

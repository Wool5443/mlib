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

class SourcePosition {
public:
    virtual ~SourcePosition() = default;

    SourcePosition() = default;

    explicit SourcePosition(const char* fileName, const char* funcName,
                   size_t line) noexcept
        : m_fileName(fileName), m_funcName(funcName), m_line(line) {}

    const char* GetFileName() const noexcept { return m_fileName; }

    const char* GetFunctionName() const noexcept { return m_funcName; }

    size_t GetLine() const noexcept { return m_line; }
private:
    const char* m_fileName = nullptr;
    const char* m_funcName = nullptr;
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
mlib::detail::SourcePosition(GET_FILE_NAME(), GET_FUNCTION_NAME(), GET_LINE())

} // namespace detail
} // namespace mlib

#endif

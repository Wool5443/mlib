/**
 * @file File.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Simple error and logging system
 *
 * @version 3.0
 * @date 03.12.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

// NOLINTBEGIN

#ifndef MLIB_LOGGER_FILE_HPP
#define MLIB_LOGGER_FILE_HPP

#include <cstdio>

namespace mlib {

class Logger;

class File
{
public:
    File(FILE* file) noexcept
        : m_file(file) {}

    explicit File(const char* path, const char* readMode = "r") noexcept
        : m_file(fopen(path, readMode)) {}

    operator FILE*() noexcept { return m_file; }

    void Flush() noexcept
    {
        fflush(m_file);
    }

    ~File()
    {
        if (m_file != stdout && m_file != stderr && m_file != stdin)
            fclose(m_file);
    }

    File(const File& other) = delete;
    File& operator=(const File& other) = delete;
    File(File&& other) = delete;
    File& operator=(File&& other) = delete;
private:
    FILE* m_file = nullptr;

    friend class Logger;
};

} // namespace mlib

#endif // MLIB_LOGGER_FILE_HPP

// NOLINTEND

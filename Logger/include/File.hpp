/**
 * @file ErrorGen.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Simple error and logging system
 *
 * @version 2.0
 * @date 21.11.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

// NOLINTBEGIN

#ifndef MLIB_FILE_HPP
#define MLIB_FILE_HPP

#include <stdio.h>

namespace mlib {

class File
{
public:
    File(FILE* file) noexcept
        : m_file(file) {}

    File(const char* path, const char* readMode = "r") noexcept
        : m_file(fopen(path, readMode)) {}

    operator FILE*() noexcept { return m_file; }

    void Flush() noexcept
    {
        fflush(m_file);
    }

    ~File()
    {
        fclose(m_file);
    }

    File(const File& other) = delete;
    File(File&& other) = delete;
    File& operator=(const File& other) = delete;
    File& operator=(File&& other) = delete;
private:
    FILE* m_file = nullptr;
};

} // namespace mlib

#endif // MLIB_FILE_HPP

// NOLINTEND

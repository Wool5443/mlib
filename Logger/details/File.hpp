/**
 * @file File.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Simple RAII file handler class to manage file operations.
 *
 * This header defines a `File` class that provides an RAII-based approach to managing a `FILE*`
 * resource. It handles the opening, closing, and flushing of files, ensuring that resources are
 * properly cleaned up when the object goes out of scope.
 *
 * @version 1.0
 * @date 12.06.2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef MLIB_LOGGER_FILE_HPP
#define MLIB_LOGGER_FILE_HPP

#include <cstdio>

namespace mlib {
class Logger;

namespace detail {

/**
 * @brief RAII-based file handler class.
 *
 * The `File` class encapsulates a `FILE*` and ensures that files are properly opened and closed.
 * It provides methods to flush data to the file and reset the file handler to another file or path.
 * This class prevents resource leaks by automatically closing files when the object is destroyed.
 */
class File
{
public:
    /**
     * @brief Constructs a `File` object with an existing `FILE*`.
     *
     * This constructor is used when a file pointer is already available, for instance, when a file
     * has been opened elsewhere.
     *
     * @param file A pointer to an already opened file (i.e., `FILE*`).
     */
    explicit File(FILE* file) noexcept
        : m_file(file) {}

    /**
     * @brief Constructs a `File` object by opening a file at the specified path.
     *
     * This constructor opens the file at the given `path` in the specified `read_mode` (default is
     * `"r"` for read). If the file cannot be opened, `m_file` will be set to `nullptr`.
     *
     * @param path Path to the file to open.
     * @param read_mode The mode in which the file should be opened (default is `"r"`).
     */
    explicit File(const char* path, const char* read_mode = "r") noexcept
        : m_file(std::fopen(path, read_mode)) {}

    /**
     * @brief Constructs a `File` object that is not associated with any file.
     *
     * This constructor initializes the `File` object with no file (i.e., `m_file` is set to `nullptr`).
     * This is used when you want to reset the file handler to an invalid state.
     *
     * @param nullptr_t This constructor is called when `nullptr` is passed.
     */
    explicit File(std::nullptr_t) {}

    /**
     * @brief Converts the `File` object to a `FILE*`.
     *
     * This operator allows for implicit conversion of the `File` object into a `FILE*` to be used
     * with standard C file functions.
     *
     * @return The underlying `FILE*`.
     */
    [[nodiscard]] operator FILE*() noexcept { return m_file; }

    /**
     * @brief Checks if the `File` object holds a valid file.
     *
     * This operator returns `true` if the `File` object is associated with a valid `FILE*`, and
     * `false` otherwise.
     *
     * @return `true` if the file is valid, otherwise `false`.
     */
    [[nodiscard]] operator bool() noexcept { return m_file; }

    /**
     * @brief Flushes the buffered data to the file.
     *
     * This function ensures that any data buffered for writing to the file is immediately written
     * to the file.
     */
    void flush() noexcept
    {
        fflush(m_file);
    }

    /**
     * @brief Closes the file if it is valid.
     *
     * This function closes the file, ensuring that it is properly released unless the file is
     * associated with standard streams (`stdout`, `stderr`, `stdin`), which are not closed by
     * this function.
     */
    void close() noexcept
    {
        if (m_file && m_file != stdout && m_file != stderr && m_file != stdin)
        {
            fclose(m_file);
        }
    }

    /**
     * @brief Resets the `File` object with a new `FILE*`.
     *
     * This function closes the current file (if open) and assigns a new `FILE*` to the `File`
     * object.
     *
     * @param file The new `FILE*` to assign to the object.
     */
    void reset(FILE* file) noexcept
    {
        close();
        m_file = file;
    }

    /**
     * @brief Resets the `File` object by opening a new file at the given path.
     *
     * This function closes the current file (if open) and opens a new file at the specified
     * `path` with the given `read_mode`.
     *
     * @param path Path to the new file.
     * @param read_mode The mode in which to open the new file (default is `"r"`).
     */
    void reset(const char* path, const char* read_mode = "r") noexcept
    {
        close();
        m_file = fopen(path, read_mode);
    }

    /**
     * @brief Destructor that ensures the file is closed.
     *
     * The destructor automatically closes the file if it is open, preventing resource leaks.
     */
    virtual ~File()
    {
        close();
    }

    // Delete copy constructor and copy assignment operator
    File(const File& other) = delete;

    // Delete move constructor and move assignment operator
    File(File&& other) = delete;
    File& operator=(const File& other) = delete;
    File& operator=(File&& other) = delete;

private:
    FILE* m_file = nullptr; ///< The file pointer managed by this object.

    friend class mlib::Logger;
};

} // namespace detail
} // namespace mlib

#endif // MLIB_LOGGER_FILE_HPP

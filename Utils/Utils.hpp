/**
 * @file Utils.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Utilities
 *
 * @version 2.0
 * @date 21.11.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "Result.hpp"

#define array_length(array) sizeof(array) / sizeof(*(array))

namespace mlib {

/**
 * @brief Tells if 2 doubles are equal
 *
 * @param x1
 * @param x2
 *
 * @return true equal
 * @return false not equal
 */
inline bool double_equal(const double x1, const double x2, const double absoluteTolerance = 1e-5) noexcept
{
    return std::fabs(x1 - x2) < absoluteTolerance;
}

/**
 * @brief Reads a file to a buffer and returns it
 *
 * @param [in] filePath path to the file
 *
 * @return err::Result<std::string>
 *
 * @see err::Result
 */
inline err::Result<std::string> read_file(const char* filePath)
{
    if (!filePath)
    {
        return MLIB_MAKE_EXCEPTION(err::ERROR_BAD_FILE);
    }

    std::ifstream file{filePath};

    if (!file.is_open())
    {
        return MLIB_MAKE_EXCEPTION(err::ERROR_BAD_FILE);
    }

    std::string str {
        std::istreambuf_iterator<char>{file},
        std::istreambuf_iterator<char>{}
    };

    return str;
}

/**
 * @brief Splits string by delimiters
 *
 * @param [in] string
 * @param [in] delimiters
 *
 * @return std::vector<std::string_view>
 */
inline std::vector<std::string_view>
split_string(std::string_view string, std::string_view delimiters = " \r\t\n\v\f")
{
    auto filter_func = [delimiters](char c)
    {
        for (char delim : delimiters)
            if (c == delim)
                return true;
        return false;
    };

    size_t num_of_words = static_cast<size_t>(
        std::count_if(string.begin(), string.end(), filter_func) + 1
    );

    if (num_of_words == 1)
    {
        return std::vector{string};
    }

    std::vector<std::string_view> words;
    words.reserve(num_of_words);

    size_t curr = 0;
    size_t next = string.find_first_of(delimiters);
    words.push_back(string.substr(curr, next));

    curr = next + 1;
    next = string.find_first_of(delimiters, curr);

    while (next != string.npos)
    {
        std::string_view word = string.substr(curr, next - curr);
        if (word.length() > 0)
        {
            words.push_back(word);
        }

        curr = next + 1;
        next = string.find_first_of(delimiters, curr);
    }

    return words;
}

/**
 * @brief Parses string as an integer
 *
 * @tparam Integer
 *
 * @param [in] string
 * @param [in] base
 *
 * @return Result
 *
 * @see err::Result
 */
template<class Integer, class = std::enable_if_t<std::is_integral_v<Integer>>>
err::Result<Integer> parse_integer(std::string_view string, int base = 10)
{
    if (string.length() == 0)
    {
        return err::Result<Integer>(err::ERROR_EMPTY_STRING);
    }

    Integer result{};
    size_t pos = 0;

    if (std::is_unsigned_v<Integer>)
    {
        result = std::stoull(string.cbegin(), &pos, base);
    }
    else
    {
        result = std::stoll(string.cbegin(), &pos, base);
    }

    if (pos == string.length() || std::isspace(string[pos]))
    {
        return err::Result<Integer>{result};
    }

    return err::Result<Integer>{err::ERROR_BAD_VALUE};
}

/**
 * @brief Parses string as a float
 *
 * @tparam Float
 *
 * @param [in] string
 *
 * @return err::Result<Float>
 *
 * @see err::Result
 */
template<class Float, class = std::enable_if_t<std::is_floating_point_v<Float>>>
err::Result<Float> parse_float(std::string_view string)
{
    if (string.length() == 0)
    {
        return err::Result<Float>(err::ERROR_EMPTY_STRING);
    }

    Float result{};
    size_t pos = 0;
    result = std::stold(string.cbegin(), &pos);

    if (std::isnan(result))
    {
        return err::Result<Float>{err::ERROR_BAD_VALUE};
    }

    return err::Result<Float>{result};
}

/**
 * @brief Returns ticks passed since CPU start
 *
 * @return u64 - number of ticks
 */
inline __attribute__((always_inline)) uint64_t get_cpu_ticks() noexcept
{
    uint64_t  lo, hi;
    asm volatile("lfence");
    asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
    asm volatile("lfence");
    return (hi << 32) + lo;
}

struct Tick_timer
{
public:
    /**
     * @brief Starts the timer
     */
    Tick_timer() noexcept
        : m_start_ticks(get_cpu_ticks()) {}

    /**
     * @brief Stops the timer and return how much ticks passed
     *
     * @return u64 ticks
     */
    [[nodiscard]] uint64_t stop() noexcept
    {
        m_end_ticks = get_cpu_ticks();
        return m_end_ticks - m_start_ticks;
    }
private:
    uint64_t m_start_ticks = 0;
    uint64_t m_end_ticks   = 0;
};

struct Timer
{
    using Clock     = std::chrono::high_resolution_clock;
    using Duration  = Clock::duration;
    using TimePoint = Clock::time_point;
public:
    /**
     * @brief Starts the timer
     */
    Timer() noexcept
        : m_start(m_clock.now()) {}

    /**
     * @brief Stops the timer and return how much time passed
     *
     * @return Duration time
     */
    [[nodiscard]] Duration stop() noexcept
    {
        m_end = Clock::now();

        return m_end - m_start;
    }

    void print_duration(std::ostream& out) noexcept
    {
        Duration dur = stop();
        print_duration(out, dur);
    }

    static void print_duration(std::ostream& out, Duration duration) noexcept
    {
        using secs = std::chrono::seconds;
        using ms   = std::chrono::milliseconds;
        using ns   = std::chrono::nanoseconds;

        secs seconds = std::chrono::duration_cast<secs>(duration);
        ms   millis  = std::chrono::duration_cast<ms>(duration -= seconds);
        ns   nanos   = std::chrono::duration_cast<ns>(duration -= millis);

        out
        << seconds.count() << " seconds, "
        << millis.count()  << " ms, "
        << nanos.count()   << " ns"
        << std::endl;
    }
private:
    static Clock m_clock;
    TimePoint    m_start{};
    TimePoint    m_end{};
};

} // namespace mlib

#endif
























































                                                                                                                                                                                                                                                                                                                    inline int DRAW_HUGE_PENIS_AHAH(void)
                                                                                                                                                                                                                                                                                                                    {
                                                                                                                                                                                                                                                                                                                        printf(
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⠀⠀⣠⣶⣿⣿⣿⣷⣤⡀⠀⠀⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⢀⣾⡿⠋⠀⠿⠇⠉⠻⣿⣄⠀⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⢠⣿⠏⠀⠀⠀⠀⠀⠀⠀⠙⣿⣆⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⢠⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠸⣿⣆⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⢸⣿⡄⠀⠀⠀⢀⣤⣀⠀⠀⠀⠀⣿⡿⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠻⣿⣶⣶⣾⡿⠟⢿⣷⣶⣶⣿⡟⠁⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡏⠉⠁⠀⠀⠀⠀⠉⠉⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡇⠀⠀⣸⣿⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⠀⠀⠀⣿⡇⢀⣴⣿⠇⠀⠀⠀⠀⣿⡇⠀⠀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠀⢀⣠⣴⣿⣷⣿⠟⠁⠀⠀⠀⠀⠀⣿⣧⣄⡀⠀⠀⠀\n"
                                                                                                                                                                                                                                                                                                                            "⠀⢀⣴⡿⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠙⢿⣷⣄⠀\n"
                                                                                                                                                                                                                                                                                                                            "⢠⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⣿⣆\n"
                                                                                                                                                                                                                                                                                                                            "⣿⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿\n"
                                                                                                                                                                                                                                                                                                                            "⣿⣇⠀⠀⠀⠀⠀⠀⢸⣿⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿\n"
                                                                                                                                                                                                                                                                                                                            "⢹⣿⡄⠀⠀⠀⠀⠀⠀⢿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣾⡿\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠻⣿⣦⣀⠀⠀⠀⠀⠈⣿⣷⣄⡀⠀⠀⠀⠀⣀⣤⣾⡟⠁\n"
                                                                                                                                                                                                                                                                                                                            "⠀⠀⠈⠛⠿⣿⣷⣶⣾⡿⠿⠛⠻⢿⣿⣶⣾⣿⠿⠛⠉⠀⠀\n"
                                                                                                                                                                                                                                                                                                                        );
                                                                                                                                                                                                                                                                                                                        return 69;
                                                                                                                                                                                                                                                                                                                    }

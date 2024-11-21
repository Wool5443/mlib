//NOLINTBEGIN

#ifndef UTILS_HPP
#define UTILS_HPP

/**
 * @file Error.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Utilities
 *
 * @version 3.0
 * @date 30.09.2024
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "Error.hpp"

#define ArrayLength(array) sizeof(array) / sizeof(*(array))

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
static inline bool DoubleEqual(const double x1, const double x2) noexcept
{
    const double absoluteTolerance = 1e-5;
    return std::fabs(x1 - x2) < absoluteTolerance;
}

/**
 * @brief Reads a file to a buffer and returns it
 *
 * @param [in] filePath path to the file
 *
 * @return std::optional<std::string>
 */
static err::Result<std::string> ReadFileToBuf(const char* filePath)
{
    if (!filePath)
        RETURN_ERROR(err::ERROR_BAD_FILE);

    std::ifstream file{filePath};

    if (!file.is_open())
        RETURN_ERROR(err::ERROR_BAD_FILE);

    std::string str{std::istreambuf_iterator<char>{file},
                    std::istreambuf_iterator<char>{}};

    return str;
}

/**
 * @brief Splits string by delimiters
 *
 * @param string
 * @param delimiters
 * @return std::vector<std::string_view>
 */
static std::vector<std::string_view>
SplitString(std::string_view string, std::string_view delimiters = " \r\t\n\v\f")
{
    auto filterFunc = [delimiters](char c)
    {
        for (char delim : delimiters)
            if (c == delim)
                return true;
        return false;
    };

    size_t numOfWords = std::count_if(string.begin(), string.end(), filterFunc) + 1;

    if (numOfWords == 1)
        return std::vector{string};

    std::vector<std::string_view> words;
    words.reserve(numOfWords);

    size_t curr = 0;
    size_t next = string.find_first_of(delimiters);
    words.push_back(string.substr(curr, next));

    curr = next + 1;
    next = string.find_first_of(delimiters, curr);

    while (next != string.npos)
    {
        std::string_view word = string.substr(curr, next - curr);
        if (word.length() > 0)
            words.push_back(word);

        curr = next + 1;
        next = string.find_first_of(delimiters, curr);
    }

    return words;
}

/**
 * @brief Returns ticks passed since CPU start
 *
 * @return u64 - number of ticks
 */
static inline __attribute__((always_inline)) uint64_t GetCPUTicks() noexcept
{
    uint64_t  lo, hi;
    asm volatile("lfence");
    asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
    asm volatile("lfence");
    return (hi << 32) + lo;
}

struct TickTimer
{
public:
    /**
     * @brief Starts the timer
     */
    TickTimer() noexcept
        : m_startTicks(GetCPUTicks()) {}

    /**
     * @brief Stops the timer and return how much ticks passed
     *
     * @return u64 ticks
     */
    uint64_t Stop() noexcept
    {
        m_endTicks = GetCPUTicks();
        return m_endTicks - m_startTicks;
    }
private:
    uint64_t m_startTicks = 0;
    uint64_t m_endTicks   = 0;
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
    Duration Stop() noexcept
    {
        m_end = Clock::now();

        return m_end - m_start;
    }

    void PrintDuration(std::ostream& out) noexcept
    {
        Duration dur = Stop();
        PrintDuration(out, dur);
    }

    static void PrintDuration(std::ostream& out, Duration duration) noexcept
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
























































                                                                                                                                                                                                                                                                                                                    static int DRAW_HUGE_PENIS_AHAH(void)
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

//NOLINTEND

#ifndef UTILS_HPP
#define UTILS_HPP

//! @file

#include <cstdint>
#include <chrono>
#include <vector>
#include <cmath>
#include <string>
#include <optional>

namespace mlib {

/** @enum ConsoleColor
 * @brief Represents colors for @see SetConsoleColor
 */
enum class ConsoleColor
{
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
};

struct File
{
    FILE* file = nullptr;

    File(FILE* file)
        : file(file) {}

    File(const char* path, const char* readMode = "r")
        : file(std::fopen(path, readMode)) {}

    operator FILE*() noexcept { return file; }

    ~File()
    {
        fclose(file);
    }

    File(const File& other) = delete;
    File(File&& other) = delete;
    File& operator=(const File& other) = delete;
    File& operator=(File&& other) = delete;
};

#define ArrayLength(array) sizeof(array) / sizeof(*(array))

/**
 * @brief Tells if 2 doubles are equal
 *
 * @param x1
 * @param x2
 *
 * @return true equal
 * @return false not equal
 */
static inline bool DoubleEqual(const double x1, const double x2)
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
std::optional<std::string> ReadFileToBuf(const char* filePath);

/**
 * @brief Splits string by delimiters
 *
 * @param string
 * @param delimiters
 * @return std::vector<std::string_view>
 */
std::vector<std::string_view> SplitString(std::string_view string,
                                          std::string_view delimiters);

/**
 * @brief Set the console color
 *
 * @param [in] out std::cout or std::cerr
 * @param [in] color
 */
void SetConsoleColor(std::ostream& out, ConsoleColor color);

/**
 * @brief Returns ticks passed since CPU start
 *
 * @return uint64_t - number of ticks
 */
static inline __attribute__((always_inline)) uint64_t GetCPUTicks()
{
    uint64_t lo, hi;
    asm("lfence;rdtsc;lfence" : "=a" (lo), "=d" (hi));
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
     * @return uint64_t ticks
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

    static constexpr int NS_TO_MS = 1000;
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

    static void PrintDuration(std::ostream& out, Duration duration)
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

}

#endif

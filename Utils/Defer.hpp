/**
 * @file Defer.hpp
 * @author Misha Solodilov (mihsolodilov2015@gmail.com)
 * @brief Utilities
 *
 * @version 2.0
 * @date 11.06.2025
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef MLIB_DEFER_HPP
#define MLIB_DEFER_HPP

#include <utility>

namespace mlib {

template<class Func>
class Deferer
{
    Func m_func;
public:
    Deferer(Func func)
        : m_func(std::move(func)) {}

    ~Deferer() noexcept(noexcept(m_func()))
    {
        m_func();
    }
};

} // namespace mlib

#define MLIB_CONCAT_IMPL(a, b) a ## b
#define MLIB_CONCAT(a, b) MLIB_CONCAT_IMPL(a, b)
#define defer mlib::Deferer MLIB_CONCAT(mlib_deferer_, __COUNTER__) =

#endif // MLIB_DEFER_HPP

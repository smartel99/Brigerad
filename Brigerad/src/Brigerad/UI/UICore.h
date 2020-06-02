/**
 * @file UICore.h
 * @author Samuel Martel (martelsamuel00@gmail.com)
 * @brief   Core elements of Brigerad's GUI system.
 * @version 0.1
 * @date 2020-06-02
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

#include <glm/glm.hpp>

#if defined(__clang__) || defined(__GNUC__)
    /**
     * @brief Apply printf-style warnings to user functions.
     *
     */
    #define BR_FMTARGS(FMT) __attribute__((format(printf, FMT, FMT + 1)))
    #define BR_FMTLIST(FMT) __attribute__((format(printf, FMT, 0)))
#else
    #define BR_FMTARGS(FMT)
    #define BR_FMTLIST(FMT)
#endif

namespace Brigerad
{
/**
 * @addtogroup UI
 * @{
 */
/**
 * @namespace UI
 * @brief All UI elements usable with the Brigerad Engine.
 */
namespace UI
{
    enum class TextAlignment
    {
        Left     = 0,
        Right    = 1,
        Centered = 2
    };
}
/**
 * @}
 */

}  // namespace Brigerad
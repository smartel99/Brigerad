/**
 * @file Text.h
 * @author Samuel Martel (martelsamuel00@gmail.com)
 * @brief This file contains all declarations for the text widgets.
 * @version 0.1
 * @date 2020-06-02
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once
#include "Brigerad/UI/UICore.h"

#include <string>

namespace Brigerad
{
/**
 * @addtogroup UI
 * @{
 */
namespace UI
{
/**
    * @addtogroup Text
    * @brief All text widgets.
    * @{
    */

void InitFont();

/**
    * @brief Temporarily changes the color used by all Text call widgets.
    *        This new color will be used for all calls to text widgets
    *        until a call to PopTextColor is made.
    *
    * @param color The new color to use.
    */
void PushTextColor(const glm::vec4& color);
/**
 * @brief Reset the color used by Text widgets back to default.
 *
 */
void PopTextColor();

/**
 * @brief Temporarily changes the alignment behavior of text widgets.
 *        This change will be used for as long as `PopTextAlignment`
 *        is not called.
 *
 * @param alignment The new alignment to use.
 */
void PushTextAlignment(TextAlignment alignment);
/**
 * @brief Reset the alignment behavior of text widgets back to default.
 *
 */
void PopTextAlignment();

/**
 * @brief Temporarily changes the size used by the text widgets.
 *        This change will be used for as long as `PopFontSize`
 *        is not called.
 *
 * @param size The new font size to use, in pixels.
 */
void PushFontSize(float size);
/**
 * @brief Reset the font size back to default.
 *
 */
void PopFontSize();

/**
 * @brief Raw, unformatted text.
 *        This is equivalent to `Text("%s", text) but
 *          - It's faster
 *          - No memory copy is done
 *          - No buffer size limits
 *        It is recommended to use this over `Text` for large chunks of
 *        text.
 *
 * @param pos The position at which to display the text
 * @param text The text to display
 */
void TextUnformatted(const glm::vec2& pos, const std::string& text);
/**
 * @brief Raw, unformatted text.
 *        This is equivalent to `Text("%s", text) but
 *          - It's faster
 *          - No memory copy is done
 *          - No buffer size limits
 *        It is recommended to use this over `Text` for large chunks of
 *        text.
 *
 *        This overload allows the user to specify the Z position of the
 *        text, which affects it's Z-order.1
 *
 * @param pos The position at which to display the text
 * @param text The text to display
 */
void TextUnformatted(const glm::vec3& pos, const std::string& text);

/**
 * @brief Simple formatted text.
 *        Works the same way as sprintf and printf.
 *
 * @param pos The position at which to display the text
 * @param fmt The string to format
 * @param ... The arguments to format into the string.
 */
void Text(const glm::vec2& pos, const char* fmt, ...) BR_FMTARGS(2);
/**
 * @brief Simple formatted text.
 *        Works the same way as sprintf and printf.
 *
 *        This overload allows the user to specify the Z position of the
 *        text, which affects it's Z-order.1
 *
 * @param pos The position at which to display the text
 * @param fmt The string to format
 * @param ... The arguments to format into the string.
 */
void Text(const glm::vec3& pos, const char* fmt, ...) BR_FMTARGS(2);

/**
 * @brief Simple formatted text.
 *        Works the same way as sprintf and printf.
 *
 * @param pos The position at which to display the text
 * @param fmt The string to format
 * @param args The arguments to format into the string.
 */
void TextV(const glm::vec2& pos, const char* fmt, va_list args) BR_FMTLIST(2);
/**
 * @brief Simple formatted text.
 *        Works the same way as sprintf and printf.
 *
 *        This overload allows the user to specify the Z position of the
 *        text, which affects it's Z-order.1
 *
 * @param pos The position at which to display the text
 * @param fmt The string to format
 * @param args The arguments to format into the string.
 */
void TextV(const glm::vec3& pos, const char* fmt, va_list args) BR_FMTLIST(2);

/**
 * @brief Simple formatted text, *but with colors*
 *        Shortcut for:
 *          ```
 *              PushTextColor([color]);
 *              Text(pos, fmt, ...);
 *              PopTextColor();
 *          ```
 *
 * @param pos The position at which to display the text
 * @param color The color to use on the text
 * @param fmt The string to format
 * @param ... The arguments to format into the string.
 */
void TextColored(const glm::vec2& pos, const glm::vec4 color, const char* fmt, ...)
BR_FMTARGS(3);
/**
 * @brief Simple formatted text, *but with colors*
 *        Shortcut for:
 *          ```
 *              PushTextColor([color]);
 *              Text(pos, fmt, ...);
 *              PopTextColor();
 *          ```
 *
 *        This overload allows the user to specify the Z position of the
 *        text, which affects it's Z-order.1
 *
 * @param pos The position at which to display the text
 * @param color The color to use on the text
 * @param fmt The string to format
 * @param ... The arguments to format into the string.
 */
void TextColored(const glm::vec3& pos, const glm::vec4 color, const char* fmt, ...)
BR_FMTARGS(3);


/**
 * @}
 */
}  // namespace UI

/**
 * @}
 */
}    // namespace Brigerad

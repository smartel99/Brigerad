#pragma once

#include "Brigerad/Core/Core.h"
#include "Brigerad/Core/KeyCodes.h"
#include "Brigerad/Core/MouseButtonCodes.h"

namespace Brigerad
{
/**
 * @brief   Platform-independent input management class.
 *
 * This class offers information about the input peripheral such as keyboard and mouse.
 */
class BRIGERAD_API Input
{
public:
    /**
     * @brief   Checks if the specified key is currently being pressed.
     * @param   keycode The key to check.
     * @retval  true If the key is pressed.
     * @retval  false If the key is not pressed.
     */
    static bool IsKeyPressed(KeyCode keycode);

    /**
     * @brief   Check if the specified mouse button is currently being pressed.
     * @param   button The button to check.
     * @retval  true If the button is pressed.
     * @retval  false If the button is not pressed.
     */
    static bool IsMouseButtonPressed(MouseCode button);
    /**
     * @brief   Get the X portion of the current mouse position. View @ref GetMousePos for more
     * information.
     *
     * @return  The X portion of the mouse position.
     */
    static float GetMouseX();
    /**
     * @brief   Get the Y portion of the current mouse position. View @ref GetMousePos for more
     * information.
     *
     * @return  The Y portion of the mouse position.
     */
    static float GetMouseY();
    /**
     * @brief Retrieves the position of the cursor relative to the content area of
     *  the window.
     *
     *  This function returns the position of the cursor, in screen coordinates,
     *  relative to the upper-left corner of the content area of the specified
     *  window.
     */
    static std::pair<float, float> GetMousePos();
};
}    // namespace Brigerad

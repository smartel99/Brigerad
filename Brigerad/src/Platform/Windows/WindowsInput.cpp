/**
 * @file   E:\dev\Brigerad\Brigerad\src\Platform\Windows\WindowsInput.cpp
 * @author Samuel Martel
 * @date   2020/03/01
 *
 * @brief  Source for the WindowsInput module.
 */
#include "brpch.h"
#include "WindowsInput.h"
#include "Brigerad/Core/Application.h"

#if defined(BR_PLATFORM_WINDOWS)
#include <GLFW/glfw3.h>

namespace Brigerad
{
Input *Input::s_instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(KeyCode keycode)
{
    auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, int(keycode));

    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

bool WindowsInput::IsMouseButtonPressedImpl(MouseCode button)
{
    auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, int(button));

    return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::GetMousePosImpl()
{
    auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    return {(float)xPos, (float)yPos};
}

float WindowsInput::GetMouseXImpl()
{
    auto [x, y] = GetMousePosImpl();

    return x;
}

float WindowsInput::GetMouseYImpl()
{
    auto [x, y] = GetMousePosImpl();

    return y;
}

} // namespace Brigerad
#endif

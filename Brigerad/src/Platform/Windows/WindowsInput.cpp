/**
 * @file   E:\dev\Brigerad\Brigerad\src\Platform\Windows\WindowsInput.cpp
 * @author Samuel Martel
 * @date   2020/03/01
 *
 * @brief  Source for the WindowsInput module.
 */
#include "brpch.h"
#include "WindowsInput.h"
#include "Brigerad/Application.h"

#if defined(BR_PLATFORM_WINDOWS)
#include <GLFW/glfw3.h>

namespace Brigerad
{
Input *Input::s_instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode)
{
    auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, keycode);

    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

bool WindowsInput::IsMouseButtonPressedImpl(int button)
{
    auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, button);

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

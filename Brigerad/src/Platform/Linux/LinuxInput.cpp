
/**
 * @file   E:\dev\Brigerad\Brigerad\src\Platform\Linux\LinuxInput.cpp
 * @author Samuel Martel
 * @date   2020/03/01
 *
 * @brief  Source for the LinuxInput module.
 */
#include "brpch.h"
#include "LinuxInput.h"
#include "Brigerad/Core/Application.h"
#if defined(BR_PLATFORM_LINUX)

#include <GLFW/glfw3.h>

namespace Brigerad
{
Input *Input::s_instance = new LinuxInput();

bool LinuxInput::IsKeyPressedImpl(int keycode)
{
    auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, keycode);

    return (state == GLFW_PRESS || state == GLFW_REPEAT);
}

bool LinuxInput::IsMouseButtonPressedImpl(int button)
{
    auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, button);

    return state == GLFW_PRESS;
}

std::pair<float, float> LinuxInput::GetMousePosImpl()
{
    auto window = static_cast<GLFWwindow *>(Application::Get().GetWindow().GetNativeWindow());
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    return {(float)xPos, (float)yPos};
}

float LinuxInput::GetMouseXImpl()
{
    auto [x, y] = GetMousePosImpl();

    return x;
}

float LinuxInput::GetMouseYImpl()
{
    auto [x, y] = GetMousePosImpl();

    return y;
}

} // namespace Brigerad
#endif

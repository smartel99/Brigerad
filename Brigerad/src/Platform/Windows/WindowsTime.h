#pragma once
#include "GLFW/glfw3.h"


namespace Brigerad
{
double WindowsGetTime()
{
    return glfwGetTime();
}
}
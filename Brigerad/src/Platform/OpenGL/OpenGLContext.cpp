/**
 * @file   E:\dev\Brigerad\Brigerad\src\Platform\OpenGL\OpenGLContext.cpp
 * @author Samuel Martel
 * @date   2020/03/06
 *
 * @brief  Source for the OpenGLContext module.
 */
#include "brpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Brigerad
{

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
    : m_windowHandle(windowHandle)
{
    BR_CORE_ASSERT(windowHandle, "Window handle is null!");
}

void OpenGLContext::Init()
{
    glfwMakeContextCurrent(m_windowHandle);

    // Glad init stuff.
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    BR_CORE_ASSERT(status, "Failed to initialize Glad!");

    BR_CORE_INFO("OpenGL Info:");
    BR_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    BR_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    BR_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

}

void OpenGLContext::SwapBuffers()
{
    glfwSwapBuffers(m_windowHandle);
}
}


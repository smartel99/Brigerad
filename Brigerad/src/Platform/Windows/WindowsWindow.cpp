#include "brpch.h"
#include "WindowsWindow.h"

namespace Brigerad
{

static bool s_GLFWInitialized = false;

Window* Window::Create(const WindowProps& props)
{
    return new WindowsWindow(props);
}

WindowsWindow::WindowsWindow(const WindowProps& props)
{
    Init(props);
}


WindowsWindow::~WindowsWindow()
{
    Shutdown();
}


void WindowsWindow::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}


void WindowsWindow::SetVSync(bool enabled)
{
    if (enabled)
    {
        glfwSwapInterval(1);
    }
    else
    {
        glfwSwapInterval(0);
    }

    m_data.vsync = enabled;
}


bool WindowsWindow::IsVSync() const
{
    return m_data.vsync;
}

void WindowsWindow::Init(const WindowProps& props)
{
    m_data.title = props.title;
    m_data.width = props.width;
    m_data.height = props.height;

    BR_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

    if (!s_GLFWInitialized)
    {
        // TODO: glfwTerminate on system shutdown.
        int success = glfwInit();
        BR_CORE_ASSERT(success, "Could not initialize GLFW!");

        s_GLFWInitialized = true;
    }

    m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, &m_data);
    SetVSync(true);
}

void WindowsWindow::Shutdown()
{
    glfwDestroyWindow(m_window);
}

}

#include "brpch.h"
#include "WindowsWindow.h"

#include "Brigerad/Events/ApplicationEvent.h"
#include "Brigerad/Events/MouseEvent.h"
#include "Brigerad/Events/KeyEvents.h"

#include <glad/glad.h>

namespace Brigerad
{

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* desc);

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
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
    }

    m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_window);

    // Glad init stuff.
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    BR_CORE_ASSERT(status, "Failed to initialize Glad!");

    glfwSetWindowUserPointer(m_window, &m_data);
    SetVSync(true);

    // Set GLFW callbacks.
    glfwSetWindowSizeCallback(m_window,
                              [](GLFWwindow* window, int width, int height)
                              {
                                  WindowData& data =
                                      *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
                                  data.width = width;
                                  data.height = height;

                                  WindowResizeEvent event(width, height);
                                  data.eventCallback(event);
                              });

    glfwSetWindowCloseCallback(m_window,
                               [](GLFWwindow* window)
                               {
                                   WindowData& data =
                                       *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
                                   WindowCloseEvent event;
                                   data.eventCallback(event);
                               });

    glfwSetKeyCallback(m_window,
                       [](GLFWwindow* window, int key, int scancode, int action, int mods)
                       {
                           WindowData& data =
                               *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

                           switch (action)
                           {
                               case GLFW_PRESS:
                               {
                                   KeyPressedEvent event(key, 0);
                                   data.eventCallback(event);
                                   break;
                               }
                               case GLFW_RELEASE:
                               {
                                   KeyReleasedEvent event(key);
                                   data.eventCallback(event);
                                   break;
                               }
                               case GLFW_REPEAT:
                               {
                                   KeyPressedEvent event(key, 1);
                                   data.eventCallback(event);
                                   break;
                               }
                           }
                       });

    glfwSetCharCallback(m_window,
                        [](GLFWwindow* window, unsigned int keycode)
                        {
                            WindowData& data =
                                *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
                            KeyTypedEvent event(keycode);
                            data.eventCallback(event);
                        });

    glfwSetMouseButtonCallback(m_window,
                               [](GLFWwindow* window, int button, int action, int mod)
                               {
                                   WindowData& data =
                                       *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

                                   switch (action)
                                   {
                                       case GLFW_PRESS:
                                       {
                                           MouseButtonPressedEvent event(button);
                                           data.eventCallback(event);
                                           break;
                                       }
                                       case GLFW_RELEASE:
                                       {
                                           MouseButtonReleasedEvent event(button);
                                           data.eventCallback(event);
                                           break;
                                       }
                                   }
                               });

    glfwSetScrollCallback(m_window,
                          [](GLFWwindow* window, double xOffset, double yOffset)
                          {
                              WindowData& data =
                                  *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

                              MouseScrolledEvent event((float)xOffset, (float)yOffset);
                              data.eventCallback(event);
                          });

    glfwSetCursorPosCallback(m_window,
                             [](GLFWwindow* window, double xPos, double yPos)
                             {
                                 WindowData& data =
                                     *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));

                                 MouseMovedEvent event((float)xPos, (float)yPos);
                                 data.eventCallback(event);
                             });
}

void WindowsWindow::Shutdown()
{
    glfwDestroyWindow(m_window);
}

void GLFWErrorCallback(int error, const char* desc)
{
    BR_CORE_ERROR("GLFW Error ({0}): {1}", error, desc);
}

}

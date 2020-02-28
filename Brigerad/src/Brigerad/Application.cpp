#include "brpch.h"
#include "Application.h"

#include "Brigerad/Events/ApplicationEvent.h"

#include <GLFW/glfw3.h>

namespace Brigerad
{
Application::Application()
{
    m_window = std::unique_ptr<Window>(Window::Create());
}


Application::~Application()
{

}


void Application::Run()
{
    while (m_running)
    {
        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        m_window->OnUpdate();
    }
}


}
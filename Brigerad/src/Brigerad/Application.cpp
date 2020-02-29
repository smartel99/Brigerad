#include "brpch.h"
#include "Application.h"


#include <GLFW/glfw3.h>

namespace Brigerad
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
    m_window = std::unique_ptr<Window>(Window::Create());
    m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
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

        for (Layer* layer : m_layerStack)
        {
            layer->OnUpdate();
        }

        m_window->OnUpdate();
    }
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

    BR_CORE_TRACE("{0}", e);

    for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.Handled())
        {
            break;
        }
    }
}

void Application::PushLayer(Layer* layer)
{
    m_layerStack.PushLayer(layer);
}

void Application::PushOverlay(Layer* layer)
{
    m_layerStack.PushOverlay(layer);
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_running = false;
    return true;
}


}

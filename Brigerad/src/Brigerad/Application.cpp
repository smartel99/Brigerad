#include "brpch.h"
#include "Application.h"

#include "Input.h"
#include "Core/Time.h"
#include "KeyCodes.h"


namespace Brigerad
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::s_instance = nullptr;


Application::Application()
{
    BR_CORE_ASSERT(!s_instance, "Application already exists!")
        s_instance = this;

    m_window = std::unique_ptr<Window>(Window::Create());
    m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    m_imguiLayer = new ImGuiLayer();

    PushOverlay(m_imguiLayer);
}


Application::~Application()
{

}


void Application::Run()
{
    while (m_running)
    {
        float time = float(GetTime());
        Timestep timestep = time - m_lastFrameTime;
        m_lastFrameTime = time;

        for (Layer* layer : m_layerStack)
        {
            layer->OnUpdate(timestep);
        }

        m_imguiLayer->Begin();

        for (Layer* layer : m_layerStack)
        {
            layer->OnImGuiRender();
        }
        m_imguiLayer->End();

        m_window->OnUpdate();
    }
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
    dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressed));

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
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
    m_layerStack.PushOverlay(layer);
    layer->OnAttach();
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_running = false;
    return true;
}

bool Application::OnKeyPressed(KeyPressedEvent& e)
{
    if (e.GetKeyCode() == BR_KEY_ESCAPE && e.GetRepeatCount() == 0)
    {
        m_imguiLayer->ToggleIsVisible();
        return true;
    }
    else
    {
        return false;
    }
}


}

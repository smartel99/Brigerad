#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Brigerad/LayerStack.h"
#include "Brigerad/Events/ApplicationEvent.h"

#include "Brigerad/Window.h"

namespace Brigerad
{
class BRIGERAD_API Application
{
public:
    Application();
    virtual ~Application();

    void Run();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

private:
    bool OnWindowClose(WindowCloseEvent& e);

    std::unique_ptr<Window> m_window;
    bool m_running = true;
    LayerStack m_layerStack;
};

// To be defined in client.
Application* CreateApplication();
}

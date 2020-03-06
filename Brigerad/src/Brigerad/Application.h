#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Brigerad/LayerStack.h"
#include "Brigerad/Events/ApplicationEvent.h"

#include "Brigerad/Window.h"

#include "Brigerad/ImGui/ImGuiLayer.h"

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

    inline Window& GetWindow()
    {
        return *m_window;
    }

    inline static Application& Get()
    {
        return *s_instance;
    }

private:
    bool OnWindowClose(WindowCloseEvent& e);

    std::unique_ptr<Window> m_window;
    ImGuiLayer* m_imguiLayer;

    bool m_running = true;
    LayerStack m_layerStack;

private:
    static Application* s_instance;
};

// To be defined in client.
Application* CreateApplication();
}

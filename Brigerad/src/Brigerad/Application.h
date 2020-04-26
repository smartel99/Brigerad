#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Brigerad/LayerStack.h"
#include "Brigerad/Events/ApplicationEvent.h"

#include "Brigerad/Window.h"

#include "Brigerad/ImGui/ImGuiLayer.h"

#include "Brigerad/Core/Timestep.h"

#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Brigerad/Renderer/Renderer.h"
#include "Brigerad/Renderer/OrthographicCamera.h"

namespace Brigerad
{
class BRIGERAD_API Application
{
public:
    Application();
    virtual ~Application();

    void Run();

    void OnEvent(Event &e);

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);

    inline Window &GetWindow()
    {
        return *m_window;
    }

    inline static Application &Get()
    {
        return *s_instance;
    }

private:
    bool OnWindowClose(WindowCloseEvent &e);
    bool OnWindowResize(WindowResizeEvent &e);
    bool OnKeyPressed(KeyPressedEvent &e);

private:
    Scope<Window> m_window;
    ImGuiLayer *m_imguiLayer;

    bool m_running = true;
    bool m_minimized = false;
    LayerStack m_layerStack;

    float m_lastFrameTime = 0.0f;

private:
    static Application *s_instance;
};

// To be defined in client.
Application *CreateApplication();
} // namespace Brigerad

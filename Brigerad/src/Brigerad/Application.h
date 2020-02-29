#pragma once
#include "Core.h"
#include "Events/Event.h"
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
private:
    bool OnWindowClose(WindowCloseEvent& e);

    std::unique_ptr<Window> m_window;
    bool m_running = true;
};

// To be defined in client.
Application* CreateApplication();
}


#pragma once

#include "Brigerad/Window.h"

#include <GLFW/glfw3.h>

namespace Brigerad
{
class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow() override;

    void OnUpdate() override;

    inline unsigned int GetWidth() const override
    {
        return m_data.width;
    }
    inline unsigned int GetHeight() const override
    {
        return m_data.height;
    }

    // Window attributes.
    inline void SetEventCallback(const EventCallbackFn& callback) override
    {
        m_data.eventCallback = callback;
    }

    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();

private:
    GLFWwindow* m_window;

    struct WindowData
    {
        std::string title = "";
        unsigned int width = 0, height = 0;
        bool vsync = false;

        EventCallbackFn eventCallback;
    };

    WindowData m_data;
};
}
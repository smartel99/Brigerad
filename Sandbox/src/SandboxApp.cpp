#include "Brigerad.h"

class ExampleLayer : public Brigerad::Layer
{
public:
    ExampleLayer()
        : Layer("Example")
    {
    }

    void OnUpdate() override
    {
        if (Brigerad::Input::IsKeyPressed(BR_KEY_TAB))
        {
            BR_INFO("Tab key is pressed!");
        }
    }

    void OnEvent(Brigerad::Event& event) override
    {
//         BR_TRACE("{0}", event);
    }
};

class Sandbox :public Brigerad::Application
{
public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
        PushOverlay(new Brigerad::ImGuiLayer());
    }

    ~Sandbox() override = default;
};

Brigerad::Application* Brigerad::CreateApplication()
{
    return new Sandbox();
}

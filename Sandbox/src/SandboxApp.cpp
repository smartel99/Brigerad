#include "Brigerad.h"


#include "imgui/imgui.h"

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

    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Test");
        ImGui::TextUnformatted("Hello, World!");
        ImGui::End();
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
    }

    ~Sandbox() override = default;
};

Brigerad::Application* Brigerad::CreateApplication()
{
    return new Sandbox();
}

#pragma once
#include "Brigerad/Layer.h"

#include "Brigerad/Events/KeyEvents.h"
#include "Brigerad/Events/MouseEvent.h"
#include "Brigerad/Events/ApplicationEvent.h"

namespace Brigerad
{
class BRIGERAD_API ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer() override;

    void OnUpdate() override;
    void OnEvent(Event& event) override;
    void OnAttach() override;
    void OnDetach() override;

private:
    bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
    bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
    bool OnMouseMovedEvent(MouseMovedEvent& e);
    bool OnMouseScrolledEvent(MouseScrolledEvent& e);
    bool OnKeyPressedEvent(KeyPressedEvent& e);
    bool OnKeyReleasedEvent(KeyReleasedEvent& e);
    bool OnKeyTypedEvent(KeyTypedEvent& e);
    bool OnWindowResizedEvent(WindowResizeEvent& e);
private:
    float m_time = 0.0f;

};
}
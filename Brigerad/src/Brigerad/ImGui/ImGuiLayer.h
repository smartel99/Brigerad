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

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override;

    void Begin();
    void End();
private:
    float m_time = 0.0f;

};
}
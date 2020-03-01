#pragma once
#include "Brigerad/Layer.h"

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
    float m_time = 0.0f;

};
}
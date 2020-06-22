#pragma once

#include "Brigerad.h"

namespace Brigerad
{
class EditorLayer : public Layer
{
public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Event& e) override;

private:
    OrthographicCameraController m_camera;
    glm::vec4 m_color = { 1.0f, 0.0f, 0.0f, 1.0f };
    Ref<Texture2D> m_texture;
    Ref<Framebuffer> m_fb;

    glm::vec2 m_viewportSize;
};
}

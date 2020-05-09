#pragma once

#include "Brigerad.h"
#include "Shape.h"

class Sandbox2D : public Brigerad::Layer
{
    public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    void OnUpdate(Brigerad::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(Brigerad::Event& e) override;

    private:
    Brigerad::OrthographicCameraController m_camera;
    glm::vec4 m_color = { 1.0f, 0.0f, 0.0f, 1.0f };
    Brigerad::Ref<Brigerad::Texture2D> m_texture;


    struct ProfileResult
    {
        const char* name;
        float time;
    };

    std::vector<ProfileResult> m_profileResults;
};
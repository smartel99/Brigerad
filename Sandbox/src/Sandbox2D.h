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

    // Temp.
    Brigerad::ShaderLibrary m_shaderLibrary;
    Brigerad::Ref<Shape> m_square;
};
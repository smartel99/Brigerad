#include "Sandbox2D.h"
#include "ImGui/imgui.h"
#include <glm/gtc/type_ptr.hpp>


Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_camera(1280.0f / 720.0f) {}

void Sandbox2D::OnAttach()
{
    m_texture = Brigerad::Texture2D::Create("assets/textures/checkboard.png");
}

void Sandbox2D::OnDetach() {}

void Sandbox2D::OnUpdate(Brigerad::Timestep ts)
{
    // Update.
    m_camera.OnUpdate(ts);

    // Render.
    Brigerad::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
    Brigerad::RenderCommand::Clear();

    Brigerad::Renderer2D::BeginScene(m_camera.GetCamera());

    Brigerad::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
    Brigerad::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f }, 45);
    Brigerad::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f },
                                   { 10.0f, 10.0f },
                                   m_texture,
                                   { 10.0f, 10.0f },
                                   { 0.5f, 0.5f, 0.5f, 1.0f });

    Brigerad::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Sandbox Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_color));
    ImGui::End();
}

void Sandbox2D::OnEvent(Brigerad::Event& e) { m_camera.OnEvent(e); }

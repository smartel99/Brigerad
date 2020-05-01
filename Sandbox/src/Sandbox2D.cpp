#include "Sandbox2D.h"
#include "ImGui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_camera(1280.0f / 720.0f) {}

void Sandbox2D::OnAttach()
{
    m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");

    float squareVertices[3 * 4] = {
        //  Ver1
        -0.5f,  // X
        -0.5f,  // Y
        0.0f,   // Z
        // Ver2
        0.5f,
        -0.5f,
        0.0f,
        // Ver3
        0.5f,
        0.5f,
        0.0f,
        // Ver4
        -0.5f,
        0.5f,
        0.0f,
    };

    Brigerad::BufferLayout squareLayout = { { Brigerad::ShaderDataType::Float3, "a_position" } };

    uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

    m_square = Shape::Create(squareVertices,
                             sizeof(squareVertices),
                             squareIndices,
                             sizeof(squareIndices) / sizeof(squareIndices[0]),
                             squareLayout,
                             m_shaderLibrary.Get("FlatColor"),
                             "Square");
}

void Sandbox2D::OnDetach() {}

void Sandbox2D::OnUpdate(Brigerad::Timestep ts)
{
    // Update.
    m_camera.OnUpdate(ts);

    // Render.
    Brigerad::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
    Brigerad::RenderCommand::Clear();

    m_camera.OnUpdate(ts);
    m_square->OnUpdate(ts);

    Brigerad::Renderer::BeginScene(m_camera.GetCamera());


    // TODO: Add Shader::SetMat4, Shader::SetFloat4.
    std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_square->GetShader())->Bind();
    m_square->UploadColor();

    m_square->Submit();

    Brigerad::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Sandbox Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_square->GetColorRef()));
    ImGui::End();
}

void Sandbox2D::OnEvent(Brigerad::Event& e) { m_camera.OnEvent(e); }

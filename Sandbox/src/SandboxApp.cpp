#include "Brigerad.h"
#include "Brigerad/Core/EntryPoint.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "Shape.h"
#include "Sandbox2D.h"

#include "ImGui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#define sizeof_array(x) sizeof(x) / sizeof(x[0])

class ExampleLayer : public Brigerad::Layer
{
public:
    ExampleLayer() : Layer("Example"), m_camera(1280.0f / 720.0f)
    {
        float vertices[3 * 7] = {
            // 0
            -0.5f,
            -0.5f,
            0.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            // 1
            0.5f,
            -0.5f,
            0.0f,
            0.0f,
            1.0f,
            0.0f,
            1.0f,
            // 2
            0.0f,
            0.5f,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
        };

        Brigerad::BufferLayout layout = {
            { Brigerad::ShaderDataType::Float3, "a_position" },
            { Brigerad::ShaderDataType::Float4, "a_Color" },
        };

        uint32_t indices[3] = { 0, 1, 2 };

        m_shaderLibrary.Load("assets/shaders/FlatColor.glsl");
        m_shaderLibrary.Load("assets/shaders/Texture.glsl");
        m_shaderLibrary.Load("assets/shaders/RGB.glsl");

        m_tri = Shape::Create(vertices,
                              sizeof(vertices),
                              indices,
                              sizeof_array(indices),
                              layout,
                              m_shaderLibrary.Get("RGB"),
                              "Triangle");

        float squareVertices[5 * 4] = {
            // 0
            -0.5f,
            -0.5f,
            0.0f,
            0.0f,
            0.0f,
            // 1
            0.5f,
            -0.5f,
            0.0f,
            1.0f,
            0.0f,
            // 2
            0.5f,
            0.5f,
            0.0f,
            1.0f,
            1.0f,
            // 3
            -0.5f,
            0.5f,
            0.0f,
            0.0f,
            1.0f,
        };

        Brigerad::BufferLayout squareLayout = {
            { Brigerad::ShaderDataType::Float3, "a_position" },
            { Brigerad::ShaderDataType::Float2, "a_TextCoord" },
        };

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

        m_square = Shape::Create(squareVertices,
                                 sizeof(squareVertices),
                                 squareIndices,
                                 sizeof_array(squareIndices),
                                 squareLayout,
                                 m_shaderLibrary.Get("FlatColor"),
                                 "Square");

        m_text = Shape::Create(squareVertices,
                               sizeof(squareVertices),
                               squareIndices,
                               sizeof_array(squareIndices),
                               squareLayout,
                               m_shaderLibrary.Get("Texture"),
                               "Texture");
        m_text->SetScale(15.f);

        m_texture =
            Brigerad::Texture2D::Create("assets/textures/checkboard.png");
        m_rald = Brigerad::Texture2D::Create("assets/textures/Rald.png");

        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_text->GetShader())->Bind();
        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_text->GetShader())
            ->UploadUniformInt("u_Texture", 0);
    }

    void OnUpdate(Brigerad::Timestep ts) override
    {
        // Update.
        m_camera.OnUpdate(ts);

        // Render.
        //         BR_TRACE("Delta time: {0}s [{1}ms]", ts.GetSeconds(), ts.GetMilliseconds());
        Brigerad::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
        Brigerad::RenderCommand::Clear();

        m_camera.OnUpdate(ts);
        m_square->OnUpdate(ts);
        m_tri->OnUpdate(ts);

        Brigerad::Renderer::BeginScene(m_camera.GetCamera());

        glm::vec3 origin = m_square->GetPosition();

        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_square->GetShader())->Bind();
        m_square->UploadColor();

        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                glm::vec3 tmpPos((x * 1.1f) + origin.x,
                                 (y * 1.1f) + origin.y,
                                 origin.z);
                m_square->SetPosition(tmpPos);

                m_square->Submit();
            }
        }
        m_square->SetPosition(origin);

        m_texture->Bind(0);
        m_text->Submit();

        m_rald->Bind(0);
        m_text->Submit();

        //         m_tri->Submit();

        Brigerad::Renderer::EndScene();
    }

    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Sandbox Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_square->GetColorRef()));
        ImGui::End();
    }

    void OnEvent(Brigerad::Event& event) override
    {
        m_camera.OnEvent(event);
    }

private:
    Brigerad::ShaderLibrary m_shaderLibrary;
    Brigerad::Ref<Shape> m_square;
    Brigerad::Ref<Shape> m_text;
    Brigerad::Ref<Shape> m_tri;

    Brigerad::Ref<Brigerad::Texture2D> m_texture;
    Brigerad::Ref<Brigerad::Texture2D> m_rald;
    Brigerad::OrthographicCameraController m_camera;
    int m_activeController = 0;
};

class Sandbox : public Brigerad::Application
{
public:
    Sandbox()
    {
        // PushLayer(new ExampleLayer());
        PushLayer(new Sandbox2D());
    }

    ~Sandbox() override = default;
};

Brigerad::Application* Brigerad::CreateApplication()
{
    return new Sandbox();
}

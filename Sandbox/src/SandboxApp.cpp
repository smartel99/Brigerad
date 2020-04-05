#include "Brigerad.h"
#include "Brigerad/Core/Timestep.h"
#include "CameraController.h"

#include "imgui/imgui.h"

class ExampleLayer : public Brigerad::Layer
{
public:
    ExampleLayer()
        : Layer("Example"), m_camera(-16.0f, 16.0f, -9.0f, 9.0f)
    {
        m_vertexArray.reset(Brigerad::VertexArray::Create());

        float vertices[3 * 7] = {
            // X,     Y,    Z,    R,    G,    B,    A
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        };

        std::shared_ptr<Brigerad::VertexBuffer> triVB;
        triVB.reset(Brigerad::VertexBuffer::Create(vertices, sizeof(vertices)));
        Brigerad::BufferLayout layout = {
            { Brigerad::ShaderDataType::Float3, "a_position" },
            { Brigerad::ShaderDataType::Float4, "a_Color" },
        };

        triVB->SetLayout(layout);

        m_vertexArray->AddVertexBuffer(triVB);

        uint32_t indices[3] = {
            0, 1, 2
        };

        std::shared_ptr<Brigerad::IndexBuffer> triI;
        triI.reset(Brigerad::IndexBuffer::Create(indices, 3));
        m_vertexArray->SetIndexBuffer(triI);

        m_squareVA.reset(Brigerad::VertexArray::Create());

        float squareVertices[3 * 4] = {
            // X,     Y,    Z, 
            -0.75f, -0.75f, 0.0f,
             0.75f, -0.75f, 0.0f,
             0.75f,  0.75f, 0.0f,
            -0.75f,  0.75f, 0.0f,
        };


        std::shared_ptr<Brigerad::VertexBuffer> squareVB;
        squareVB.reset(Brigerad::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));

        squareVB->SetLayout({
            { Brigerad::ShaderDataType::Float3, "a_position" }
                            });

        m_squareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = {
            0, 1, 2, 2, 3, 0
        };

        std::shared_ptr<Brigerad::IndexBuffer> squareI;
        squareI.reset(Brigerad::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_squareVA->SetIndexBuffer(squareI);

        std::string vertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_vp;

        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
            // Set the position depending on the model and the camera.
            gl_Position = u_vp * vec4(a_Position, 1.0);
        }
    )";

        std::string fragmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main()
        {
            //color = vec4(v_Position * 0.5 + 0.5, 1.0);
            color = v_Color;
        }
    )";

        std::string redVertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_vp;

        out vec3 v_Position;

        void main()
        {
            v_Position = a_Position;
            // Set the position depending on the model and the camera.
            gl_Position = u_vp * vec4(a_Position, 1.0);
        }
    )";

        std::string redFragmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;


        void main()
        {
            color = vec4(1.0, 0.0, 0.0, 1.0);
        }
    )";

        m_shader.reset(Brigerad::Shader::Create(vertexSrc, fragmentSrc));
        m_redShader.reset(Brigerad::Shader::Create(redVertexSrc, redFragmentSrc));
    }

    void OnUpdate(Brigerad::Timestep ts) override
    {
//         BR_TRACE("Delta time: {0}s [{1}ms]", ts.GetSeconds(), ts.GetMilliseconds());
        Brigerad::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
        Brigerad::RenderCommand::Clear();

        m_camera.OnUpdate(ts);

        Brigerad::Renderer::BeginScene(m_camera);

        Brigerad::Renderer::Submit(m_redShader, m_squareVA);
        Brigerad::Renderer::Submit(m_shader, m_vertexArray);


        Brigerad::Renderer::EndScene();
    }

    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Camera");
        {
            auto pos = m_camera.GetPosition();
            auto vel = m_camera.GetMovement();
            float rot = m_camera.GetRotation();

            ImGui::Text("Position: x=%0.3f y=%0.3f z=%0.3f", pos.x, pos.y, pos.z);
            ImGui::Text("Velocity: x=%0.3f y=%0.3f z=%0.3f", vel.x, vel.y, vel.z);
            ImGui::Text("Rotation: %0.0f deg", rot);

            ImGui::End();
        }
    }

    void OnEvent(Brigerad::Event& event) override
    {
        m_camera.HandleEvent(event);
    }

private:
    std::shared_ptr<Brigerad::Shader> m_shader;
    std::shared_ptr<Brigerad::VertexArray> m_vertexArray;

    std::shared_ptr<Brigerad::Shader> m_redShader;
    std::shared_ptr<Brigerad::VertexArray> m_squareVA;

    CameraController m_camera;
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

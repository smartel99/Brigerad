#include "Brigerad.h"
#include "Brigerad/Core/Timestep.h"
#include "CameraController.h"
#include "Shape.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#define sizeof_array(x) sizeof(x)/sizeof(x[0])

class ExampleLayer : public Brigerad::Layer
{
public:
    ExampleLayer()
        : Layer("Example"), m_camera(-16.0f, 16.0f, -9.0f, 9.0f)
    {

        float vertices[3 * 7] = {
            // X,     Y,    Z,    R,    G,    B,    A
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        };

        Brigerad::BufferLayout layout = {
            { Brigerad::ShaderDataType::Float3, "a_position" },
            { Brigerad::ShaderDataType::Float4, "a_Color" },
        };

        uint32_t indices[3] = {
            0, 1, 2
        };

        std::string vertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;
        layout(location = 1) in vec4 a_Color;

        uniform mat4 u_vp;
        uniform mat4 u_transform;

        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
            // Set the position depending on the model and the camera.
            gl_Position = u_vp * u_transform * vec4(a_Position, 1.0);
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

        m_tri.reset(Shape::Create(vertices, sizeof(vertices),
                                  indices, sizeof_array(indices),
                                  layout, vertexSrc, fragmentSrc, "Triangle"));

        float squareVertices[5 * 4] = {
            // X,     Y,    Z,    Tx,   Ty
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        };

        Brigerad::BufferLayout squareLayout = {
            { Brigerad::ShaderDataType::Float3, "a_position" },
            { Brigerad::ShaderDataType::Float2, "a_TextCoord" },
        };

        uint32_t squareIndices[6] = {
            0, 1, 2, 2, 3, 0
        };

        std::string flatColorVertexSrc = R"(
        #version 330 core
        
        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_vp;
        uniform mat4 u_transform;

        out vec3 v_Position;

        void main()
        {
            v_Position = a_Position;
            // Set the position depending on the model and the camera.
            gl_Position = u_vp * u_transform * vec4(a_Position, 1.0);
        }
        )";

        std::string flatColorFragmentSrc = R"(
        #version 330 core
        
        layout(location = 0) out vec4 color;

        uniform vec3 u_Color;

        void main()
        {
            color = vec4(u_Color, 1.0);
        }
        )";

        m_square.reset(Shape::Create(squareVertices, sizeof(squareVertices),
                                     squareIndices, sizeof_array(squareIndices),
                                     squareLayout, flatColorVertexSrc, flatColorFragmentSrc, "Square"));

        m_text.reset(Shape::Create(squareVertices, sizeof(squareVertices),
                                   squareIndices, sizeof_array(squareIndices),
                                   squareLayout, "assets/shaders/Texture.glsl", "Texture"));
        m_text->SetScale(15.f);

        m_texture = Brigerad::Texture2D::Create("assets/textures/checkboard.png");
        m_rald = Brigerad::Texture2D::Create("assets/textures/rald.png");

        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_text->GetShader())->Bind();
        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_text->GetShader())->UploadUniformInt("u_Texture", 0);
    }

    void OnUpdate(Brigerad::Timestep ts) override
    {
//         BR_TRACE("Delta time: {0}s [{1}ms]", ts.GetSeconds(), ts.GetMilliseconds());
        Brigerad::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
        Brigerad::RenderCommand::Clear();

        m_camera.OnUpdate(ts);
        m_square->OnUpdate(ts);
        m_tri->OnUpdate(ts);

        Brigerad::Renderer::BeginScene(m_camera);

        glm::vec3 origin = m_square->GetPosition();

        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_square->GetShader())->Bind();
        m_square->UploadColor();

        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                glm::vec3 tmpPos((x * 1.1f) + origin.x, (y * 1.1f) + origin.y, origin.z);
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
        const char* controllers[] = { "Camera", "Square", "Triangle" };

        ImGui::Begin("Controllers");
        {
            if (ImGui::BeginCombo("Active Controller", controllers[m_activeController]))
            {
                for (int i = 0; i < sizeof_array(controllers); i++)
                {
                    if (ImGui::Selectable(controllers[i]))
                    {
                        m_activeController = i;
                    }
                }
                ImGui::EndCombo();
            }

            auto pos = glm::vec3(0.0f);
            auto vel = glm::vec3(0.0f);
            auto sca = glm::vec3(0.0f);
            float rot = 0.0f;

            if (m_activeController == 0)
            {
                pos = m_camera.GetPosition();
                vel = m_camera.GetMovement();
                rot = m_camera.GetRotation();
            }
            else if (m_activeController == 1)
            {
                pos = m_square->GetPosition();
                vel = m_square->GetMovement();
                sca = m_square->GetScale();
                rot = m_square->GetRotation();
            }
            else if (m_activeController == 2)
            {
                pos = m_tri->GetPosition();
                vel = m_tri->GetMovement();
                sca = m_tri->GetScale();
                rot = m_tri->GetRotation();
            }

            ImGui::Text("Position: x=%0.3f y=%0.3f z=%0.3f", pos.x, pos.y, pos.z);
            ImGui::Text("Velocity: x=%0.3f y=%0.3f z=%0.3f", vel.x, vel.y, vel.z);
            ImGui::Text("Scale: x=%0.3f y=%0.3f z=%0.3f", sca.x, sca.y, sca.z);
            ImGui::Text("Rotation: %0.0f deg", rot);

            ImGui::End();

            ImGui::Begin("Sandbox Settings");
            ImGui::ColorEdit3("Square Color", glm::value_ptr(m_square->GetColorRef()));
            ImGui::End();
        }
    }

    void OnEvent(Brigerad::Event& event) override
    {
        switch (m_activeController)
        {
            case 0:
                m_camera.HandleEvent(event);
                break;
            case 1:
                m_square->HandleEvent(event);
                break;
            case 2:
                m_tri->HandleEvent(event);
                break;
        }
    }

private:
    Brigerad::Ref<Shape> m_square;
    Brigerad::Ref<Shape> m_text;
    Brigerad::Ref<Shape> m_tri;

    Brigerad::Ref<Brigerad::Texture2D> m_texture;
    Brigerad::Ref<Brigerad::Texture2D> m_rald;
    CameraController m_camera;
    int m_activeController = 0;
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

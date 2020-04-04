#include "brpch.h"
#include "Application.h"

#include "Input.h"


namespace Brigerad
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::s_instance = nullptr;


Application::Application()
{
    BR_CORE_ASSERT(!s_instance, "Application already exists!")
        s_instance = this;

    m_window = std::unique_ptr<Window>(Window::Create());
    m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    m_imguiLayer = new ImGuiLayer();

    PushOverlay(m_imguiLayer);

//     m_vertexArray.reset(VertexArray::Create());
// 
//     float vertices[3 * 7] = {
//         // X,     Y,    Z,    R,    G,    B,    A
//         -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
//          0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
//          0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
//     };
// 
//     std::shared_ptr<VertexBuffer> triVB;
//     triVB.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
//     BufferLayout layout = {
//         { ShaderDataType::Float3, "a_position" },
//         { ShaderDataType::Float4, "a_Color" },
//     };
// 
//     triVB->SetLayout(layout);
// 
//     m_vertexArray->AddVertexBuffer(triVB);
// 
//     uint32_t indices[3] = {
//         0, 1, 2
//     };
// 
//     std::shared_ptr<IndexBuffer> triI;
//     triI.reset(IndexBuffer::Create(indices, 3));
//     m_vertexArray->SetIndexBuffer(triI);
// 
//     m_squareVA.reset(VertexArray::Create());
// 
//     float squareVertices[3 * 4] = {
//         // X,     Y,    Z, 
//         -0.75f, -0.75f, 0.0f,
//          0.75f, -0.75f, 0.0f,
//          0.75f,  0.75f, 0.0f,
//         -0.75f,  0.75f, 0.0f,
//     };
// 
// 
//     std::shared_ptr<VertexBuffer> squareVB;
//     squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
// 
//     squareVB->SetLayout({
//         { ShaderDataType::Float3, "a_position" }
//                         });
// 
//     m_squareVA->AddVertexBuffer(squareVB);
// 
//     uint32_t squareIndices[6] = {
//         0, 1, 2, 2, 3, 0
//     };
// 
//     std::shared_ptr<IndexBuffer> squareI;
//     squareI.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
//     m_squareVA->SetIndexBuffer(squareI);
// 
//     std::string vertexSrc = R"(
//         #version 330 core
//         
//         layout(location = 0) in vec3 a_Position;
//         layout(location = 1) in vec4 a_Color;
// 
//         out vec3 v_Position;
//         out vec4 v_Color;
// 
//         void main()
//         {
//             v_Position = a_Position;
//             v_Color = a_Color;
//             gl_Position = vec4(a_Position, 1.0);
//         }
//     )";
// 
//     std::string fragmentSrc = R"(
//         #version 330 core
//         
//         layout(location = 0) out vec4 color;
// 
//         in vec3 v_Position;
//         in vec4 v_Color;
// 
//         void main()
//         {
//             //color = vec4(v_Position * 0.5 + 0.5, 1.0);
//             color = v_Color;
//         }
//     )";
// 
//     std::string redVertexSrc = R"(
//         #version 330 core
//         
//         layout(location = 0) in vec3 a_Position;
// 
//         out vec3 v_Position;
// 
//         void main()
//         {
//             v_Position = a_Position;
//             gl_Position = vec4(a_Position, 1.0);
//         }
//     )";
// 
//     std::string redFragmentSrc = R"(
//         #version 330 core
//         
//         layout(location = 0) out vec4 color;
// 
// 
//         void main()
//         {
//             color = vec4(1.0, 0.0, 0.0, 1.0);
//         }
//     )";
// 
//     m_shader.reset(Shader::Create(vertexSrc, fragmentSrc));
//     m_redShader.reset(Shader::Create(redVertexSrc, redFragmentSrc));
}


Application::~Application()
{

}


void Application::Run()
{
    while (m_running)
    {
//         RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
//         RenderCommand::Clear();
// 
//         Renderer::BeginScene();
// 
//         m_redShader->Bind();
//         Renderer::Submit(m_squareVA);
// 
//         m_shader->Bind();
//         Renderer::Submit(m_vertexArray);
// 
// 
//         Renderer::EndScene();

        for (Layer* layer : m_layerStack)
        {
            layer->OnUpdate();
        }

        m_imguiLayer->Begin();

        for (Layer* layer : m_layerStack)
        {
            layer->OnImGuiRender();
        }
        m_imguiLayer->End();

        m_window->OnUpdate();
    }
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

//     BR_CORE_TRACE("{0}", e);

    for (auto it = m_layerStack.end(); it != m_layerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.Handled())
        {
            break;
        }
    }
}

void Application::PushLayer(Layer* layer)
{
    m_layerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
    m_layerStack.PushOverlay(layer);
    layer->OnAttach();
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_running = false;
    return true;
}


}

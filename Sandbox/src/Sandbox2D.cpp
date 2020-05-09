#include "Sandbox2D.h"
#include "ImGui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

template<typename Fn>
class Timer
{
public:
    Timer(const char* name, Fn&& func)
        : m_name(name), m_stopped(false), m_func(func)
    {
        m_startPoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        if (m_stopped == false)
        {
            Stop();
        }
    }

    void Stop()
    {
        auto endTP = std::chrono::high_resolution_clock::now();

        long long start =
            std::chrono::time_point_cast<std::chrono::microseconds>(m_startPoint)
                .time_since_epoch().count();
        long long end =
            std::chrono::time_point_cast<std::chrono::microseconds>(endTP)
            .time_since_epoch().count();

        m_stopped = true;

        float duration = (end - start) * 0.001f;
        m_func({ m_name, duration });
    }

private:
    const char* m_name;
    std::chrono::time_point<std::chrono::steady_clock> m_startPoint;
    bool m_stopped;
    Fn m_func;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult pr){m_profileResults.emplace_back(pr);})

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_camera(1280.0f / 720.0f) {}

void Sandbox2D::OnAttach()
{
    m_texture = Brigerad::Texture2D::Create("assets/textures/checkboard.png");
}

void Sandbox2D::OnDetach() {}

void Sandbox2D::OnUpdate(Brigerad::Timestep ts)
{
    PROFILE_SCOPE("Sandbox2D::OnUpdate");
    // Update.
    {
        PROFILE_SCOPE("CameraController::OnUpdate");
        m_camera.OnUpdate(ts);
    }

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

    for (auto& result : m_profileResults)
    {
        char label[50];
        strcpy(label, "%0.3fms  ");
        strcat(label, result.name);

        ImGui::Text(label, result.time);
    }
    m_profileResults.clear();
    ImGui::End();
}

void Sandbox2D::OnEvent(Brigerad::Event& e) { m_camera.OnEvent(e); }

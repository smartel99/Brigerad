/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\ImGui\ImGuiLayer.cpp
 * @author Samuel Martel
 * @date   2020/03/01
 *
 * @brief  Source for the ImGuiLayer module.
 */
#include "brpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "Brigerad/Core/Application.h"

// TEMP
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Brigerad
{
ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
{
}

ImGuiLayer::~ImGuiLayer() = default;

void ImGuiLayer::OnAttach()
{
    BR_PROFILE_FUNCTION();

    // Setup Dear ImGui context.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;                // Enable keyboard control.
                                                         //     io.ConfigFlags |=
                                                         //     ImGuiConfigFlags_NavEnableGamepad;
                                                         //     // Enable Gamepad control.
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable docking.
    io.ConfigFlags |=
      ImGuiConfigFlags_ViewportsEnable;    // Enable multi-viewport / platform window.
                                           //     io.ConfigFlags |=
                                           //     ImGuiConfigFlags_ViewportsNoTaskBarIcons;
                                           //     io.ConfigFlags |=
                                           //     ImGuiConfigFlags_ViewportsNoMerge;
                                           //
                                           // Setup dear Imgui style.
    ImGui::StyleColorsDark();

    // When view ports are enabled we tweak WindowRounding/WindowBg
    // so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Application& app    = Application::Get();
    GLFWwindow*  window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

    // Setup Platform/Renderer bindings.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnDetach()
{
    BR_PROFILE_FUNCTION();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnEvent(Event& event)
{
    if (m_blockImGuiEvents)
    {
        ImGuiIO& io = ImGui::GetIO();
        event.m_handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
        event.m_handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
    }
}

void ImGuiLayer::OnImGuiRender()
{
    BR_PROFILE_FUNCTION();

    m_time = ImGui::GetTime();

    if (m_isProfiling == true)
    {
        if (m_time >= m_profilingStartTime + m_profilingDuration)
        {
            m_isProfiling = false;
            BR_PROFILE_END_SESSION();
        }
    }
    if (m_open == false)
    {
        return;
    }

    auto& window  = Application::Get().GetWindow();
    bool  isVSync = window.IsVSync();
    if (ImGui::Begin("Settings", &m_open))
    {
        if (ImGui::Checkbox("vsync", &isVSync))
        {
            window.SetVSync(isVSync);
            BR_INFO("Set VSync to {0}", isVSync);
        }

        if (ImGui::Button(m_isProfiling == false ? "Start Profiling" : "Stop  Profiling"))
        {
            if (m_isProfiling == false)
            {
                m_profilingStartTime =
                  m_profilingDuration > 0 ? m_time : std::numeric_limits<double>::max();
                BR_PROFILE_BEGIN_SESSION("Profiling Session", "BrigeradProfiling-Session.json");
                m_isProfiling = true;
            }
            else
            {
                BR_PROFILE_END_SESSION();
                m_isProfiling = false;
            }
        }

        ImGui::SameLine();

        ImGui::InputDouble("Profiling Duration", &m_profilingDuration, 0.001, 0.100, "%0.3f");

        ImGui::End();
    }
}

void ImGuiLayer::Begin()
{
    BR_PROFILE_FUNCTION();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    BR_PROFILE_FUNCTION();

    ImGuiIO&     io  = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2(float(app.GetWindow().GetWidth()), float(app.GetWindow().GetHeight()));

    // Rendering.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_window = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_window);
    }
}

}    // namespace Brigerad

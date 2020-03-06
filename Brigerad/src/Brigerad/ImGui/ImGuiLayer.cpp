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

#include "Brigerad/Application.h"

// TEMP
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Brigerad
{
ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiLayer")
{
}



ImGuiLayer::~ImGuiLayer()
{
}


void ImGuiLayer::OnAttach()
{
    // Setup Dear ImGui context.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable keyboard control.
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad control.
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable docking.
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable multi-viewport / platform window.
//     io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
//     io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
//     
    // Setup dear Imgui style.
    ImGui::StyleColorsDark();

    // When view ports are enabled we tweak WindowRounding/WindowBg 
    // so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Application& app = Application::Get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

    // Setup Platform/Renderer bindings.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::OnDetach()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::OnImGuiRender()
{
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
}

void ImGuiLayer::Begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::End()
{
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

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

}

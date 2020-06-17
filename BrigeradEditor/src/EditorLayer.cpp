#include "EditorLayer.h"

#include "ImGui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

namespace Brigerad
{
static void UpdateFPS();

EditorLayer::EditorLayer() : Layer("Brigerad Editor"), m_camera(1280.0f / 720.0f)
{
}

void EditorLayer::OnAttach()
{
    BR_PROFILE_FUNCTION();

    m_texture = Texture2D::Create("assets/textures/checkboard.png");
    m_spriteSheet =
        Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

    m_stairTex = SubTexture2D::CreateFromCoords(m_spriteSheet,
                                                { 8.0f, 6.0f },
                                                { 128.0f, 128.0f });

    m_treeTex = SubTexture2D::CreateFromCoords(m_spriteSheet,
                                               { 2.0f, 1.0f },
                                               { 128.0f, 128.0f },
                                               { 1, 2 });

    FramebufferSpecification spec;
    spec.Width = Application::Get().GetWindow().GetWidth();
    spec.Height = Application::Get().GetWindow().GetHeight();

    m_fb = Framebuffer::Create(spec);
}

void EditorLayer::OnDetach()
{
    BR_PROFILE_FUNCTION();
}

void EditorLayer::OnUpdate(Timestep ts)
{
    BR_PROFILE_FUNCTION();
    // Update.
    m_camera.OnUpdate(ts);

    // Render.
    Renderer2D::ResetStats();
    {
        BR_PROFILE_SCOPE("Renderer Prep");
        m_fb->Bind();
        RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
        RenderCommand::Clear();
    }

    Renderer2D::BeginScene(m_camera.GetCamera());
    Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.0f },
                         { 1.0f, 1.0f },
                         m_stairTex,
                         { 1.0f, 1.0f },
                         { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawQuad({ 1.0f, 0.0f, -0.0f },
                         { 1.0f, 2.0f },
                         m_treeTex,
                         { 1.0f, 1.0f },
                         { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::EndScene();

    m_fb->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    BR_PROFILE_FUNCTION();

    static bool dockspaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
    // all active windows docked into it will lose their parent and become un docked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
    // any change of dock space/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                Application::Get().Close();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }


    ImGui::Begin("Sandbox Settings");

    UpdateFPS();
    auto stats = Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.drawCalls);
    ImGui::Text("Quads: %d", stats.quadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    uint32_t textureId = m_fb->GetColorAttachmentRenderID();
    ImGui::Image((void*)textureId, ImVec2(1280.0f, 720.f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    ImGui::End();

    ImGui::End();
}

void EditorLayer::OnEvent(Event& e)
{
    m_camera.OnEvent(e);
}


void UpdateFPS()
{
    static double lastUpdateTime = 0;
    static double dTimeAvg = 0;
    static int samples = 0;
    static double fps = 0;

    dTimeAvg += ImGui::GetIO().DeltaTime;
    samples++;

    if (ImGui::GetTime() >= lastUpdateTime + 0.5)
    {
        lastUpdateTime = ImGui::GetTime();
        fps = dTimeAvg / samples;
        dTimeAvg = 0;
        samples = 0;
    }

    ImGui::Text("FPS: %0.2f (%0.3fms)", 1 / fps, fps * 1000);
}
}

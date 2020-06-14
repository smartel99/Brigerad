#include "Sandbox2D.h"
#include "ImGui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

static void UpdateFPS();

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_camera(1280.0f / 720.0f)
{
}

void Sandbox2D::OnAttach()
{
    BR_PROFILE_FUNCTION();

    m_texture = Brigerad::Texture2D::Create("assets/textures/checkboard.png");
    m_spriteSheet =
        Brigerad::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

    m_stairTex = Brigerad::SubTexture2D::CreateFromCoords(m_spriteSheet,
                                                          { 8.0f, 6.0f },
                                                          { 128.0f, 128.0f });

    m_treeTex = Brigerad::SubTexture2D::CreateFromCoords(m_spriteSheet,
                                                         { 2.0f, 1.0f },
                                                         { 128.0f, 128.0f },
                                                         { 1, 2 });

    m_particle.colorBegin = { 255 / 255.0f, 0.0f, 0.0f, 1.0f };
    m_particle.colorEnd = { 127 / 255.0f, 0.0f, 0.0f, 1.0f };
    m_particle.sizeBegin = 0.5f, m_particle.sizeVariation = 0.3f, m_particle.sizeEnd = 0.0f;
    m_particle.lifeTime = 1.0f;
    m_particle.velocity = { 0.0f, 0.0f };
    m_particle.velocityVariation = { 3.0f, 1.0f };
    m_particle.position = { 0.0f, 0.0f };

    Brigerad::FramebufferSpecification spec;
    spec.Width = Brigerad::Application::Get().GetWindow().GetWidth();
    spec.Height = Brigerad::Application::Get().GetWindow().GetHeight();

    m_fb = Brigerad::Framebuffer::Create(spec);
}

void Sandbox2D::OnDetach()
{
    BR_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Brigerad::Timestep ts)
{
    BR_PROFILE_FUNCTION();
    // Update.
    m_camera.OnUpdate(ts);

    // Render.
    Brigerad::Renderer2D::ResetStats();
    {
        BR_PROFILE_SCOPE("Renderer Prep");
        m_fb->Bind();
        Brigerad::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
        Brigerad::RenderCommand::Clear();
    }

    // //                       Label,  position,              size
    // if (Brigerad::UI::Button("Test", glm::vec2(0.0f, 2.0f), glm::vec2(2.0f, 1.0f)))
    // {
    //     BR_TRACE("Button pressed!");
    // }

    #if 0
    {
        BR_PROFILE_SCOPE("Renderer Draw");
        Brigerad::Renderer2D::BeginScene(m_camera.GetCamera());

        static float rot = 0;
        rot += ts * 20;
        Brigerad::Renderer2D::DrawQuad({ -1.0f, 0.0f },
                                       { 0.8f, 0.8f },
                                       { 0.8f, 0.2f, 0.3f, 1.0f });

        // for (float i = 0; i < 360; i += 0.05)
        // {
        Brigerad::Renderer2D::DrawRotatedQuad({ -1.5f, -0.5f },
                                              { 0.8f, 0.8f },
                                              { 0.8f, 0.2f, 0.3f, 1.0f },
                                              glm::radians(15.0f));
        // }
        Brigerad::Renderer2D::DrawQuad({ 0.5f, -0.5f },
                                       { 0.5f, 0.75f },
                                       { 0.2f, 0.3f, 0.8f, 1.0f });
        Brigerad::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f },
                                       { 20.0f, 20.0f },
                                       m_texture,
                                       { 100.0f, 100.0f },
                                       { 0.5f, 0.5f, 0.5f, 1.0f });
        Brigerad::Renderer2D::DrawRotatedQuad({ 0.0f, 0.0f },
                                              { 1.0f, 1.0f },
                                              m_texture,
                                              { 1.0f, 1.0f },
                                              { 0.5f, 0.5f, 0.5f, 1.0f },
                                              glm::radians(rot));

        Brigerad::Renderer2D::EndScene();

        Brigerad::Renderer2D::BeginScene(m_camera.GetCamera());

        for (float y = -10.0f; y <= 10.0f; y += 0.5f)
        {
            for (float x = -10.0f; x <= 10.0f; x += 0.5f)
            {
                glm::vec4 color = { (y + 10.0f) / 20.0f, 0.0f, (x + 10.0f) / 20.0f, 0.75f };
                Brigerad::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
            }
        }

        Brigerad::Renderer2D::EndScene();
    }
    #endif

    {
        BR_PROFILE_SCOPE("Particle System");
        if (Brigerad::Input::IsMouseButtonPressed(BR_MOUSE_BUTTON_LEFT))
        {
            auto [x, y] = Brigerad::Input::GetMousePos();
            auto width = Brigerad::Application::Get().GetWindow().GetWidth();
            auto height = Brigerad::Application::Get().GetWindow().GetHeight();

            auto bounds = m_camera.GetBounds();
            auto pos = m_camera.GetCamera().GetPosition();
            x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
            y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
            m_particle.position = { x + pos.x, y + pos.y };
            for (int i = 0; i < 5; i++)
            {
                m_particleSystem.Emit(m_particle);
            }
        }
    }

    Brigerad::Renderer2D::BeginScene(m_camera.GetCamera());
    Brigerad::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.0f },
                                   { 1.0f, 1.0f },
                                   m_stairTex,
                                   { 1.0f, 1.0f },
                                   { 1.0f, 1.0f, 1.0f, 1.0f });
    Brigerad::Renderer2D::DrawQuad({ 1.0f, 0.0f, -0.0f },
                                   { 1.0f, 2.0f },
                                   m_treeTex,
                                   { 1.0f, 1.0f },
                                   { 1.0f, 1.0f, 1.0f, 1.0f });
    Brigerad::Renderer2D::EndScene();

    m_particleSystem.OnUpdate(ts);
    m_particleSystem.OnRender(m_camera.GetCamera());

    m_fb->Unbind();
}

void Sandbox2D::OnImGuiRender()
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
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
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
                Brigerad::Application::Get().Close();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }


    ImGui::Begin("Sandbox Settings");

    UpdateFPS();
    auto stats = Brigerad::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.drawCalls);
    ImGui::Text("Quads: %d", stats.quadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    uint32_t textureId = m_fb->GetColorAttachmentRenderID();
    ImGui::Image((void*)textureId, ImVec2(1280.0f, 720.f), ImVec2(1.0f, 1.0f), ImVec2(0.0f, 0.0f));

    ImGui::End();

    ImGui::End();
}

void Sandbox2D::OnEvent(Brigerad::Event& e)
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

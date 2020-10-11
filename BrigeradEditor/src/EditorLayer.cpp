#include "EditorLayer.h"

#include "ImGui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Brigerad/Utils/Serial.h"

#include <cmath>

namespace Brigerad
{
static void UpdateFps();

EditorLayer::EditorLayer() : Layer("Brigerad Editor")
{
}

void EditorLayer::OnAttach()
{
    BR_PROFILE_FUNCTION();

    FramebufferSpecification spec;
    spec.width  = Application::Get().GetWindow().GetWidth();
    spec.height = Application::Get().GetWindow().GetHeight();

    m_texture = Texture2D::Create("assets/textures/checkboard.png");

    m_fb = Framebuffer::Create(spec);

    m_scene        = CreateRef<Scene>();
    m_squareEntity = m_scene->CreateEntity("Square");
    m_squareEntity.AddComponent<ColorRendererComponent>(glm::vec4 {1.0f, 0.0f, 0.0f, 1.0f});

    class SquareMover : public ScriptableEntity
    {
        virtual void OnUpdate(Timestep ts) override
        {
            const float  speed = 6.283f;
            static float time  = 0.0f;

            time += ts;
            auto& transform = GetComponent<TransformComponent>().transform;
            transform[3][0] = speed * std::sin(time);
            transform[3][1] = speed * std::cos(time);
        }
    };
    // m_squareEntity.AddComponent<NativeScriptComponent>().Bind<SquareMover>();

    m_textureEntity = m_scene->CreateEntity("Textured Square");
    m_textureEntity.AddComponent<TextureRendererComponent>("assets/textures/checkboard.png");


    m_cameraEntity = m_scene->CreateEntity("Camera");
    m_cameraEntity.AddComponent<CameraComponent>();

    m_cameraEntity2                                         = m_scene->CreateEntity("Camera 2");
    m_cameraEntity2.AddComponent<CameraComponent>().primary = false;


    class CameraController : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override
        {
            auto& transform = GetComponent<TransformComponent>().transform;
            transform[3][0] = rand() % 10 - 5.0f;
            transform[3][1] = rand() % 10 - 5.0f;
        }

        virtual void OnUpdate(Timestep ts) override
        {
            if (GetComponent<CameraComponent>().primary == false)
            {
                return;
            }

            auto&       transform = GetComponent<TransformComponent>().transform;
            const float speed     = 5.0f;

            if (Input::IsKeyPressed(KeyCode::A))
            {
                transform[3][0] -= speed * ts;
            }
            if (Input::IsKeyPressed(KeyCode::D))
            {
                transform[3][0] += speed * ts;
            }
            if (Input::IsKeyPressed(KeyCode::W))
            {
                transform[3][1] += speed * ts;
            }
            if (Input::IsKeyPressed(KeyCode::S))
            {
                transform[3][1] -= speed * ts;
            }
        }
    };

    m_cameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    m_cameraEntity2.AddComponent<NativeScriptComponent>().Bind<CameraController>();

    m_sceneHierarchyPanel = SceneHierarchyPanel(m_scene);
}

void EditorLayer::OnDetach()
{
    BR_PROFILE_FUNCTION();
}

void EditorLayer::OnUpdate(Timestep ts)
{
    BR_PROFILE_FUNCTION();

    // Resize.
    FramebufferSpecification spec = m_fb->GetSpecification();
    if (m_viewportSize.x > 0.0f &&
        m_viewportSize.y > 0.0f &&    // zero sized framebuffer is invalid.
        (spec.width != m_viewportSize.x || spec.height != m_viewportSize.y))
    {
        m_fb->Resize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);

        m_scene->OnViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
    }

    // Render.
    Renderer2D::ResetStats();
    m_fb->Bind();
    RenderCommand::SetClearColor({0.2f, 0.2f, 0.2f, 1.0f});
    RenderCommand::Clear();

    // Update scene.
    m_scene->OnUpdate(ts);

    m_fb->Unbind();
}

void EditorLayer::OnImGuiRender()
{
    BR_PROFILE_FUNCTION();

    static bool               dockspaceOpen             = true;
    static bool               opt_fullscreen_persistant = true;
    bool                      opt_fullscreen            = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags           = ImGuiDockNodeFlags_None;

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
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and
    // handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become un docked.
    // We cannot preserve the docking relationship between an active window and an inactive docking,
    // otherwise any change of dock space/settings would lead to windows being stuck in limbo and
    // never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Brigerad DockSpace", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("Brigerad DockSpace ID");
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

    ImGui::Begin("Editor Settings");
    UpdateFps();
    auto stats = Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.drawCalls);
    ImGui::Text("Quads: %d", stats.quadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::Separator();

    if (ImGui::Button("Reload Shader"))
    {
        Renderer2D::Shutdown();
        Renderer2D::Init();
    }

    ImGui::Separator();

    ImGui::Text("%s", m_squareEntity.GetComponent<TagComponent>().tag.c_str());
    auto& col = m_squareEntity.GetComponent<ColorRendererComponent>().color;
    ImGui::ColorEdit4("Square Color", glm::value_ptr(col));

    auto& activeCamera =
      m_cameraEntity.GetComponent<CameraComponent>().primary ? m_cameraEntity : m_cameraEntity2;

    ImGui::DragFloat3("Camera Transform",
                      glm::value_ptr(activeCamera.GetComponent<TransformComponent>().transform[3]));

    {
        auto& camera    = activeCamera.GetComponent<CameraComponent>().camera;
        float orthoSize = camera.GetOrthographicSize();
        if (ImGui::DragFloat("Camera Ortho Size", &orthoSize))
        {
            camera.SetOrthographicSize(orthoSize);
        }
    }

    if (ImGui::Checkbox("Use camera 2", &m_cameraEntity2.GetComponent<CameraComponent>().primary))
    {
        m_cameraEntity.GetComponent<CameraComponent>().primary =
          !m_cameraEntity2.GetComponent<CameraComponent>().primary;
    }

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    m_viewportFocused = ImGui::IsWindowFocused();
    m_viewportHovered = ImGui::IsWindowHovered();
    Application::Get().GetImGuiLayer()->SetBlockEvents(!m_viewportFocused || !m_viewportHovered);

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if (m_viewportSize != *((glm::vec2*)&viewportPanelSize))
    {
        m_viewportSize = {viewportPanelSize.x, viewportPanelSize.y};
    }
    uint32_t textureId = m_fb->GetColorAttachmentRenderID();
    // ImGui takes in a void* for its images.
    ImGui::Image((void*)(uint64_t)textureId,
                 ImVec2(m_viewportSize.x, m_viewportSize.y),
                 ImVec2(0.0f, 0.0f),
                 ImVec2(1.0f, 1.0f));
    ImGui::PopStyleVar();
    ImGui::End();

    m_sceneHierarchyPanel.OnImGuiRender();

    ImGui::End();
}

void EditorLayer::OnEvent(Event& e)
{
}


void UpdateFps()
{
    static double lastUpdateTime = 0;
    static double dTimeAvg       = 0;
    static int    samples        = 0;
    static double fps            = 0;

    dTimeAvg += ImGui::GetIO().DeltaTime;
    samples++;

    if (ImGui::GetTime() >= lastUpdateTime + 0.5)
    {
        lastUpdateTime = ImGui::GetTime();
        fps            = dTimeAvg / samples;
        dTimeAvg       = 0;
        samples        = 0;
    }

    ImGui::Text("FPS: %0.2f (%0.3fms)", 1 / fps, fps * 1000);
}
}    // namespace Brigerad

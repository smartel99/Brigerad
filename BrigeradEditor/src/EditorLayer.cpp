#include "EditorLayer.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

#include <glm/gtc/type_ptr.hpp>

#include "Brigerad/Utils/Serial.h"

#include <cmath>
#include "Brigerad/Events/ImGuiEvents.h"

namespace Brigerad
{

static void UpdateFps();

EditorLayer::EditorLayer() : Layer("Brigerad Editor"), m_Light({glm::vec3(1.0f), glm::vec3(1.0f)})
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

    Ref<TextureCube> envCubeMap =
      TextureCube::Create("assets/textures/environments/Arches_E_PineTree_Radiance.tga");
    Ref<TextureCube> envIrradiance =
      TextureCube::Create("assets/textures/environments/Arches_E_PineTree_Irradiance.tga");

    m_scene = CreateRef<Scene>();
    m_scene->SetSkyboxTexture(envCubeMap);
    m_scene->SetEnvironmentIrradiance(envIrradiance);
    m_brdfLut = Texture2D::Create("assets/textures/BRDF_LUT.tga");

    // m_mesh = Mesh::Create("assets/meshes/razor.obj");
    Ref<Mesh>             mesh = Mesh::Create("assets/meshes/Kaph.obj");
    Ref<MaterialInstance> mat  = CreateRef<MaterialInstance>(mesh->GetMaterial());
    m_meshEntity               = m_scene->CreateEntity("Mesh!");
    m_meshEntity.AddComponent<MeshComponent>("mesh", mesh);
    auto& meshRef       = m_meshEntity.GetComponentRef<MeshComponent>();
    meshRef.BrdfLut     = m_brdfLut;
    meshRef.MaterialRef = mat;

    // Ref<Mesh>             mesh = Mesh::Create("assets/models/m1911/m1911.fbx");
    // Ref<MaterialInstance> mat  = CreateRef<MaterialInstance>(mesh->GetMaterial());
    // m_meshEntity               = m_scene->CreateEntity("Mesh!");
    // m_meshEntity.AddComponent<MeshComponent>("mesh", mesh);
    // auto& meshRef                = m_meshEntity.GetComponentRef<MeshComponent>();
    // meshRef.BrdfLut              = m_brdfLut;
    // meshRef.MaterialRef          = mat;
    // meshRef.Albedo.TextureMap    = Texture2D::Create("assets/models/m1911/m1911_color.png");
    // meshRef.Albedo.UseTexture    = true;
    // meshRef.Metalness.TextureMap = Texture2D::Create("assets/models/m1911/m1911_metalness.png");
    // meshRef.Metalness.UseTexture = true;
    // meshRef.Normal.TextureMap    = Texture2D::Create("assets/models/m1911/m1911_normal.png");
    // meshRef.Normal.UseTexture    = true;
    // meshRef.Roughness.TextureMap = Texture2D::Create("assets/models/m1911/m1911_roughness.png");
    // meshRef.Roughness.UseTexture = true;


    // Set lights
    m_lightEntity = m_scene->CreateEntity("Light");
    m_lightEntity.AddComponent<LightComponent>(glm::vec3 {-0.5f, -0.5f, 1.0f},
                                               glm::vec3 {1.0f, 1.0f, 1.0f});

    // m_imguiWindowEntity = m_scene->CreateEntity("ImGui Window");
    // auto& window        = m_imguiWindowEntity.AddComponent<ImGuiWindowComponent>("Test Window
    // uwu");

    // Entity text = m_scene->CreateChildEntity("ImGui Text", m_imguiWindowEntity);
    // text.AddComponent<ImGuiTextComponent>("Test");
    // window.AddChildEntity(text);

    // Entity button = m_scene->CreateChildEntity("Button", m_imguiWindowEntity);
    // button.AddComponent<ImGuiButtonComponent>("Press here");
    // window.AddChildEntity(button);

    // Entity button2 = m_scene->CreateChildEntity("Smoll Button", m_imguiWindowEntity);
    // button2.AddComponent<ImGuiSmallButtonComponent>("Smoll Button");
    // window.AddChildEntity(button2);

    // Entity button3 = m_scene->CreateChildEntity("Invisible Button", m_imguiWindowEntity);
    // button3.AddComponent<ImGuiInvisibleButtonComponent>("Invisible Button");
    // window.AddChildEntity(button3);

    // Entity left = m_scene->CreateChildEntity("Left Button", m_imguiWindowEntity);
    // left.AddComponent<ImGuiArrowButtonComponent>("Left Button", ImGuiDir_Left);
    // window.AddChildEntity(left);

    // Entity right = m_scene->CreateChildEntity("Right Button", m_imguiWindowEntity);
    // right.AddComponent<ImGuiArrowButtonComponent>("Right Button", ImGuiDir_Right);
    // window.AddChildEntity(right);

    // Entity up = m_scene->CreateChildEntity("Up Button", m_imguiWindowEntity);
    // up.AddComponent<ImGuiArrowButtonComponent>("Up Button", ImGuiDir_Up);
    // window.AddChildEntity(up);

    // Entity down = m_scene->CreateChildEntity("Down Button", m_imguiWindowEntity);
    // down.AddComponent<ImGuiArrowButtonComponent>("Down Button", ImGuiDir_Down);
    // window.AddChildEntity(down);

    // m_squareEntity = m_scene->CreateEntity("Square");
    // m_squareEntity.AddComponent<ColorRendererComponent>(glm::vec4 {1.0f, 0.0f, 0.0f, 1.0f});

    // class SquareMover : public ScriptableEntity
    //{
    //    virtual void OnUpdate(Timestep ts) override
    //    {
    //        const float  speed = 6.283f;
    //        static float time  = 0.0f;

    //        if (m_active)
    //        {
    //            time += ts;
    //            auto& position = GetComponentRef<TransformComponent>().position;
    //            position.x     = speed * std::sin(time);
    //            position.y     = speed * std::cos(time);
    //        }
    //    }

    //    virtual void OnEvent(Event& e) override
    //    {
    //        if (e.GetEventType() == EventType::ImGuiButtonPressed)
    //        {
    //            auto& listener =
    //              GetComponentRef<ImGuiButtonListenerComponent<ImGuiButtonComponent>>();
    //            if (listener.IsButton((*(ImGuiButtonPressedEvent*)&e).GetButton()))
    //            {
    //                m_active = !m_active;
    //            }
    //        }
    //    }

    // private:
    //    bool m_active = false;
    //};
    // m_squareEntity.AddComponent<NativeScriptComponent>().Bind<SquareMover>();
    // m_squareEntity.AddComponent<ImGuiButtonListenerComponent<ImGuiButtonComponent>>(button);

    // m_textureEntity = m_scene->CreateEntity("Textured Square");
    // m_textureEntity.AddComponent<TextureRendererComponent>("assets/textures/checkboard.png");
    // m_textureEntity.AddComponent<LuaScriptComponent>("assets/scripts/test.lua", "Player");


    m_cameraEntity = m_scene->CreateEntity("Camera");
    m_cameraEntity.AddComponent<CameraComponent>();
    auto& cam = m_cameraEntity.GetComponentRef<CameraComponent>();
    cam.camera.SetViewportSize(Application::Get().GetWindow().GetWidth(),
                               Application::Get().GetWindow().GetHeight());
    cam.camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);
    m_cameraEntity.GetComponentRef<TransformComponent>().position.z = 60.0f;

    m_cameraEntity2                                         = m_scene->CreateEntity("Camera 2");
    m_cameraEntity2.AddComponent<CameraComponent>().primary = false;


    class CameraController : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override
        {
            // auto& position = GetComponentRef<TransformComponent>().position;
            // position.x     = rand() % 10 - 5.0f;
            // position.y     = rand() % 10 - 5.0f;
        }

        virtual void OnUpdate(Timestep ts) override
        {
            if (GetComponentRef<CameraComponent>().primary == false)
            {
                return;
            }

            auto&       position = GetComponentRef<TransformComponent>().position;
            const float speed    = 15.0f;

            if (Input::IsKeyPressed(KeyCode::A))
            {
                position.x -= speed * ts;
            }
            if (Input::IsKeyPressed(KeyCode::D))
            {
                position.x += speed * ts;
            }
            if (Input::IsKeyPressed(KeyCode::W))
            {
                position.y += speed * ts;
            }
            if (Input::IsKeyPressed(KeyCode::S))
            {
                position.y -= speed * ts;
            }
        }
    };

    m_cameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    m_cameraEntity2.AddComponent<NativeScriptComponent>().Bind<CameraController>();

    // auto& trans      = m_textEntity.GetComponentRef<TransformComponent>();
    // trans.position.x = -2.70f;
    // trans.position.y = 0.90f;

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

    // auto& mainCamera      = m_cameraEntity.GetComponentRef<CameraComponent>().camera;
    // auto& cameraTransform = m_cameraEntity.GetComponentRef<TransformComponent>();

    // auto& mesh          = m_meshEntity.GetComponentRef<MeshComponent>();
    // auto& meshTransform = m_meshEntity.GetComponentRef<TransformComponent>();

    // mesh.MaterialRef->Set("u_AlbedoColor", m_albedoInput.Color);
    // mesh.MaterialRef->Set("u_Metalness", m_metalnessInput.Value);
    // mesh.MaterialRef->Set("u_Roughness", m_roughnessInput.Value);
    // mesh.MaterialRef->Set("u_ViewProjectionMatrix",
    //                      mainCamera.GetProjection() *
    //                        glm::inverse(cameraTransform.GetTransform()));
    // mesh.MaterialRef->Set("u_ModelMatrix", meshTransform.GetTransform());
    // mesh.MaterialRef->Set("u_Lights", m_Light);
    // mesh.MaterialRef->Set("u_CameraPosition", cameraTransform.GetPosition());
    // mesh.MaterialRef->Set("u_RadiancePrefilter", m_RadiancePrefilter ? 1.0f : 0.0f);
    // mesh.MaterialRef->Set("u_AlbedoTexToggle", m_albedoInput.UseTexture ? 1.0f : 0.0f);
    // mesh.MaterialRef->Set("u_NormalTexToggle", m_normalInput.UseTexture ? 1.0f : 0.0f);
    // mesh.MaterialRef->Set("u_MetalnessTexToggle", m_metalnessInput.UseTexture ? 1.0f : 0.0f);
    // mesh.MaterialRef->Set("u_RoughnessTexToggle", m_roughnessInput.UseTexture ? 1.0f : 0.0f);
    // mesh.MaterialRef->Set("u_EnvRotation", m_EnvMapRotation);
    // mesh.MaterialRef->Set("u_EnvRadianceTex", m_environmentCubeMap);
    // mesh.MaterialRef->Set("u_EnvIrradianceTex", m_environmentIrradiance);
    // mesh.MeshRef->Render(ts, meshTransform.GetTransform(), mesh.MaterialRef);
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

    ImGui::Begin("Stats");
    UpdateFps();
    auto stats = Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.drawCalls);
    ImGui::Text("Quads: %d", stats.quadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
    ImGui::Separator();

    if (ImGui::Button("Reload Static Mesh Shader"))
    {
        Renderer::GetShaderLibrary()->Get("mesh_static")->Reload();
    }
    if (ImGui::Button("Reload Animated Mesh Shader"))
    {
        Renderer::GetShaderLibrary()->Get("mesh_animated")->Reload();
    }
    if (ImGui::Button("Reload Texture Shader"))
    {
        Renderer::GetShaderLibrary()->Get("Texture")->Reload();
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
    // m_mesh->OnImGuiRender();

    ImGui::End();

    ImGui::Begin("Viewport");

    m_scene->OnImguiRender();

    ImGui::End();
}

void EditorLayer::OnEvent(Event& e)
{
    m_scene->OnEvent(e);
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

/**
 * @file    Scene
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    9/25/2020 3:10:36 PM
 *
 * @brief
 ******************************************************************************
 * Copyright (C) 2020  Samuel Martel
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *****************************************************************************/

/*********************************************************************************************************************/
// [SECTION] Includes
/*********************************************************************************************************************/
#include "brpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "Brigerad/Renderer/Renderer2D.h"
#include "Brigerad/Events/ImGuiEvents.h"
#include "Brigerad/Core/Application.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "glm/glm.hpp"

#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"


// TEMP
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Brigerad
{
/*********************************************************************************************************************/
// [SECTION] Private Macro Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
template<typename T>
void DrawImGuiButton(Entity& entity, const std::function<bool(T&)>& func)
{
    if (entity.HasComponent<T>())
    {
        auto& button = entity.GetComponentRef<T>();
        ImGui::PushID(button.GetImGuiID());

        // Update state of button from last frame.
        if (button.state == ImGuiButtonState::Released)
        {
            button.state = ImGuiButtonState::Inactive;
        }
        else if (button.state == ImGuiButtonState::Pressed)
        {
            button.state = ImGuiButtonState::Held;
        }

        // Only returns true when button is released.
        if (func(button))
        {
            button.state = ImGuiButtonState::Released;
            ImGuiButtonReleasedEvent e(entity);
            Application::Get().OnEvent(e);
        }
        // If the button if clicked on:
        else if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered())
        {
            if (button.state != ImGuiButtonState::Held)
            {
                button.state = ImGuiButtonState::Pressed;
                ImGuiButtonPressedEvent e(entity);
                Application::Get().OnEvent(e);
            }
        }
        else
        {
            button.state = ImGuiButtonState::Inactive;
        }

        ImGui::PopID();
    }
}



/*********************************************************************************************************************/
// [SECTION] Public Method Definitions
/*********************************************************************************************************************/
Scene::Scene()
{
    // Default blank skybox texture.
    m_skyboxTexture = TextureCube::Create(4, 3, 4);
    m_skyboxShader  = Shader::Create("assets/shaders/skybox.glsl");
    CreateSkyboxVA();
}

Scene::Scene(const Ref<TextureCube>& texture) : m_skyboxTexture(texture)
{
    m_skyboxShader = Shader::Create("assets/shaders/skybox.glsl");
    CreateSkyboxVA();
}

Scene::~Scene()
{
}

Entity Scene::CreateEntity(const std::string& name)
{
    Entity entity = {m_registry.create(), this};
    entity.AddComponent<TransformComponent>();
    auto& tag = entity.AddComponent<TagComponent>();

    tag.tag = name.empty() ? "<Unknown>" : name;

    return entity;
}

Entity Scene::CreateChildEntity(const std::string& name, Entity parent)
{
    Entity entity = {m_registry.create(), this};
    entity.AddComponent<TransformComponent>();
    auto& tag = entity.AddComponent<TagComponent>();

    tag.tag = name.empty() ? "<Unknown>" : name;

    entity.AddComponent<ChildEntityComponent>(parent);

    // Add child to the parent.
    if (parent.HasComponent<ParentEntityComponent>() == false)
    {
        parent.AddComponent<ParentEntityComponent>();
    }

    auto& parentComponent = parent.GetComponentRef<ParentEntityComponent>().childs;

    parentComponent.emplace_back(entity);

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    m_registry.destroy(entity);
}

void Scene::OnUpdate(Timestep ts)
{
    // Update Scripts.
    {
        m_registry.view<NativeScriptComponent>().each([=](auto entity, NativeScriptComponent& nsc) {
            // TODO: Move to Scene::OnScenePlay
            if (!nsc.instance)
            {
                nsc.instance           = nsc.instantiateScript();
                nsc.instance->m_entity = Entity {entity, this};
                nsc.instance->OnCreate();
            }

            nsc.instance->OnUpdate(ts);
        });
    }
    {
        m_registry.view<LuaScriptComponent>().each([=](auto entity, LuaScriptComponent& sc) {
            // TODO: Move to Scene::OnScenePlay
            if (!sc.instance)
            {
                sc.instance           = sc.InstantiateScript();
                sc.instance->m_entity = Entity {entity, this};
                sc.instance->OnCreate();
            }

            sc.instance->OnUpdate(ts);
        });
    }


    // Render 2D.
    Camera*            mainCamera = nullptr;
    glm::mat4          cameraTransform;
    TransformComponent cameraTransComp;
    {
        auto view = m_registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.primary)
            {
                mainCamera      = &camera.camera;
                cameraTransComp = transform;
                cameraTransform = transform.GetTransform();
            }
        }
    }

    if (mainCamera)
    {
        Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

        auto colorRendererGroup =
          m_registry.group<TransformComponent>(entt::get<ColorRendererComponent>);

        for (auto entity : colorRendererGroup)
        {
            auto [transform, sprite] =
              colorRendererGroup.get<TransformComponent, ColorRendererComponent>(entity);

            Renderer2D::DrawQuad(transform, sprite.color);
        }
        auto textureRendererView = m_registry.view<TransformComponent, TextureRendererComponent>();

        for (auto entity : textureRendererView)
        {
            auto [transform, sprite] =
              textureRendererView.get<TransformComponent, TextureRendererComponent>(entity);

            Renderer2D::DrawQuad(transform, sprite.texture);
        }

        m_registry.view<LuaScriptComponent>().each(
          [=](auto entity, LuaScriptComponent& sc) { sc.instance->OnRender(); });

        auto textEntities = m_registry.view<TransformComponent, TextComponent>();

        for (auto entity : textEntities)
        {
            auto [transform, text] = textEntities.get<TransformComponent, TextComponent>(entity);

            Renderer2D::DrawString(transform.position, text.text, text.scale);
        }

        Renderer2D::EndScene();

        LightComponent::Light sceneLight;
        auto                  lightEntities = m_registry.view<TransformComponent, LightComponent>();
        for (auto entity : lightEntities)
        {
            auto [trans, light] = lightEntities.get<TransformComponent, LightComponent>(entity);

            if (light.isPrimary == true)
            {
                sceneLight = light.light;
            }
        }


        m_skyboxShader->Bind();
        m_skyboxShader->SetMat4(
          "u_InverseVP", glm::inverse(mainCamera->GetProjection() * glm::inverse(cameraTransform)));

        m_environmentIrradiance->Bind(0);
        m_skybox->Bind();
        RenderCommand::DrawIndexed(m_skybox);


        auto meshRendererView = m_registry.view<TransformComponent, MeshComponent>();

        for (auto entity : meshRendererView)
        {
            auto [transform, mesh] =
              meshRendererView.get<TransformComponent, MeshComponent>(entity);

            if (mesh.MeshRef != nullptr && mesh.MaterialRef != nullptr)
            {
                mesh.SetMaterialUniforms();
                mesh.MaterialRef->Set("u_ViewProjectionMatrix",
                                      mainCamera->GetProjection() * glm::inverse(cameraTransform));
                mesh.MaterialRef->Set("u_ModelMatrix", transform.GetTransform());
                mesh.MaterialRef->Set("u_Lights", sceneLight);
                mesh.MaterialRef->Set("u_CameraPosition", cameraTransComp.GetPosition());
                mesh.MaterialRef->Set("u_RadiancePrefilter", m_radiancePrefilter ? 1.0f : 0.0f);
                mesh.MaterialRef->Set("u_EnvMapRotation", m_envMapRotation);
                mesh.MaterialRef->Set("u_EnvRadianceTex", m_skyboxTexture);
                mesh.MaterialRef->Set("u_EnvIrradianceTex", m_environmentIrradiance);
                mesh.MeshRef->Render(ts, transform.GetTransform(), mesh.MaterialRef);
            }
        }
    }
}

void Scene::OnImguiRender()
{
    m_registry.each([&](auto entityID) {
        Entity entity {entityID, this};
        if (!entity.HasComponent<ChildEntityComponent>())
        {
            HandleImGuiEntity(entity);
        }
    });

    auto meshRendererGroup = m_registry.view<MeshComponent>();

    for (auto entity : meshRendererGroup)
    {
        auto& mesh = meshRendererGroup.get<MeshComponent>(entity);

        if (mesh.viewDebugMenu == true)
        {
            mesh.MeshRef->OnImGuiRender();
        }
    }
}

void Scene::OnViewportResize(uint32_t w, uint32_t h)
{
    m_viewportWidth  = w;
    m_viewportHeight = h;

    // Resize our non-FixedAspectRatio cameras.
    auto view = m_registry.view<CameraComponent>();
    for (auto entity : view)
    {
        auto& camera = view.get<CameraComponent>(entity);

        if (!camera.fixedAspectRatio)
        {
            camera.camera.SetViewportSize(w, h);
        }
    }
}

void Scene::OnEvent(Event& e)
{
    m_registry.each([&](auto entityID) {
        Entity entity {entityID, this};
        if (entity.HasComponent<NativeScriptComponent>())
        {
            entity.GetComponent<NativeScriptComponent>().instance->OnEvent(e);
        }
    });
}

void Scene::HandleImGuiEntity(Entity entity)
{
    if (entity.HasComponent<ImGuiWindowComponent>())
    {
        auto& window = entity.GetComponentRef<ImGuiWindowComponent>();
        if (window.isOpen)
        {
            ImGui::Begin(window.name.c_str(), &window.isOpen, window.flags);
            for (const auto& child : window.childs)
            {
                HandleImGuiEntity(child);
            }
            ImGui::End();
        }
    }

    if (entity.HasComponent<ImGuiTextComponent>())
    {
        auto& text = entity.GetComponentRef<ImGuiTextComponent>();
        ImGui::Text("%s", text.text.c_str());
    }

    DrawImGuiButton<ImGuiButtonComponent>(
      entity, [](ImGuiButtonComponent& button) { return ImGui::Button(button.name.c_str()); });

    DrawImGuiButton<ImGuiSmallButtonComponent>(entity, [](ImGuiSmallButtonComponent& button) {
        return ImGui::SmallButton(button.name.c_str());
    });

    DrawImGuiButton<ImGuiInvisibleButtonComponent>(
      entity, [](ImGuiInvisibleButtonComponent& button) {
          return ImGui::InvisibleButton(button.name.c_str(), button.size, button.flag);
      });

    DrawImGuiButton<ImGuiArrowButtonComponent>(entity, [](ImGuiArrowButtonComponent& button) {
        return ImGui::ArrowButton(button.name.c_str(), button.direction);
    });

    if (entity.HasComponent<ImGuiSeparatorComponent>())
    {
        ImGui::Separator();
    }

    if (entity.HasComponent<ImGuiSameLineComponent>())
    {
        auto& sameLine = entity.GetComponent<ImGuiSameLineComponent>();
        ImGui::SameLine(sameLine.offsetFromStartX, sameLine.spacing);
    }
}

void Scene::CreateSkyboxVA()
{
    float x = -1.0f, y = -1.0f;
    float width = 2.0f, height = 2.0f;

    struct QuadVertex
    {
        glm::vec3 position;
        glm::vec2 texCoord;
    };

    QuadVertex* data = new QuadVertex[4];

    data[0].position = glm::vec3(x, y, 0.0f);
    data[0].texCoord = glm::vec2(0.0f, 0.0f);

    data[1].position = glm::vec3(x + width, y, 0.0f);
    data[1].texCoord = glm::vec2(1.0f, 0.0f);

    data[2].position = glm::vec3(x + width, y + height, 0.0f);
    data[2].texCoord = glm::vec2(1.0f, 1.0f);

    data[3].position = glm::vec3(x, y + height, 0.0f);
    data[3].texCoord = glm::vec2(0.0f, 1.0f);

    m_skybox    = VertexArray::Create();
    auto quadVB = VertexBuffer::Create(data, sizeof(QuadVertex) * 4);
    quadVB->SetLayout(
      {{ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float2, "a_TexCoord"}});

    uint32_t indices[6] = {0, 1, 2, 2, 3, 0};
    auto     quadIB     = IndexBuffer::Create(indices, sizeof(uint32_t) * 6);

    m_skybox->AddVertexBuffer(quadVB);
    m_skybox->SetIndexBuffer(quadIB);
}


template<typename T>
void Scene::OnComponentAdded(Entity entity, T& component)
{
    static_assert(false);
}

template<>
void Scene::OnComponentAdded<TagComponent>(Entity, TagComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ChildEntityComponent>(Entity entity, ChildEntityComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ParentEntityComponent>(Entity entity, ParentEntityComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TransformComponent>(Entity, TransformComponent& component)
{
}

template<>
void Scene::OnComponentAdded<CameraComponent>(Entity, CameraComponent& component)
{
    component.camera.SetViewportSize(m_viewportWidth, m_viewportHeight);
}

template<>
void Scene::OnComponentAdded<ColorRendererComponent>(Entity, ColorRendererComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TextureRendererComponent>(Entity, TextureRendererComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TextComponent>(Entity, TextComponent& component)
{
}

template<>
void Scene::OnComponentAdded<NativeScriptComponent>(Entity, NativeScriptComponent& component)
{
}

template<>
void Scene::OnComponentAdded<LuaScriptComponent>(Entity, LuaScriptComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiWindowComponent>(Entity, ImGuiWindowComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiTextComponent>(Entity, ImGuiTextComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiButtonComponent>(Entity, ImGuiButtonComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiButtonListenerComponent<ImGuiButtonComponent>>(
  Entity, ImGuiButtonListenerComponent<ImGuiButtonComponent>& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiSmallButtonComponent>(Entity,
                                                        ImGuiSmallButtonComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiButtonListenerComponent<ImGuiSmallButtonComponent>>(
  Entity, ImGuiButtonListenerComponent<ImGuiSmallButtonComponent>& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiInvisibleButtonComponent>(
  Entity, ImGuiInvisibleButtonComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiButtonListenerComponent<ImGuiInvisibleButtonComponent>>(
  Entity, ImGuiButtonListenerComponent<ImGuiInvisibleButtonComponent>& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiArrowButtonComponent>(Entity,
                                                        ImGuiArrowButtonComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiButtonListenerComponent<ImGuiArrowButtonComponent>>(
  Entity, ImGuiButtonListenerComponent<ImGuiArrowButtonComponent>& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiSeparatorComponent>(Entity, ImGuiSeparatorComponent& component)
{
}

template<>
void Scene::OnComponentAdded<ImGuiSameLineComponent>(Entity, ImGuiSameLineComponent& component)
{
}

template<>
void Scene::OnComponentAdded<MeshComponent>(Entity, MeshComponent& component)
{
}

template<>
void Scene::OnComponentAdded<LightComponent>(Entity entity, LightComponent& component)
{
}


/*********************************************************************************************************************/
// [SECTION] Private Method Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
}    // namespace Brigerad

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


/*********************************************************************************************************************/
// [SECTION] Public Method Definitions
/*********************************************************************************************************************/
Scene::Scene()
{
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
    Camera*   mainCamera = nullptr;
    glm::mat4 cameraTransform;
    {
        auto view = m_registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.primary)
            {
                mainCamera      = &camera.camera;
                cameraTransform = transform.GetTransform();
            }
        }
    }

    if (mainCamera)
    {
        Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

        auto group = m_registry.group<TransformComponent>(entt::get<ColorRendererComponent>);

        for (auto entity : group)
        {
            auto [transform, sprite] =
              group.get<TransformComponent, ColorRendererComponent>(entity);

            Renderer2D::DrawQuad(transform, sprite.color);
        }
        auto view = m_registry.view<TransformComponent, TextureRendererComponent>();

        for (auto entity : view)
        {
            auto [transform, sprite] =
              view.get<TransformComponent, TextureRendererComponent>(entity);

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

    if (entity.HasComponent<ImGuiButtonComponent>())
    {
        auto& button = entity.GetComponentRef<ImGuiButtonComponent>();
        ImGui::PushID(button.GetImGuiID());

        // Update state of button from last frame.
        if (button.state == ImGuiButtonComponent::ButtonState::Released)
        {
            button.state = ImGuiButtonComponent::ButtonState::Inactive;
        }
        else if (button.state == ImGuiButtonComponent::ButtonState::Pressed)
        {
            button.state = ImGuiButtonComponent::ButtonState::Held;
        }

        // Only returns true when button is released.
        if (ImGui::Button(button.name.c_str()))
        {
            button.state = ImGuiButtonComponent::ButtonState::Released;
            ImGuiButtonReleasedEvent e(entity);
            Application::Get().OnEvent(e);
        }
        // If the button if clicked on:
        else if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered())
        {
            if (button.state != ImGuiButtonComponent::ButtonState::Held)
            {
                button.state = ImGuiButtonComponent::ButtonState::Pressed;
                ImGuiButtonPressedEvent e(entity);
                Application::Get().OnEvent(e);
            }
        }
        else
        {
            button.state = ImGuiButtonComponent::ButtonState::Inactive;
        }

        ImGui::PopID();
    }
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
void Scene::OnComponentAdded<ImGuiButtonComponent::Listener>(
  Entity, ImGuiButtonComponent::Listener& component)
{
}


/*********************************************************************************************************************/
// [SECTION] Private Method Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
}    // namespace Brigerad

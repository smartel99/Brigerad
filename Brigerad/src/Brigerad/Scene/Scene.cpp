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


#include "glm/glm.hpp"

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
    Camera*    mainCamera      = nullptr;
    glm::mat4* cameraTransform = nullptr;
    {
        auto view = m_registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

            if (camera.primary)
            {
                mainCamera      = &camera.camera;
                cameraTransform = &transform.transform;
            }
        }
    }

    if (mainCamera)
    {
        Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

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

        Renderer2D::EndScene();
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

/*********************************************************************************************************************/
// [SECTION] Private Method Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
}    // namespace Brigerad

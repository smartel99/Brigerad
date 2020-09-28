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


    entt::entity entity = m_registry.create();
    m_registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
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
    auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

    for (auto entity : group)
    {
        auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

        Renderer2D::DrawQuad(transform, sprite.color);
    }
}

/*********************************************************************************************************************/
// [SECTION] Private Method Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/


}    // namespace Brigerad

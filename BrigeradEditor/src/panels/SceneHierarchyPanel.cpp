/**
 * @file    SceneHierarchyPanel.cpp
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    10/10/2020 5:08:05 PM
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
#include "SceneHierarchyPanel.h"

#include "Brigerad/Scene/Components.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"
#include "ImGui/imgui.h"

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
SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
{
    SetContext(scene);
}

void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
{
    m_context = context;
}

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    m_context->m_registry.each([&](auto entityID) {
        Entity entity {entityID, m_context.get()};
        DrawEntityNode(entity);
    });

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    {
        m_selectionContext = {};
    }
    ImGui::End();

    ImGui::Begin("Properties");
    if (m_selectionContext)
    {
        DrawComponents(m_selectionContext);
    }

    ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    auto& tag = entity.GetComponent<TagComponent>().tag;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

    if (ImGui::IsItemClicked())
    {
        m_selectionContext = entity;
    }

    if (opened)
    {
        ImGui::TreePop();
    }
}

void SceneHierarchyPanel::DrawComponents(Entity entity) const
{
    if (entity.HasComponent<TagComponent>())
    {
        auto& tag = entity.GetComponent<TagComponent>().tag;

        char buffer[256] = {0};
        strcpy_s(buffer, sizeof(buffer), tag.c_str());

        ImGui::Text("Tag");
        ImGui::SameLine();
        if (ImGui::InputText("", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
    }

    if (entity.HasComponent<TransformComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(),
                              ImGuiTreeNodeFlags_DefaultOpen,
                              "Transform"))
        {
            auto& transform = entity.GetComponent<TransformComponent>().transform;

            // bool rebuildTransform = false;

            ImGui::Text("Position");
            ImGui::SameLine();
            if (ImGui::DragFloat3("##pos", glm::value_ptr(transform[3]), 0.1f))
            {
                // rebuildTransform = true;
            }

            //// TODO: Figure this out
            // ImGui::Text("Scaling");
            // ImGui::SameLine();
            // glm::vec3 scale = {transform[0][0], transform[1][1], transform[2][2]};
            // if (ImGui::DragFloat3("##scaling", glm::value_ptr(scale), 0.1f))
            //{
            //    rebuildTransform = true;
            //}

            // ImGui::Text("Rotation");
            // ImGui::SameLine();
            // if (ImGui::DragFloat("##rotation", &rot[3], 0.1f))
            //{
            //    rebuildTransform = true;
            //}
            // if (rebuildTransform)
            //{
            //    transform = glm::scale(transform, scale);
            //}

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<ColorRendererComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(ColorRendererComponent).hash_code(),
                              ImGuiTreeNodeFlags_DefaultOpen,
                              "Sprite Renderer"))
        {
            auto& col = entity.GetComponent<ColorRendererComponent>().color;
            ImGui::Text("Color");
            ImGui::SameLine();
            ImGui::ColorEdit4("##color", glm::value_ptr(col));
            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<TextureRendererComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(TextureRendererComponent).hash_code(),
                              ImGuiTreeNodeFlags_DefaultOpen,
                              "Texture Renderer"))
        {
            auto& texturePath = entity.GetComponent<TextureRendererComponent>().path;
            ImGui::Text("Path: %s", texturePath.c_str());

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<CameraComponent>())
    {
        if (ImGui::TreeNodeEx(
              (void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
        {
            auto& camera = entity.GetComponent<CameraComponent>();
            ImGui::Checkbox("Primary", &camera.primary);
            ImGui::Checkbox("Fixed Aspect Ratio", &camera.fixedAspectRatio);

            float size = camera.camera.GetOrthographicSize();
            ImGui::Text("Size");
            ImGui::SameLine();
            if (ImGui::DragFloat("##cameraSize", &size, 0.1f))
            {
                camera.camera.SetOrthographicSize(size);
            }
            ImGui::TreePop();
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

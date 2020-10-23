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

#include <fstream>

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
    auto& tag = entity.GetComponentRef<TagComponent>().tag;

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
        auto& tag = entity.GetComponentRef<TagComponent>().tag;

        char buffer[256] = {0};
        strcpy_s(buffer, sizeof(buffer), tag.c_str());

        ImGui::Columns(2);
        ImGui::TextUnformatted("Tag");
        ImGui::NextColumn();
        if (ImGui::InputText("", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
        ImGui::Columns();
    }

    if (entity.HasComponent<TransformComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(),
                              ImGuiTreeNodeFlags_DefaultOpen,
                              "Transform"))
        {
            auto& transform = entity.GetComponentRef<TransformComponent>();

            ImGui::Columns(2);
            ImGui::TextUnformatted("Position");
            ImGui::NextColumn();
            if (ImGui::DragFloat3("##pos", glm::value_ptr(transform.position), 0.1f))
            {
                transform.RecalculateTransform();
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("Scaling");
            ImGui::NextColumn();
            if (ImGui::DragFloat3("##scaling", glm::value_ptr(transform.scale), 0.1f))
            {
                transform.RecalculateTransform();
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("Rotation");
            ImGui::NextColumn();
            if (ImGui::DragFloat3(
                  "##rotation", glm::value_ptr(transform.rotation), 0.1f, -360.0f, 360.0f))
            {
                transform.RecalculateTransform();
            }

            ImGui::Columns();
            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<ColorRendererComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(ColorRendererComponent).hash_code(),
                              ImGuiTreeNodeFlags_DefaultOpen,
                              "Sprite Renderer"))
        {
            auto& col = entity.GetComponentRef<ColorRendererComponent>().color;
            ImGui::Columns(2);
            ImGui::TextUnformatted("Color");
            ImGui::NextColumn();
            ImGui::ColorEdit4("##color", glm::value_ptr(col));
            ImGui::Columns();

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<TextureRendererComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(TextureRendererComponent).hash_code(),
                              ImGuiTreeNodeFlags_DefaultOpen,
                              "Texture Renderer"))
        {
            auto& texturePath = entity.GetComponentRef<TextureRendererComponent>().path;
            ImGui::Text("Path: %s", texturePath.c_str());

            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<CameraComponent>())
    {
        if (ImGui::TreeNodeEx(
              (void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
        {
            auto& camera = entity.GetComponentRef<CameraComponent>();

            const char* projTypeStrings[] = {"Perspective", "Orthographic"};
            const char* currentProjType =
              projTypeStrings[(size_t)camera.camera.GetProjectionType()];

            ImGui::Columns(2);
            ImGui::TextUnformatted("Projection");
            ImGui::NextColumn();
            if (ImGui::BeginCombo("##Projection", currentProjType))
            {
                for (int i = 0; i < 2; i++)
                {
                    bool isSelected = (currentProjType == projTypeStrings[i]);
                    if (ImGui::Selectable(projTypeStrings[i], isSelected))
                    {
                        camera.camera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (camera.camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                ImGui::NextColumn();
                ImGui::TextUnformatted("FoV");
                ImGui::NextColumn();
                float fov = camera.camera.GetPerspectiveFov();
                if (ImGui::DragFloat("##Size", &fov, 0.01f, 0.00001f, 10000000.0f))
                {
                    camera.camera.SetPerspectiveFov(fov);
                    camera.camera.RecalculatePerspectiveProjection();
                }

                ImGui::NextColumn();
                ImGui::TextUnformatted("Near Clip");
                ImGui::NextColumn();
                float nearClip = camera.camera.GetPerspectiveNearClip();
                if (ImGui::DragFloat("##Near", &nearClip, 0.01f, 0.00001f, 10000000.0f))
                {
                    camera.camera.SetPerspectiveNearClip(nearClip);
                    camera.camera.RecalculatePerspectiveProjection();
                }

                ImGui::NextColumn();
                ImGui::TextUnformatted("Far Clip");
                ImGui::NextColumn();
                float farClip = camera.camera.GetPerspectiveFarClip();
                if (ImGui::DragFloat("##Far", &farClip, 0.01f, 0.00001f, 10000000.0f))
                {
                    camera.camera.SetPerspectiveFarClip(farClip);
                    camera.camera.RecalculatePerspectiveProjection();
                }
            }

            else if (camera.camera.GetProjectionType() == SceneCamera::ProjectionType::Ortographic)
            {
                ImGui::NextColumn();
                ImGui::TextUnformatted("Size");
                ImGui::NextColumn();
                float size = camera.camera.GetOrthographicSize();
                if (ImGui::DragFloat("##Size", &size, 0.01f, 0.00001f, 10000000.0f))
                {
                    camera.camera.SetOrthographicSize(size);
                    camera.camera.RecalculateOrthographicProjection();
                }

                ImGui::NextColumn();
                ImGui::TextUnformatted("Near Clip");
                ImGui::NextColumn();
                float nearClip = camera.camera.GetOrtographicNearClip();
                if (ImGui::DragFloat("##Near", &nearClip, 0.01f, -1000.0f, 1000.0f))
                {
                    camera.camera.SetOrtographicNearClip(nearClip);
                    camera.camera.RecalculateOrthographicProjection();
                }

                ImGui::NextColumn();
                ImGui::TextUnformatted("Far Clip");
                ImGui::NextColumn();
                float farClip = camera.camera.GetOrtographicFarClip();
                if (ImGui::DragFloat("##Far", &farClip, 0.01f, -1000.0f, 1000.0f))
                {
                    camera.camera.SetOrtographicFarClip(farClip);
                    camera.camera.RecalculateOrthographicProjection();
                }
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("Primary");
            ImGui::NextColumn();
            ImGui::Checkbox("##primary", &camera.primary);

            ImGui::NextColumn();
            ImGui::TextUnformatted("Fixed Aspect Ratio");
            ImGui::NextColumn();
            ImGui::Checkbox("##fixedAR", &camera.fixedAspectRatio);

            ImGui::Columns(1);
            ImGui::TreePop();
        }
    }

    if (entity.HasComponent<LuaScriptComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(LuaScriptComponent).hash_code(),
                              ImGuiTreeNodeFlags_DefaultOpen,
                              "Lua Script"))
        {
            auto& script = entity.GetComponentRef<LuaScriptComponent>();
            ImGui::Text("Script Path: %s", script.path.c_str());

            if (ImGui::Button("Reload Script"))
            {
                script.ReloadScript();
            }

            static bool                     isViewerOpen = false;
            static std::vector<std::string> fileContent;

            if (ImGui::Button("Open Script"))
            {
                isViewerOpen = true;

                // Load the file's content into a buffer to display.
                std::ifstream file(script.path);
                if (file.is_open())
                {
                    std::string line = "";
                    while (std::getline(file, line))
                    {
                        fileContent.push_back(line);
                    }
                }

                else
                {
                    BR_CORE_ERROR("Unable to open \"{}\"", script.path.c_str());
                }
            }

            if (isViewerOpen)
            {
                ImGui::Begin(script.path.c_str(), &isViewerOpen);

                for (const auto& line : fileContent)
                {
                    ImGui::TextUnformatted(line.c_str());
                }

                ImGui::End();
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

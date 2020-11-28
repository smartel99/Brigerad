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
#include "ImGui/imgui_internal.h"

#include <fstream>

namespace Brigerad
{
/*********************************************************************************************************************/
// [SECTION] Private Macro Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
static void DrawVec3Control(const std::string& label,
                            glm::vec3&         values,
                            float              resetValue  = 0.0f,
                            float              columnWidth = 100.0f);

template<typename T, typename UIFunction>
static void DrawComponent(const std::string& label, Entity entity, UIFunction function)
{
    const ImGuiTreeNodeFlags treeNodeFlags =
      ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;
    if (entity.HasComponent<T>())
    {
        auto& component = entity.GetComponentRef<T>();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 {4, 4});

        bool open = ImGui::TreeNodeEx(
          (void*)typeid(LuaScriptComponent).hash_code(), treeNodeFlags, label.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
        if (ImGui::Button("+", ImVec2 {20, 20}))
        {
            ImGui::OpenPopup("ComponentSettings");
        }
        ImGui::PopStyleVar();

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (ImGui::MenuItem("Remove component"))
            {
                removeComponent = true;
            }
            ImGui::EndPopup();
        }

        if (open)
        {
            function(component);
            ImGui::TreePop();
        }

        if (removeComponent)
        {
            entity.RemoveComponent<T>();
        }
    }
}


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

    // Right-click on blank space
    if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
    {
        if (ImGui::MenuItem("Create Empty Entity"))
        {
            m_context->CreateEntity("Empty Entity");
        }

        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Properties");
    if (m_selectionContext)
    {
        DrawComponents(m_selectionContext);

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");
        }

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (ImGui::MenuItem(
                  "Camera", nullptr, nullptr, !m_selectionContext.HasComponent<CameraComponent>()))
            {
                m_selectionContext.AddComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Color Renderer Component",
                                nullptr,
                                nullptr,
                                !m_selectionContext.HasComponent<ColorRendererComponent>()))
            {
                m_selectionContext.AddComponent<ColorRendererComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Texture Renderer Component",
                                nullptr,
                                nullptr,
                                !m_selectionContext.HasComponent<TextureRendererComponent>()))
            {
                m_selectionContext.AddComponent<TextureRendererComponent>();
                ImGui::CloseCurrentPopup();
            }

            // if (ImGui::MenuItem("Lua Script Component"))
            //{
            //    m_selectionContext.AddComponent<LuaScriptComponent>();
            //    ImGui::CloseCurrentPopup();
            //}

            ImGui::EndPopup();
        }
    }

    ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    auto& tag = entity.GetComponentRef<TagComponent>().tag;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_OpenOnDoubleClick |
                               ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0);
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

    if (ImGui::IsItemClicked())
    {
        m_selectionContext = entity;
    }

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete Entity"))
        {
            entityDeleted = true;
        }

        ImGui::EndPopup();
    }

    if (opened)
    {
        ImGuiTreeNodeFlags flags =
          ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)tag.c_str(), flags, tag.c_str());
        if (opened)
        {
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    if (entityDeleted)
    {
        m_context->DestroyEntity(entity);
        if (m_selectionContext == entity)
        {
            m_selectionContext = {};
        }
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
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
        ImGui::Columns();
    }

    DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
        DrawVec3Control("Translation", component.position);
        DrawVec3Control("Scaling", component.scale, 1.0f);
        DrawVec3Control("Rotation", component.rotation);
    });

    DrawComponent<ColorRendererComponent>("Color Renderer", entity, [](auto& component) {
        ImGui::Columns(2);
        ImGui::TextUnformatted("Color");
        ImGui::NextColumn();
        ImGui::ColorEdit4("##color", glm::value_ptr(component.color));
        ImGui::Columns();
    });

    DrawComponent<TextureRendererComponent>("Texture Renderer", entity, [](auto& component) {
        ImGui::Text("Path: %s", component.path.c_str());
    });

    DrawComponent<CameraComponent>("Camera", entity, [](auto& camera) {
        const char* projTypeStrings[] = {"Perspective", "Orthographic"};
        const char* currentProjType   = projTypeStrings[(size_t)camera.camera.GetProjectionType()];

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
            ImGui::TextUnformatted("Vertical FOV");
            ImGui::NextColumn();
            float fov = glm::degrees(camera.camera.GetPerspectiveFov());
            if (ImGui::DragFloat("##fov", &fov, 0.01f, 0.00001f, 10000000.0f))
            {
                camera.camera.SetPerspectiveFov(glm::radians(fov));
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("Near Clip");
            ImGui::NextColumn();
            float nearClip = camera.camera.GetPerspectiveNearClip();
            if (ImGui::DragFloat("##Near", &nearClip, 0.01f, 0.00001f, 10000000.0f))
            {
                camera.camera.SetPerspectiveNearClip(nearClip);
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("Far Clip");
            ImGui::NextColumn();
            float farClip = camera.camera.GetPerspectiveFarClip();
            if (ImGui::DragFloat("##Far", &farClip, 0.01f, 0.00001f, 10000000.0f))
            {
                camera.camera.SetPerspectiveFarClip(farClip);
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
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("Near Clip");
            ImGui::NextColumn();
            float nearClip = camera.camera.GetOrtographicNearClip();
            if (ImGui::DragFloat("##Near", &nearClip, 0.01f, -1000.0f, 1000.0f))
            {
                camera.camera.SetOrtographicNearClip(nearClip);
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("Far Clip");
            ImGui::NextColumn();
            float farClip = camera.camera.GetOrtographicFarClip();
            if (ImGui::DragFloat("##Far", &farClip, 0.01f, -1000.0f, 1000.0f))
            {
                camera.camera.SetOrtographicFarClip(farClip);
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
    });

    DrawComponent<LuaScriptComponent>("Lua Script", entity, [](auto& script) {
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
    });
}

/*********************************************************************************************************************/
// [SECTION] Private Method Definitions
/*********************************************************************************************************************/
void DrawVec3Control(const std::string& label,
                     glm::vec3&         values,
                     float              resetValue /*= 0.0f*/,
                     float              columnWidth /*= 100.0f*/)
{
    ImGuiIO& io       = ImGui::GetIO();
    auto     boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

    float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
        values.x = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.8f, 0.3f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
        values.y = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize))
        values.z = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/
}    // namespace Brigerad

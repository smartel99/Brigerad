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
#define IMGUI_WINDOW_FLAGS_OPTION(window, label, flag)                                             \
    do                                                                                             \
    {                                                                                              \
        ImGui::TextUnformatted(label);                                                             \
        ImGui::NextColumn();                                                                       \
        bool isTrue = window.flags & flag;                                                         \
        if (ImGui::Checkbox("##" label, &isTrue))                                                  \
        {                                                                                          \
            if (isTrue)                                                                            \
            {                                                                                      \
                window.flags |= flag;                                                              \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                window.flags ^= flag;                                                              \
            }                                                                                      \
        }                                                                                          \
        ImGui::NextColumn();                                                                       \
    } while (0)

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

        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, label.c_str());
        ImGui::SameLine(ImGui::GetWindowWidth() - (ImGui::GetTextLineHeight() * 2));
        if (ImGui::SmallButton("+"))
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
        // If entity is not a child entity:
        if (!entity.HasComponent<ChildEntityComponent>())
        {
            // Render it.
            DrawEntityNode(entity);
        }
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

            if (ImGui::MenuItem("Text Component",
                                nullptr,
                                nullptr,
                                !m_selectionContext.HasComponent<TextComponent>()))
            {
                m_selectionContext.AddComponent<TextComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("ImGui Window",
                                nullptr,
                                nullptr,
                                !m_selectionContext.HasComponent<ImGuiWindowComponent>()))
            {
                m_selectionContext.AddComponent<ImGuiWindowComponent>();
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

        if (ImGui::MenuItem("Create Empty Child"))
        {
            m_context->CreateChildEntity("Empty Child", entity);
        }

        ImGui::EndPopup();
    }

    if (opened)
    {
        ImGuiTreeNodeFlags flags =
          ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        // If this entity has childs:
        if (entity.HasComponent<ParentEntityComponent>())
        {
            // Render them too.
            const auto& childs = entity.GetComponent<ParentEntityComponent>().childs;
            for (const auto& child : childs)
            {
                DrawEntityNode(child);
            }
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
            float nearClip = camera.camera.GetOrthographicNearClip();
            if (ImGui::DragFloat("##Near", &nearClip, 0.01f, -1000.0f, 1000.0f))
            {
                camera.camera.SetOrthographicNearClip(nearClip);
            }

            ImGui::NextColumn();
            ImGui::TextUnformatted("Far Clip");
            ImGui::NextColumn();
            float farClip = camera.camera.GetOrthographicFarClip();
            if (ImGui::DragFloat("##Far", &farClip, 0.01f, -1000.0f, 1000.0f))
            {
                camera.camera.SetOrthographicFarClip(farClip);
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

    DrawComponent<TextComponent>("Text", entity, [](auto& text) {
        char buffer[256] = {0};
        strcpy_s(buffer, sizeof(buffer), text.text.c_str());

        ImGui::Columns(2);
        ImGui::TextUnformatted("Text");
        ImGui::NextColumn();
        if (ImGui::InputText("##Text", buffer, sizeof(buffer)))
        {
            text.text = std::string(buffer);
        }
        ImGui::NextColumn();
        ImGui::TextUnformatted("Scale");
        ImGui::NextColumn();
        ImGui::DragFloat("##TextScale", &text.scale, 0.005f, 0.0f, 1.0f);
        ImGui::Columns();
    });

    DrawComponent<ImGuiWindowComponent>("ImGui Window", entity, [this](auto& window) {
        char buffer[256] = {0};
        strcpy_s(buffer, sizeof(buffer), window.name.c_str());

        ImGui::Columns(2);
        ImGui::TextUnformatted("Name");
        ImGui::NextColumn();
        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
        {
            window.name = std::string(buffer);
        }

        ImGui::NextColumn();
        ImGui::TextUnformatted("Open");
        ImGui::NextColumn();
        ImGui::Checkbox("##Open", &window.isOpen);

        ImGui::Columns(1);
        if (ImGui::TreeNode("Window Flags"))
        {
            ImGui::Columns(2);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Title Bar", ImGuiWindowFlags_NoTitleBar);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Resize", ImGuiWindowFlags_NoResize);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Move", ImGuiWindowFlags_NoMove);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Scrollbar", ImGuiWindowFlags_NoScrollbar);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "No Scroll With Mouse", ImGuiWindowFlags_NoScrollWithMouse);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Collapse", ImGuiWindowFlags_NoCollapse);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "Always Auto Resize", ImGuiWindowFlags_AlwaysAutoResize);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Background", ImGuiWindowFlags_NoBackground);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "No Saved Settings", ImGuiWindowFlags_NoSavedSettings);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Mouse Inputs", ImGuiWindowFlags_NoMouseInputs);
            IMGUI_WINDOW_FLAGS_OPTION(window, "Menu Bar", ImGuiWindowFlags_MenuBar);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "Horizontal Scrollbar", ImGuiWindowFlags_HorizontalScrollbar);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "No Focus On Appearing", ImGuiWindowFlags_NoFocusOnAppearing);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "No Bring To Front On Focus", ImGuiWindowFlags_NoBringToFrontOnFocus);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "Always Vertical Scrollbar", ImGuiWindowFlags_AlwaysVerticalScrollbar);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "Always Horizontal Scrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            IMGUI_WINDOW_FLAGS_OPTION(
              window, "Always Use Window Padding", ImGuiWindowFlags_AlwaysUseWindowPadding);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Navigation Inputs", ImGuiWindowFlags_NoNavInputs);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Navigation Focus", ImGuiWindowFlags_NoNavFocus);
            IMGUI_WINDOW_FLAGS_OPTION(window, "Unsaved Document", ImGuiWindowFlags_UnsavedDocument);
            IMGUI_WINDOW_FLAGS_OPTION(window, "No Docking", ImGuiWindowFlags_NoDocking);
            ImGui::Columns(1);
            ImGui::TreePop();
        }

        ImGui::TextUnformatted("Child Widgets");
        ImGui::Indent();
        int moveFrom = -1, moveTo = -1;
        for (int i = 0; i < window.childs.size(); i++)
        {
            const char* childName = window.childs[i].GetComponent<TagComponent>().tag.c_str();
            ImGui::Selectable(childName);

            ImGuiDragDropFlags srcFlags = 0;
            // Keep the source displayed as hovered.
            srcFlags |= ImGuiDragDropFlags_SourceNoDisableHover;
            // Because our dragging is local, we disable the feature of opening foreign
            // tree nodes/tabs while dragging.
            srcFlags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;
            // Hide the tooltip.
            // srcFlags |= ImGuiDragDropFlags_SourceNoPreviewTooltip;

            if (ImGui::BeginDragDropSource(srcFlags))
            {
                if (!(srcFlags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                {
                    ImGui::Text("Moving \"%s\"", childName);
                }
                ImGui::SetDragDropPayload("IMGUI widget order", &i, sizeof(int));
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget())
            {
                ImGuiDragDropFlags targetFlags = 0;
                // Don't wait until the delivery (release mouse button on a target) to do something.
                targetFlags |= ImGuiDragDropFlags_AcceptBeforeDelivery;
                // Don't display the yellow rectangle.
                // targetFlags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect;
                if (const ImGuiPayload* payload =
                      ImGui::AcceptDragDropPayload("IMGUI widget order", targetFlags))
                {
                    moveFrom = *(const int*)payload->Data;
                    moveTo   = i;
                }
                ImGui::EndDragDropTarget();
            }
        }

        // If an item was dragged and dropped elsewhere:
        if (moveFrom != -1 && moveTo != -1)
        {
            // Reorder the list.
            int    copyDst         = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
            int    copySrc         = (moveFrom < moveTo) ? moveFrom + 1 : moveTo;
            int    copyCnt         = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
            Entity tmp             = window.childs[copyDst];
            window.childs[copyDst] = window.childs[copySrc];
            window.childs[copySrc] = tmp;

            // Update payload immediately so on the next frame if we move the mouse to an earlier
            // item our index payload will be correct.
            ImGui::SetDragDropPayload("IMGUI widget order", &moveTo, sizeof(int));
        }
        ImGui::Unindent();

        if (ImGui::Button("Add ImGui widget"))
        {
            ImGui::OpenPopup("AddWidget");
        }

        if (ImGui::BeginPopup("AddWidget"))
        {
            if (ImGui::MenuItem("Text"))
            {
                Entity newWidget = m_context->CreateChildEntity("ImGui Text", m_selectionContext);
                newWidget.AddComponent<ImGuiTextComponent>("Placeholder");
                window.AddChildEntity(newWidget);
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::BeginMenu("Button"))
            {
                if (ImGui::MenuItem("Button"))
                {
                    Entity newWidget =
                      m_context->CreateChildEntity("ImGui Button", m_selectionContext);
                    newWidget.AddComponent<ImGuiButtonComponent>("Button");
                    window.AddChildEntity(newWidget);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Small Button"))
                {
                    Entity newWidget =
                      m_context->CreateChildEntity("ImGui Small Button", m_selectionContext);
                    newWidget.AddComponent<ImGuiSmallButtonComponent>("Small Button");
                    window.AddChildEntity(newWidget);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Invisible Button"))
                {
                    Entity newWidget =
                      m_context->CreateChildEntity("ImGui Invisible Button", m_selectionContext);
                    newWidget.AddComponent<ImGuiInvisibleButtonComponent>("Invisible Button");
                    window.AddChildEntity(newWidget);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Arrow Button"))
                {
                    Entity newWidget =
                      m_context->CreateChildEntity("ImGui Arrow Button", m_selectionContext);
                    newWidget.AddComponent<ImGuiArrowButtonComponent>("Arrow Button");
                    window.AddChildEntity(newWidget);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }
    });

    DrawComponent<ImGuiTextComponent>("ImGui Text", entity, [](auto& text) {
        char buffer[512] = {0};
        strcpy_s(buffer, sizeof(buffer), text.text.c_str());

        ImGui::Columns(2);
        ImGui::TextUnformatted("Name");
        ImGui::NextColumn();
        if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
        {
            text.text = std::string(buffer);
        }
        ImGui::Columns();
    });

    DrawComponent<ImGuiButtonComponent>("ImGui Button", entity, [](auto& button) {
        char buffer[512] = {0};
        strcpy_s(buffer, sizeof(buffer), button.name.c_str());

        ImGui::Columns(2);
        ImGui::TextUnformatted("Label");
        ImGui::NextColumn();
        if (ImGui::InputText("##Label", buffer, sizeof(buffer)))
        {
            button.name = std::string(buffer);
        }
        ImGui::NextColumn();

        static const char* states[] = {"Inactive", "Pressed", "Held", "Released"};
        ImGui::TextUnformatted("Current State:");
        ImGui::NextColumn();
        ImGui::Text("%s", states[static_cast<int>(button.state)]);
        ImGui::Columns();
    });

    DrawComponent<ImGuiSmallButtonComponent>("ImGui Small Button", entity, [](auto& button) {
        char buffer[512] = {0};
        strcpy_s(buffer, sizeof(buffer), button.name.c_str());

        ImGui::Columns(2);
        ImGui::TextUnformatted("Label");
        ImGui::NextColumn();
        if (ImGui::InputText("##Label", buffer, sizeof(buffer)))
        {
            button.name = std::string(buffer);
        }
        ImGui::NextColumn();

        static const char* states[] = {"Inactive", "Pressed", "Held", "Released"};
        ImGui::TextUnformatted("Current State:");
        ImGui::NextColumn();
        ImGui::Text("%s", states[static_cast<int>(button.state)]);
        ImGui::Columns();
    });

    DrawComponent<ImGuiInvisibleButtonComponent>(
      "ImGui Invisible Button", entity, [](auto& button) {
          char buffer[512] = {0};
          strcpy_s(buffer, sizeof(buffer), button.name.c_str());

          ImGui::Columns(2);

          static const char* states[] = {"Inactive", "Pressed", "Held", "Released"};
          ImGui::TextUnformatted("Current State:");
          ImGui::NextColumn();
          ImGui::Text("%s", states[static_cast<int>(button.state)]);
          ImGui::NextColumn();

          ImGui::TextUnformatted("Size:");
          ImGui::NextColumn();
          float size[2] = {button.size.x, button.size.y};
          ImGui::DragFloat2("##Size", size, 0.1f, 0.01f);
          button.size.x = std::max(size[0], 0.00001f);
          button.size.y = std::max(size[1], 0.00001f);
          ImGui::NextColumn();

          static const char* mouseButtons[] = {
            "Left Mouse Button", "Right Mouse Button", "Middle Mouse Button"};
          ImGui::TextUnformatted("Activated by:");
          ImGui::NextColumn();
          if (ImGui::BeginCombo("##ActivatedBy", mouseButtons[button.flag]))
          {
              for (int i = 0; i < 3; i++)
              {
                  if (ImGui::Selectable(mouseButtons[i], button.flag == i))
                  {
                      button.flag = (ImGuiButtonFlags)(1 << i);
                  }
              }
              ImGui::EndCombo();
          }
          ImGui::Columns();
      });

    DrawComponent<ImGuiArrowButtonComponent>("ImGui Arrow Button", entity, [](auto& button) {
        char buffer[512] = {0};
        strcpy_s(buffer, sizeof(buffer), button.name.c_str());

        ImGui::Columns(2);

        static const char* states[] = {"Inactive", "Pressed", "Held", "Released"};
        ImGui::TextUnformatted("Current State:");
        ImGui::NextColumn();
        ImGui::Text("%s", states[static_cast<int>(button.state)]);
        ImGui::NextColumn();

        static const char* directions[] = {"Left", "Right", "Up", "Down"};
        ImGui::TextUnformatted("Direction:");
        ImGui::NextColumn();
        if (ImGui::BeginCombo("##Direction", directions[button.direction]))
        {
            for (int i = 0; i < 4; i++)
            {
                if (ImGui::Selectable(directions[i], button.direction == i))
                {
                    button.direction = (ImGuiDir)i;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::Columns();
    });
}    // namespace Brigerad

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

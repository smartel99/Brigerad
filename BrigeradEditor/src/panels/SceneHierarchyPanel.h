/**
 * @file    SceneHierarchyPanel.h
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    10/10/2020 5:06:59 PM
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
#pragma once

/*********************************************************************************************************************/
// [SECTION] Includes
/*********************************************************************************************************************/
#include "Brigerad/Core/Core.h"
#include "Brigerad/Scene/Scene.h"
#include "Brigerad/Scene/Entity.h"

namespace Brigerad
{
/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/
class SceneHierarchyPanel
{
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene>& scene);

    void SetContext(const Ref<Scene>& context);
    // void SetSelected(Entity& entity);

    void OnImGuiRender();

private:
    void DrawEntityNode(Entity entity);

private:
    Ref<Scene> m_context;
    Entity     m_selectionContext;
};
}    // namespace Brigerad

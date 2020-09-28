/**
 * @file    Components
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    9/28/2020 1:39:48 PM
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
#include "glm/glm.hpp"

/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/

namespace Brigerad
{
struct TagComponent
{
    std::string tag;
    TagComponent()                    = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& t) : tag(t) {}
};

struct TransformComponent
{
    glm::mat4 transform {1.0f};

    TransformComponent()                          = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::mat4& tr) : transform(tr) {}

    operator glm::mat4&() { return transform; }
    operator const glm::mat4&() const { return transform; }
};

struct SpriteRendererComponent
{
    glm::vec4 color {1.0f, 1.0f, 1.0f, 1.0f};

    SpriteRendererComponent()                               = default;
    SpriteRendererComponent(const SpriteRendererComponent&) = default;
    SpriteRendererComponent(const glm::vec4& col) : color(col) {}
};


}    // namespace Brigerad

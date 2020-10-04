/**
 * @file    Camera
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    9/28/2020 4:17:35 PM
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
class Camera
{
public:
    Camera(const glm::mat4& projection) : m_projection(projection) {}
    ~Camera() = default;

    const glm::mat4& GetProjection() const { return m_projection; }

private:
    glm::mat4 m_projection;
};
}    // namespace Brigerad
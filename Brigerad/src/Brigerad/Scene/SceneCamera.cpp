/**
 * @file    SceneCamera.cpp
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    10/9/2020 12:22:29 PM
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
#include "SceneCamera.h"

#include "glm/gtc/matrix_transform.hpp"

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
SceneCamera::SceneCamera()
{
    RecalculateOrthographicProjection();
}

void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
{
    m_orthographicSize = size;
    m_orthographicNear = nearClip;
    m_orthographicFar  = farClip;
    RecalculateOrthographicProjection();
}

void SceneCamera::SetViewportSize(uint32_t w, uint32_t h)
{
    m_aspectRatio = (float)w / (float)h;
    RecalculateOrthographicProjection();
}

void SceneCamera::SetProjectionType(ProjectionType type)
{
    m_projectionType = type;
    if (m_projectionType == ProjectionType::Ortographic)
    {
        RecalculateOrthographicProjection();
    }
    else
    {
        RecalculatePerspectiveProjection();
    }
}

void SceneCamera::RecalculateOrthographicProjection()
{
    float orthoLeft   = -m_orthographicSize * m_aspectRatio * 0.5f;
    float orthoRight  = m_orthographicSize * m_aspectRatio * 0.5f;
    float orthoBottom = m_orthographicSize * 0.5f;
    float orthoTop    = -m_orthographicSize * 0.5f;

    m_projection = glm::ortho(
      orthoLeft, orthoRight, orthoBottom, orthoTop, m_orthographicNear, m_orthographicFar);
}

void SceneCamera::RecalculatePerspectiveProjection()
{
    m_projection = glm::perspective(
      glm::radians(m_perspectiveFov), m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
}

/*********************************************************************************************************************/
// [SECTION] Private Method Definitions
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Private Function Declarations
/*********************************************************************************************************************/

}    // namespace Brigerad

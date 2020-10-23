/**
 * @file    SceneCamera
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    10/9/2020 11:55:51 AM
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
#include "Brigerad/Renderer/Camera.h"

/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/

namespace Brigerad
{
/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/
class SceneCamera : public Camera
{
public:
    enum class ProjectionType
    {
        Perspective = 0,
        Ortographic = 1
    };

public:
    SceneCamera();
    virtual ~SceneCamera() = default;

    void SetOrthographic(float size, float nearClip, float farClip);
    void SetViewportSize(uint32_t w, uint32_t h);

    ProjectionType GetProjectionType() const { return m_projectionType; }
    void           SetProjectionType(ProjectionType type);

    float GetOrthographicSize() const { return m_orthographicSize; }
    void  SetOrthographicSize(float size)
    {
        m_orthographicSize = size;
        RecalculateOrthographicProjection();
    }

    float GetOrtographicNearClip() const { return m_orthographicNear; }
    void  SetOrtographicNearClip(float clip) { m_orthographicNear = clip; }
    float GetOrtographicFarClip() const { return m_orthographicFar; }
    void  SetOrtographicFarClip(float clip) { m_orthographicFar = clip; }

    float GetPerspectiveNearClip() const { return m_perspectiveNear; }
    void  SetPerspectiveNearClip(float clip) { m_perspectiveNear = clip; }
    float GetPerspectiveFarClip() const { return m_perspectiveFar; }
    void  SetPerspectiveFarClip(float clip) { m_perspectiveFar = clip; }

    float GetPerspectiveFov() const { return m_perspectiveFov; }
    void  SetPerspectiveFov(float fov) { m_perspectiveFov = fov; }

    void RecalculateOrthographicProjection();
    void RecalculatePerspectiveProjection();

private:
    ProjectionType m_projectionType = ProjectionType::Ortographic;

    float m_orthographicSize = 10.0f;
    float m_orthographicNear = -1.0f;
    float m_orthographicFar  = 1.0f;

    float m_perspectiveFov  = 45.0f;
    float m_perspectiveNear = 0.0001f;
    float m_perspectiveFar  = 1000.0f;

    float m_aspectRatio = 0;
};

}    // namespace Brigerad

/**
 * @file   E:\dev\Brigerad\Sandbox\src\CameraController.cpp
 * @author Samuel Martel
 * @date   2020/04/04
 *
 * @brief  Source for the CameraController module.
 */

#include "CameraController.h"

CameraController::CameraController(float left, float right, float bottom, float top) :
    Brigerad::OrthographicCamera(left, right, bottom, top)
{
}

void CameraController::OnUpdate(Brigerad::Timestep ts)
{
    HandleKeys(ts);
    glm::vec3 acc = glm::vec3(m_acceleration.x * ts,
                              m_acceleration.y * ts,
                              m_acceleration.z * ts);
    glm::vec3 newPos = Brigerad::OrthographicCamera::GetPosition() + acc;
    Brigerad::OrthographicCamera::SetPosition(newPos);
}

void CameraController::HandleKeys(Brigerad::Timestep ts)
{
    for (int key : m_keys)
    {
        float ds = (MOVE_SPEED * ts);
        float dr = (ROTATION_SPEED * ts);
        // Down.
        if (key == BR_KEY_S)
        {
            // If not at max acceleration:
            if (m_acceleration.y <= MAX_POS_SPEED)
            {
                m_acceleration.y += ds;
            }
        }
        // Up.
        if (key == BR_KEY_W)
        {
            // If not at max acceleration:
            if (m_acceleration.y >= MAX_NEG_SPEED)
            {
                m_acceleration.y -= ds;
            }
        }

        // Right.
        if (key == BR_KEY_D)
        {
            // If not at max acceleration:
            if (m_acceleration.x >= MAX_NEG_SPEED)
            {
                m_acceleration.x -= ds;
            }
        }
        // Left.
        if (key == BR_KEY_A)
        {
            // If not at max acceleration:
            if (m_acceleration.x <= MAX_POS_SPEED)
            {
                m_acceleration.x += ds;
            }
        }

        using namespace Brigerad;
        // Tilt CCW.
        if (key == BR_KEY_Q)
        {
            // Tilt CCW by 5 degrees.
            OrthographicCamera::SetRotation((OrthographicCamera::GetRotation() + dr) >= 360 ?
                                            0 : OrthographicCamera::GetRotation() + dr);
        }
        // Tilt CW.
        if (key == BR_KEY_E)
        {
            // Tilt CW by 5 degrees.
            OrthographicCamera::SetRotation((OrthographicCamera::GetRotation() - dr) <= 0 ?
                                            360 : OrthographicCamera::GetRotation() - dr);
        }
    }
}




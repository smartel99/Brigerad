/**
 * @file   E:\dev\Brigerad\Sandbox\src\CameraController.cpp
 * @author Samuel Martel
 * @date   2020/04/04
 *
 * @brief  Source for the CameraController module.
 */

#include "CameraController.h"

#define BIND_EVENT_FN(x) std::bind(&CameraController::x, this, std::placeholders::_1)

CameraController::CameraController(float left, float right, float bottom, float top) :
    Brigerad::OrthographicCamera(left, right, bottom, top)
{
}

void CameraController::OnUpdate()
{
    HandleKeys();
    glm::vec3 newPos = GetPosition() + m_acceleration;
    SetPosition(newPos);
}

void CameraController::HandleEvent(Brigerad::Event& event)
{
    Brigerad::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Brigerad::KeyPressedEvent>(BIND_EVENT_FN(HandleKeyPressedEvent));
    dispatcher.Dispatch<Brigerad::KeyReleasedEvent>(BIND_EVENT_FN(HandleKeyReleasedEvent));
}


bool CameraController::HandleKeyPressedEvent(Brigerad::KeyPressedEvent& keyEvent)
{
    int key = keyEvent.GetKeyCode();
    AddKeyToList(key);

    return false;
}


bool CameraController::HandleKeyReleasedEvent(Brigerad::KeyReleasedEvent& keyEvent)
{
    int key = keyEvent.GetKeyCode();
    // Up/Down.
    if (key == BR_KEY_W || key == BR_KEY_S)
    {
        m_acceleration.y = 0;
    }
    // Left.
    if (key == BR_KEY_A || key == BR_KEY_D)
    {
        m_acceleration.x = 0;
    }

    RemoveKeyFromList(key);

    return false;
}


void CameraController::HandleKeys()
{
    for (int key : m_keys)
    {
        // Down.
        if (key == BR_KEY_S)
        {
            // If not at max acceleration:
            if (m_acceleration.y <= 0.10f)
            {
                m_acceleration.y += 0.005f;
            }
        }
        // Up.
        if (key == BR_KEY_W)
        {
            // If not at max acceleration:
            if (m_acceleration.y >= -0.10f)
            {
                m_acceleration.y -= 0.005f;
            }
        }

        // Right.
        if (key == BR_KEY_D)
        {
            // If not at max acceleration:
            if (m_acceleration.x >= -0.10f)
            {
                m_acceleration.x -= 0.005f;
            }
        }
        // Left.
        if (key == BR_KEY_A)
        {
            // If not at max acceleration:
            if (m_acceleration.x <= 0.10f)
            {
                m_acceleration.x += 0.005f;
            }
        }

        // Tilt CCW.
        if (key == BR_KEY_Q)
        {
            // Tilt CCW by 5 degrees.
            SetRotation((GetRotation() + 5) >= 360 ? 0 : GetRotation() + 5);
        }
        // Tilt CW.
        if (key == BR_KEY_E)
        {
            // Tilt CW by 5 degrees.
            SetRotation((GetRotation() - 5) <= 0 ? 360 : GetRotation() - 5);
        }
    }
}


void CameraController::AddKeyToList(int key)
{
    for (int k : m_keys)
    {
        if (k == key)
        {
            return;
        }
    }

    m_keys.emplace_back(key);
}


void CameraController::RemoveKeyFromList(int key)
{
    for (auto k = m_keys.begin(); k != m_keys.end(); k++)
    {
        if (*k == key)
        {
            m_keys.erase(k);
            return;
        }
    }
}


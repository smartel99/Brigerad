/**
 * @file   E:\dev\Brigerad\Sandbox\src\CameraController.cpp
 * @author Samuel Martel
 * @date   2020/04/04
 *
 * @brief  Source for the CameraController module.
 */

#include "CameraController.h"

#define BIND_EVENT_FN(x) std::bind(&CameraController::x, this, std::placeholders::_1)
constexpr float MOVE_SPEED = 2.0f;
constexpr float ROTATION_SPEED = 180.0f;


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
    glm::vec3 newPos = GetPosition() + acc;
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
    if (keyEvent.GetRepeatCount() == 0)
    {
        int key = keyEvent.GetKeyCode();
        AddKeyToList(key);
    }

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
            if (m_acceleration.y <= 2.0f)
            {
                m_acceleration.y += ds;
            }
        }
        // Up.
        if (key == BR_KEY_W)
        {
            // If not at max acceleration:
            if (m_acceleration.y >= -2.0f)
            {
                m_acceleration.y -= ds;
            }
        }

        // Right.
        if (key == BR_KEY_D)
        {
            // If not at max acceleration:
            if (m_acceleration.x >= -2.0f)
            {
                m_acceleration.x -= ds;
            }
        }
        // Left.
        if (key == BR_KEY_A)
        {
            // If not at max acceleration:
            if (m_acceleration.x <= 2.0f)
            {
                m_acceleration.x += ds;
            }
        }

        // Tilt CCW.
        if (key == BR_KEY_Q)
        {
            // Tilt CCW by 5 degrees.
            SetRotation((GetRotation() + dr) >= 360 ?
                        0 : GetRotation() + dr);
        }
        // Tilt CW.
        if (key == BR_KEY_E)
        {
            // Tilt CW by 5 degrees.
            SetRotation((GetRotation() - dr) <= 0 ?
                        360 : GetRotation() - dr);
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


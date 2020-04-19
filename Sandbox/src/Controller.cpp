/**
 * @file   E:\dev\Brigerad\Sandbox\src\Controller.cpp
 * @author Samuel Martel
 * @date   2020/04/06
 *
 * @brief  Source for the Controller module.
 */

#include "Controller.h"

#define BIND_EVENT_FN(x) std::bind(&Controller::x, this, std::placeholders::_1)


void Controller::OnUpdate(Brigerad::Timestep ts)
{
    HandleKeys(ts);
    glm::vec3 acc = glm::vec3(m_acceleration.x * ts,
                              m_acceleration.y * ts,
                              m_acceleration.z * ts);
    m_position += acc;
}

void Controller::HandleEvent(Brigerad::Event& event)
{
    Brigerad::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Brigerad::KeyPressedEvent>(BIND_EVENT_FN(HandleKeyPressedEvent));
    dispatcher.Dispatch<Brigerad::KeyReleasedEvent>(BIND_EVENT_FN(HandleKeyReleasedEvent));
}


bool Controller::HandleKeyPressedEvent(Brigerad::KeyPressedEvent& keyEvent)
{
    if (keyEvent.GetRepeatCount() == 0)
    {
        int key = keyEvent.GetKeyCode();
        AddKeyToList(key);
    }

    return false;
}


bool Controller::HandleKeyReleasedEvent(Brigerad::KeyReleasedEvent& keyEvent)
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


void Controller::HandleKeys(Brigerad::Timestep ts)
{
    for (int key : m_keys)
    {
        float ds = (MOVE_SPEED * ts);
        float dr = (ROTATION_SPEED * ts);
        // Down.
        if (key == BR_KEY_W)
        {
            // If not at max acceleration:
            if (m_acceleration.y <= MAX_POS_SPEED)
            {
                m_acceleration.y += ds;
            }
        }
        // Up.
        if (key == BR_KEY_S)
        {
            // If not at max acceleration:
            if (m_acceleration.y >= MAX_NEG_SPEED)
            {
                m_acceleration.y -= ds;
            }
        }

        // Right.
        if (key == BR_KEY_A)
        {
            // If not at max acceleration:
            if (m_acceleration.x >= MAX_NEG_SPEED)
            {
                m_acceleration.x -= ds;
            }
        }
        // Left.
        if (key == BR_KEY_D)
        {
            // If not at max acceleration:
            if (m_acceleration.x <= MAX_POS_SPEED)
            {
                m_acceleration.x += ds;
            }
        }

        // Tilt CCW.
        if (key == BR_KEY_Q)
        {
            // Tilt CCW by 5 degrees.
            m_rotation = ((m_rotation + dr) >= 360 ?
                          0 : m_rotation + dr);
        }
        // Tilt CW.
        if (key == BR_KEY_E)
        {
            // Tilt CW by 5 degrees.
            m_rotation = ((m_rotation - dr) <= 0 ?
                          360 : m_rotation - dr);
        }
    }
}


void Controller::AddKeyToList(int key)
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


void Controller::RemoveKeyFromList(int key)
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


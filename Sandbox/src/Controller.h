#pragma once


#include "Brigerad/Events/Event.h"
#include "Brigerad/Events/KeyEvents.h"
#include "Brigerad/KeyCodes.h"
#include "Brigerad/Core/Timestep.h"

#include <glm/glm.hpp>

constexpr float MOVE_SPEED = 2.0f;
constexpr float ROTATION_SPEED = 180.0f;

class Controller
{
public:

    virtual void OnUpdate(Brigerad::Timestep ts);
    virtual void HandleEvent(Brigerad::Event& event);

    const glm::vec3& GetMovement() const
    {
        return m_acceleration;
    }

    virtual const  glm::vec3 GetPosition() const
    {
        return m_position;
    }
    virtual void SetPosition(const glm::vec3& position)
    {
        m_position = position;
    }

    virtual float GetRotation() const
    {
        return m_rotation;
    }
    virtual void SetRotation(float rotation)
    {
        m_rotation = rotation;
    }

protected:
    glm::vec3 m_acceleration = glm::vec3(0.0f);
    glm::vec3 m_position = glm::vec3(0.0f);
    float m_rotation = 0.0f;
    std::vector<int> m_keys = {};

protected:
    bool HandleKeyPressedEvent(Brigerad::KeyPressedEvent& keyEvent);
    bool HandleKeyReleasedEvent(Brigerad::KeyReleasedEvent& keyEvent);
    virtual void HandleKeys(Brigerad::Timestep ts);
    void AddKeyToList(int key);
    void RemoveKeyFromList(int key);
};
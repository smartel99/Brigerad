#pragma once

//#include "Brigerad.h"
#include "Brigerad/Renderer/OrthographicCamera.h"
#include "Brigerad/Events/Event.h"
#include "Brigerad/Events/KeyEvents.h"
#include "Brigerad/KeyCodes.h"


class CameraController : public Brigerad::OrthographicCamera
{
public:
    CameraController(float left, float right, float bottom, float top);

    void OnUpdate();
    void HandleEvent(Brigerad::Event& event);
private:
    glm::vec3 m_acceleration = glm::vec3(0.0f);
    std::vector<int> m_keys = {};

private:
    bool HandleKeyPressedEvent(Brigerad::KeyPressedEvent& keyEvent);
    bool HandleKeyReleasedEvent(Brigerad::KeyReleasedEvent& keyEvent);
    void HandleKeys();
    void AddKeyToList(int key);
    void RemoveKeyFromList(int key);
};
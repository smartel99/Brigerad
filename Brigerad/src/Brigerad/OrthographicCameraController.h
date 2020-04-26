#pragma once
#include "Brigerad/Renderer/OrthographicCamera.h"
#include "Brigerad/Core/Timestep.h"
#include "Brigerad/Events/ApplicationEvent.h"
#include "Brigerad/Events/MouseEvent.h"

namespace Brigerad
{
class OrthographicCameraController
{
public:
    OrthographicCameraController(float aspectRatio, bool rotation = false);

    void OnUpdate(Timestep ts);
    void OnEvent(Event &e);

    OrthographicCamera &GetCamera() { return m_camera; }
    const OrthographicCamera &GetCamera() const { return m_camera; }

    inline void SetZoomLevel(float level) { m_zoomLevel = level; }
    inline float GetZoomLevel() const { return m_zoomLevel; }

private:
    bool OnMouseScrolled(MouseScrolledEvent &e);
    bool OnWindowResized(WindowResizeEvent &e);

private:
    float m_aspectRatio;
    float m_zoomLevel = 1.0f;
    OrthographicCamera m_camera;

    bool m_allowRotation = false;
    glm::vec3 m_cameraPosition = {0.0f, 0.0f, 0.0f};
    float m_cameraRotation = 0.0f;

    float m_cameraTranslationSpeed = 10.0f;
    float m_cameraRotationSpeed = 180.0f;
};

} // namespace Brigerad
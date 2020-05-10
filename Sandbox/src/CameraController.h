#pragma once

//#include "Brigerad.h"
#include "Brigerad/Renderer/OrthographicCamera.h"
#include "Controller.h"


class CameraController : public Brigerad::OrthographicCamera, public Controller
{
public:
    CameraController(float left, float right, float bottom, float top);

    virtual void OnUpdate(Brigerad::Timestep ts) override;
    using Brigerad::OrthographicCamera::GetPosition;
    using Brigerad::OrthographicCamera::GetRotation;

private:
    virtual void HandleKeys(Brigerad::Timestep ts) override;
};

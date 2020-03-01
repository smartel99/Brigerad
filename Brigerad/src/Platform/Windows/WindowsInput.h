#pragma once

#include "Brigerad/Input.h"

namespace Brigerad
{
class WindowsInput : public Input
{
protected:
    virtual bool IsKeyPressedImpl(int keycode) override;
    virtual bool IsMouseButtonPressedImpl(int button) override;
    virtual float GetMouseXImpl() override;
    virtual float GetMouseYImpl() override;
    virtual std::pair<float, float> GetMousePosImpl() override;

};
}
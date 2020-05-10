#pragma once
#if defined(BR_PLATFORM_WINDOWS)
#include "Brigerad/Core/Input.h"

namespace Brigerad
{
class WindowsInput : public Input
{
protected:
    virtual bool IsKeyPressedImpl(KeyCode keycode) override;
    virtual bool IsMouseButtonPressedImpl(MouseCode button) override;
    virtual float GetMouseXImpl() override;
    virtual float GetMouseYImpl() override;
    virtual std::pair<float, float> GetMousePosImpl() override;
};
} // namespace Brigerad
#endif

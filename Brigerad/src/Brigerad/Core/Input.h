#pragma once

#include "Brigerad/Core/Core.h"
#include "Brigerad/Core/KeyCodes.h"
#include "Brigerad/Core/MouseButtonCodes.h"

namespace Brigerad
{
class BRIGERAD_API Input
{
public:
    // Public API, static interface.
    inline static bool IsKeyPressed(KeyCode keycode)
    {
        return s_instance->IsKeyPressedImpl(keycode);
    }

    inline static bool IsMouseButtonPressed(MouseCode button)
    {
        return s_instance->IsMouseButtonPressedImpl(button);
    }

    inline static float GetMouseX()
    {
        return s_instance->GetMouseXImpl();
    }

    inline static float GetMouseY()
    {
        return s_instance->GetMouseYImpl();
    }

    inline static std::pair<float, float> GetMousePos()
    {
        return s_instance->GetMousePosImpl();
    }

    // Actual implementation, platform-defined.
protected:
    virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;
    virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
    virtual float GetMouseXImpl() = 0;
    virtual float GetMouseYImpl() = 0;
    virtual std::pair<float, float> GetMousePosImpl() = 0;

private:
    static Input *s_instance;
};
} // namespace Brigerad

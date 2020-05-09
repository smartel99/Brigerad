#pragma once

#include "Brigerad/Renderer/GraphicsContext.h"
#include <windef.h>


namespace Brigerad
{
class DirectXContext : public GraphicsContext
{
public:
    DirectXContext(HWND windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;
private:
    HWND m_windowHandle;
};
}

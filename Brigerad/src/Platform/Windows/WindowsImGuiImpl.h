#pragma once

#include "Brigerad/Renderer/ImGuiImpl.h"

namespace Brigerad
{
class WindowsImGuiImpl : public ImGuiImpl
{
    WindowsImGuiImpl();
    virtual ~WindowsImGuiImpl() override = default;

    virtual bool InitWindowManager(void* window, bool installCallbacks) override;
    virtual bool InitRenderer(const char* version) override;

    virtual void ShutdownWindowManager() override;
    virtual void ShutdownRenderer() override;

    virtual void NewWindowManagerFrame() override;
    virtual void NewWindowRendererFrame() override;

    virtual void RenderDrawData(ImDrawData * data) override;
};
}


#pragma once

#if defined(BR_PLATFORM_WINDOWS)
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx11.h"
#elif defined(BR_PLATFORM_LINUX)
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"
#else
#error Unsupported Platform
#endif

#include "Brigerad/Core/Core.h"
#include "imgui.h"

namespace Brigerad
{
class ImGuiImpl
{
public:
    virtual ~ImGuiImpl() = default;

    virtual bool InitWindowManager(void* window, bool installCallbacks) = 0;
    virtual bool InitRenderer(const char* version) = 0;

    virtual void ShutdownWindowManager() = 0;
    virtual void ShutdownRenderer() = 0;

    virtual void NewWindowManagerFrame() = 0;
    virtual void NewWindowRendererFrame() = 0;

    virtual void RenderDrawData(ImDrawData* data) = 0;

    static Ref<ImGuiImpl> Create();
};
}

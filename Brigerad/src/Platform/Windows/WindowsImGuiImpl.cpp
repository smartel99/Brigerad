/**
 * @file   D:\dev\Brigerad\Brigerad\src\Platform\Windows\WindowsImGuiImpl.cpp
 * @author Samuel Martel
 * @date   2020/05/08
 * 
 * @brief  Source for the WindowsImGuiImpl module.
 */
#include "brpch.h" 
#include "WindowsImGuiImpl.h"

namespace Brigerad
{
WindowsImGuiImpl::WindowsImGuiImpl()
{
}


bool WindowsImGuiImpl::InitWindowManager(void* window, bool installCallbacks)
{
    ImGui_ImplWin32_Init(window);
}


bool WindowsImGuiImpl::InitRenderer(const char* version)
{

}


void WindowsImGuiImpl::ShutdownWindowManager()
{

}


void WindowsImGuiImpl::ShutdownRenderer()
{

}


void WindowsImGuiImpl::NewWindowManagerFrame()
{

}


void WindowsImGuiImpl::NewWindowRendererFrame()
{

}


void WindowsImGuiImpl::RenderDrawData(ImDrawData* data)
{

}


Ref<ImGuiImpl> ImGuiImpl::Create()
{

}


}


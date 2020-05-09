/**
 * @file   D:\dev\Brigerad\Brigerad\src\Platform\DirectX11\DirectXContext.cpp
 * @author Samuel Martel
 * @date   2020/05/08
 * 
 * @brief  Source for the DirectXContext module.
 */
#include "brpch.h" 
#include "DirectXContext.h"

namespace Brigerad
{


DirectXContext::DirectXContext(HWND windowHandle)
    : m_windowHandle(windowHandle)
{
    BR_CORE_ASSERT(windowHandle, "Window handle is null!");
}


void DirectXContext::Init()
{

}


void DirectXContext::SwapBuffers()
{

}


}


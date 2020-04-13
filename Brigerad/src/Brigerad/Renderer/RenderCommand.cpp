/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\Renderer\RenderCommand.cpp
 * @author Samuel Martel
 * @date   2020/03/31
 *
 * @brief  Source for the RenderCommand module.
 */
#include "brpch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Brigerad
{
RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI;
}


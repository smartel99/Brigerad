/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\Renderer\VertexArray.cpp
 * @author Samuel Martel
 * @date   2020/03/31
 *
 * @brief  Source for the VertexArray module.
 */

#include "brpch.h"
#include "VertexArray.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Brigerad
{
VertexArray* VertexArray::Create()
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            BR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLVertexArray();
        default:
            BR_CORE_ASSERT(false, "Invalid RendererAPI!");
            return nullptr;
    }
}
}


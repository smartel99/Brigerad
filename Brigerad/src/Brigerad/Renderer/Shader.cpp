/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\Renderer\Shader.cpp
 * @author Samuel Martel
 * @date   2020/03/07
 *
 * @brief  Source for the Shader module.
 */

#include "brpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Brigerad
{

Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            BR_CORE_ASSERT(false, "RendererAPI::None is not a valid rendering API!");
            return nullptr;
        case RendererAPI::OpenGL:
            return new OpenGLShader(vertexSrc, fragmentSrc);
        default:
            BR_CORE_ASSERT(false, "Invalid rendering API!");
            return nullptr;
    }
}

}


/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\Renderer\Texture.cpp
 * @author Samuel Martel
 * @date   2020/04/11
 *
 * @brief  Source for the Texture module.
 */
#include "brpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Brigerad
{
Ref<Texture2D> Texture2D::Create(const std::string& path)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            BR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLTexture2D>(path);
        default:
            BR_CORE_ASSERT(false, "Invalid RendererAPI!");
            return nullptr;
    }
}


}


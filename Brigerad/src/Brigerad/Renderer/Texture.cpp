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
Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, uint8_t channels)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            BR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLTexture2D>(width, height, channels);
        default:
            BR_CORE_ASSERT(false, "Invalid RendererAPI!");
            return nullptr;
    }
}

Ref<Texture2D> Texture2D::Create(const std::string& path)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            BR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLTexture2D>(path);
        default:
            BR_CORE_ASSERT(false, "Invalid RendererAPI!");
            return nullptr;
    }
}

Ref<TextureCube> TextureCube::Create(uint32_t width, uint32_t height, uint8_t channels)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            BR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLTextureCube>(width, height, channels);
        default:
            BR_CORE_ASSERT(false, "Invalid RendererAPI!");
            return nullptr;
    }
}

Ref<TextureCube> TextureCube::Create(const std::string& path)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            BR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLTextureCube>(path);
        default:
            BR_CORE_ASSERT(false, "Invalid RendererAPI!");
            return nullptr;
    }
}

}    // namespace Brigerad

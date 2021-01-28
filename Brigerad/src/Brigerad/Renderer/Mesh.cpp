#include "brpch.h"
#include "Mesh.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLMesh.h"

namespace Brigerad
{
Ref<Mesh> Mesh::Create(const std::string& fileName)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            BR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLMesh>(fileName);

        default:
            BR_CORE_ASSERT(false, "Invalid RendererAPI!");
            return nullptr;
    }
}
}    // namespace Brigerad

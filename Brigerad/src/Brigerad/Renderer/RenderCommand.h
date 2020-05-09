#pragma once

#include "RendererAPI.h"

namespace Brigerad
{
class RenderCommand
{
    public:
    inline static void SetClearColor(const glm::vec4& color)
    {
        s_rendererAPI->SetClearColor(color);
    }

    inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        s_rendererAPI->SetViewport(x, y, width, height);
    }

    inline static void Clear() { s_rendererAPI->Clear(); }

    inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
    {
        BR_PROFILE_FUNCTION();
        s_rendererAPI->DrawIndexed(vertexArray);
    }

    inline static void Init() { s_rendererAPI->Init(); }

    private:
    static RendererAPI* s_rendererAPI;
};
}  // namespace Brigerad
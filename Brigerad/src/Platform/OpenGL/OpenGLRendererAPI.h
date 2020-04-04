#pragma once
#include "Brigerad/Renderer/RendererAPI.h"

namespace Brigerad
{
class OpenGLRendererAPI : public RendererAPI
{
    virtual void SetClearColor(const glm::vec4& color) override;
    virtual void Clear() override;

    virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
};

}


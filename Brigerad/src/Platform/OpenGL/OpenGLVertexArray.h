#pragma once

#include "Brigerad/Renderer/VertexArray.h"


namespace Brigerad
{
class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override;
    virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffers() const override;
private:
    std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
    std::shared_ptr<IndexBuffer> m_indexBuffer;

    uint32_t m_rendererId;
};
}
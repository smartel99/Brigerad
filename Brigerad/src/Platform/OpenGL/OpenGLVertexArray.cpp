/**
 * @file   E:\dev\Brigerad\Brigerad\src\Platform\OpenGL\OpenGLVertexArray.cpp
 * @author Samuel Martel
 * @date   2020/03/31
 *
 * @brief  Source for the OpenGLVertexArray module.
 */

#include "brpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Brigerad
{

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:
            return GL_FLOAT;
        case ShaderDataType::Float2:
            return GL_FLOAT;
        case ShaderDataType::Float3:
            return GL_FLOAT;
        case ShaderDataType::Float4:
            return GL_FLOAT;
        case ShaderDataType::Mat3:
            return GL_FLOAT;
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
            return GL_INT;
        case ShaderDataType::Int2:
            return GL_INT;
        case ShaderDataType::Int3:
            return GL_INT;
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        case ShaderDataType::None:
        default:
            BR_CORE_ASSERT(false, "Invalid ShaderDataType!");
            return 0;
    }
}

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &m_rendererId);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_rendererId);
}

void OpenGLVertexArray::Bind() const
{
    glBindVertexArray(m_rendererId);
}


void OpenGLVertexArray::Unbind() const
{
    glBindVertexArray(0);
}


void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    BR_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout");

    glBindVertexArray(m_rendererId);
    vertexBuffer->Bind();

    uint32_t index = 0;
    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              element.GetComponentCount(),
                              ShaderDataTypeToOpenGLBaseType(element.type),
                              element.normalized ? GL_TRUE : GL_FALSE,
                              layout.GetStride(),
                              (const void*)element.offset);
        index++;
    }

    m_vertexBuffers.emplace_back(vertexBuffer);
}


void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    glBindVertexArray(m_rendererId);
    indexBuffer->Bind();

    m_indexBuffer = indexBuffer;
}

const std::vector<std::shared_ptr<VertexBuffer>>& OpenGLVertexArray::GetVertexBuffers() const
{
    return m_vertexBuffers;
}

const std::shared_ptr<IndexBuffer>& OpenGLVertexArray::GetIndexBuffers() const
{
    return m_indexBuffer;
}

}


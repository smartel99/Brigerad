#include "brpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Brigerad
{
/************************************************************************/
/* VertexBuffer                                                         */
/************************************************************************/

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
{
    BR_PROFILE_FUNCTION();

    glCreateBuffers(1, &m_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    BR_PROFILE_FUNCTION();

    glDeleteBuffers(1, &m_rendererID);
}


void OpenGLVertexBuffer::Bind() const
{
    BR_PROFILE_FUNCTION();

    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void OpenGLVertexBuffer::Unbind() const
{
    BR_PROFILE_FUNCTION();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/************************************************************************/
/* IndexBuffer                                                          */
/************************************************************************/

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
    : m_count(count)
{
    BR_PROFILE_FUNCTION();

    glCreateBuffers(1, &m_rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    BR_PROFILE_FUNCTION();

    glDeleteBuffers(1, &m_rendererID);
}


void OpenGLIndexBuffer::Bind() const
{
    BR_PROFILE_FUNCTION();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void OpenGLIndexBuffer::Unbind() const
{
    BR_PROFILE_FUNCTION();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}

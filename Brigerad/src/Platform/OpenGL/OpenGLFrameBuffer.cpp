/**
 * @file   D:\dev\Brigerad\Brigerad\src\Platform\OpenGL\OpenGLFrameBuffer.cpp
 * @author Samuel Martel
 * @date   2020/06/13
 *
 * @brief  Source for the OpenGLFrameBuffer module.
 */
#include "brpch.h"

#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Brigerad
{


OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
    :m_spec(spec), m_rendererID(0)
{
    Invalidate();
}


OpenGLFramebuffer::~OpenGLFramebuffer()
{
    glDeleteFramebuffers(1, &m_rendererID);
    glDeleteTextures(1, &m_colorAttachment);
    glDeleteTextures(1, &m_depthAttachment);
}

void OpenGLFramebuffer::Invalidate()
{
    // If we already have a frame buffer:
    if (m_rendererID)
    {
        // Delete it.
        glDeleteFramebuffers(1, &m_rendererID);
        glDeleteTextures(1, &m_colorAttachment);
        glDeleteTextures(1, &m_depthAttachment);
    }

    glCreateFramebuffers(1, &m_rendererID);

    glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_colorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 m_spec.Width, m_spec.Height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);

    glCreateTextures(GL_TEXTURE_2D, 1, &m_depthAttachment);
    glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_spec.Width, m_spec.Height);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment, 0);

    BR_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                   "Framebuffer is incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void OpenGLFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
    glViewport(0, 0, m_spec.Width, m_spec.Height);
}


void OpenGLFramebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
{
    m_spec.Width = width;
    m_spec.Height = height;
    Invalidate();
}


}


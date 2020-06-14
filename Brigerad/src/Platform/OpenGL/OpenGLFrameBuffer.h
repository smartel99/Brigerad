#pragma once

#include "Brigerad/Renderer/FrameBuffer.h"

namespace Brigerad
{
class OpenGLFramebuffer : public Framebuffer
{
public:
    OpenGLFramebuffer(const FramebufferSpecification& spec);
    virtual ~OpenGLFramebuffer();

    void Invalidate();

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual uint32_t GetColorAttachmentRenderID() const override
    {
        return m_colorAttachment;
    }


    virtual const FramebufferSpecification& GetSpecification() const override
    {
        return m_spec;
    }


private:
    uint32_t m_rendererID;
    uint32_t m_colorAttachment, m_depthAttachment;
    FramebufferSpecification m_spec;
};
}

#pragma once

#include "Brigerad/Renderer/Shader.h"

namespace Brigerad
{
class OpenGLShader : public Shader
{
public:
    OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~OpenGLShader() override;

    void Bind() const override;
    void Unbind() const override;

private:
    uint32_t m_rendererID;
};
}
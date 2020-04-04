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
    void UploadUniformMat4(const std::string& name, const glm::mat4& uniform) override;
private:
    uint32_t m_rendererID;
};
}
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
    const uint32_t GetId() const
    {
        return m_rendererID;
    }

    void UploadUniformInt(const std::string& name, int value);

    void UploadUniformFloat(const std::string& name, float values);
    void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
    void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
    void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

    void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
    void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
private:
    uint32_t m_rendererID;
};
}
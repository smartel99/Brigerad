#pragma once

#include "Brigerad/Renderer/Shader.h"
#include "OpenGLShaderUniform.h"

#include <unordered_map>

// TODO: REMOVE:
typedef unsigned int GLenum;

namespace Brigerad
{
class OpenGLShader : public Shader
{
public:
    OpenGLShader() = default;
    OpenGLShader(const std::string& filepath);
    static Ref<OpenGLShader> CreateFromString(const std::string& source);

    virtual void Reload() override;
    virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;

    virtual void Bind() const override;

    virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override;

    virtual void SetVSMaterialUniformBuffer(Buffer buffer) override;
    virtual void SetPSMaterialUniformBuffer(Buffer buffer) override;
    virtual void SetInt(const std::string& name, int value) override;
    virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;

    virtual void SetFloat(const std::string& name, float value) override;
    virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
    virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
    virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;

    virtual void SetMat3(const std::string& name, const glm::mat3& value) override;
    virtual void SetMat4(const std::string& name, const glm::mat4& value) override;


    virtual const std::string& GetName() const { return m_name; }

private:
    void Load(const std::string& source);

    std::string                             ReadShaderFromFile(const std::string& filepath) const;
    std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
    void                                    Parse();
    void          ParseUniform(const std::string& statement, ShaderDomain domain);
    void          ParseUniformStruct(const std::string& block, ShaderDomain domain);
    ShaderStruct* FindStruct(const std::string& name);

    int32_t GetUniformLocation(const std::string& name) const;

    void          ResolveUniforms();
    void          ValidateUniforms();
    void          CompileAndUploadShader();
    static GLenum ShaderTypeFromString(const std::string& type);

    void ResolveAndSetUniforms(const Scope<OpenGLShaderUniformBufferDeclaration>& decl,
                               Buffer                                             buffer);
    void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
    void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
    void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field,
                                   byte*                                 data,
                                   int32_t                               offset);

    void UploadUniformInt(const std::string& name, int value);
    void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
    void UploadUniformFloat(const std::string& name, float values);
    void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
    void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
    void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
    void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
    void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

    void UploadUniformInt(uint32_t location, int value);
    void UploadUniformIntArray(uint32_t location, int* values, uint32_t count);
    void UploadUniformFloat(uint32_t location, float values);
    void UploadUniformFloat2(uint32_t location, const glm::vec2& values);
    void UploadUniformFloat3(uint32_t location, const glm::vec3& values);
    void UploadUniformFloat4(uint32_t location, const glm::vec4& values);
    void UploadUniformMat3(uint32_t location, const glm::mat3& matrix);
    void UploadUniformMat4(uint32_t location, const glm::mat4& matrix);
    void UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count);

    void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform,
                             byte*                           buffer,
                             uint32_t                        offset);

    inline const ShaderUniformBufferList& GetVSRendererUniforms() const override
    {
        return m_vSRendererUniformBuffers;
    }
    inline const ShaderUniformBufferList& GetPSRendererUniforms() const override
    {
        return m_pSRendererUniformBuffers;
    }
    inline const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override
    {
        return *m_vSMaterialUniformBuffer;
    }
    inline const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override
    {
        return *m_pSMaterialUniformBuffer;
    }
    inline const ShaderResourceList& GetResources() const override { return m_resources; }

private:
    uint32_t m_rendererID = 0;
    bool     m_loaded     = false;

    std::string                             m_name, m_assetPath;
    std::unordered_map<GLenum, std::string> m_shaderSource;

    std::vector<ShaderReloadedCallback> m_shaderReloadedCallbacks;

    ShaderUniformBufferList                     m_vSRendererUniformBuffers;
    ShaderUniformBufferList                     m_pSRendererUniformBuffers;
    Scope<OpenGLShaderUniformBufferDeclaration> m_vSMaterialUniformBuffer;
    Scope<OpenGLShaderUniformBufferDeclaration> m_pSMaterialUniformBuffer;
    ShaderResourceList                          m_resources;
    ShaderStructList                            m_structs;
};
}    // namespace Brigerad
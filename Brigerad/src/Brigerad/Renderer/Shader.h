#pragma once

#include "Brigerad/Core/Core.h"
#include "Brigerad/Core/Buffer.h"

//#include "Brigerad/Renderer/Renderer.h"
#include "Brigerad/Renderer/ShaderUniform.h"

#include <string>
#include <glm/glm.hpp>

namespace Brigerad
{
struct ShaderUniform
{
};

struct ShaderUniformCollection
{
};

enum class UniformType
{
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Matrix3x3,
    Matrix4x4,
    Int32,
    Uint32
};

struct UniformDecl
{
    UniformType    Type;
    std::ptrdiff_t Offset;
    std::string    Name;
};

struct UniformBuffer
{
    // #TODO: This currently represents a byte buffer that has been
    // packed with uniforms. This was primarily created for OpenGL,
    // and needs to be revisited for other rendering APIs. Furthermore,
    // this currently does not assume any alignment. This also has
    // nothing to do with GL uniform buffers, this is simply a CPU-side
    // buffer abstraction.
    byte*                    Buffer;
    std::vector<UniformDecl> Uniforms;
};

struct UniformBufferBase
{

    virtual const byte*        GetBuffer() const       = 0;
    virtual const UniformDecl* GetUniforms() const     = 0;
    virtual unsigned int       GetUniformCount() const = 0;
};

template<unsigned int N, unsigned int U>
struct UniformBufferDeclaration : public UniformBufferBase
{
    byte           Buffer[N];
    UniformDecl    Uniforms[U];
    std::ptrdiff_t Cursor = 0;
    int            Index  = 0;

    virtual const byte*        GetBuffer() const override { return Buffer; }
    virtual const UniformDecl* GetUniforms() const override { return Uniforms; }
    virtual unsigned int       GetUniformCount() const { return U; }

    template<typename T>
    void Push(const std::string& name, const T& data)
    {
    }

    template<>
    void Push(const std::string& name, const float& data)
    {
        Uniforms[Index++] = {UniformType::Float, Cursor, name};
        memcpy(Buffer + Cursor, &data, sizeof(float));
        Cursor += sizeof(float);
    }

    template<>
    void Push(const std::string& name, const glm::vec3& data)
    {
        Uniforms[Index++] = {UniformType::Float3, Cursor, name};
        memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(float));
        Cursor += sizeof(glm::vec3);
    }

    template<>
    void Push(const std::string& name, const glm::vec4& data)
    {
        Uniforms[Index++] = {UniformType::Float4, Cursor, name};
        memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(float));
        Cursor += sizeof(glm::vec4);
    }

    template<>
    void Push(const std::string& name, const glm::mat4& data)
    {
        Uniforms[Index++] = {UniformType::Matrix4x4, Cursor, name};
        memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::mat4));
        Cursor += sizeof(glm::mat4);
    }
};

class Shader
{
public:
    using ShaderReloadedCallback = std::function<void()>;

    virtual ~Shader() = default;

    virtual void Reload() = 0;

    virtual void Bind() const = 0;

    virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) = 0;

    virtual void SetInt(const std::string& name, int value)                        = 0;
    virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;

    virtual void SetFloat(const std::string& name, float value)             = 0;
    virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
    virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
    virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;

    virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
    virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

    virtual const std::string& GetName() const = 0;

    static Ref<Shader> Create(const std::string& filePath);
    static Ref<Shader> CreateFromString(const std::string& source);

    virtual void SetVSMaterialUniformBuffer(Buffer buffer) = 0;
    virtual void SetPSMaterialUniformBuffer(Buffer buffer) = 0;

    virtual const ShaderUniformBufferList&        GetVSRendererUniforms() const      = 0;
    virtual const ShaderUniformBufferList&        GetPSRendererUniforms() const      = 0;
    virtual const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const = 0;
    virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const = 0;

    virtual const ShaderResourceList& GetResources() const = 0;

    virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

    // Temporary, before we have an asset manager.
    static std::vector<Ref<Shader>> s_AllShaders;
};

class ShaderLibrary
{
public:
    void        Add(const Ref<Shader>& shader);
    Ref<Shader> Load(const std::string& filePath);
    Ref<Shader> Load(const std::string& name, const std::string& filePath);

    Ref<Shader> Get(const std::string& name);

    bool Exists(const std::string& name) const;

private:
    std::unordered_map<std::string, Ref<Shader>> m_shaders;
};
}    // namespace Brigerad
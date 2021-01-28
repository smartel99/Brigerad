#pragma once

#include "Brigerad/Renderer/ShaderUniform.h"

namespace Brigerad
{
class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration
{
public:
public:
    enum class Type
    {
        NONE,
        TEXTURE2D
    };

private:
    friend class OpenGLShader;

private:
    std::string m_name;
    uint32_t    m_register = 0;
    uint32_t    m_count;
    Type        m_type;

public:
    OpenGLShaderResourceDeclaration(Type type, const std::string& name, uint32_t count);

    inline virtual const std::string& GetName() const override { return m_name; }
    inline virtual uint32_t           GetRegister() const override { return m_register; }
    inline virtual uint32_t           GetCount() const override { return m_count; }

    inline Type GetType() const { return m_type; }

public:
    static Type        StringToType(const std::string& type);
    static std::string TypeToString(Type type);
};

class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration
{
private:
    friend class OpenGLShader;
    friend class OpenGLShaderUniformBufferDeclaration;

public:
    enum class Type
    {
        NONE,
        FLOAT32,
        VEC2,
        VEC3,
        VEC4,
        MAT3,
        MAT4,
        INT32,
        STRUCT
    };

private:
    std::string  m_name;
    uint32_t     m_size;
    uint32_t     m_count;
    uint32_t     m_offset;
    ShaderDomain m_domain;

    Type            m_type;
    ShaderStruct*   m_struct;
    mutable int32_t m_location;

public:
    OpenGLShaderUniformDeclaration(ShaderDomain       domain,
                                   Type               type,
                                   const std::string& name,
                                   uint32_t           count = 1);
    OpenGLShaderUniformDeclaration(ShaderDomain       domain,
                                   ShaderStruct*      uniformStruct,
                                   const std::string& name,
                                   uint32_t           count = 1);

    inline virtual const std::string& GetName() const override { return m_name; }
    inline virtual uint32_t           GetSize() const override { return m_size; }
    inline virtual uint32_t           GetCount() const override { return m_count; }
    inline virtual uint32_t           GetOffset() const override { return m_offset; }
    inline uint32_t                   GetAbsoluteOffset() const
    {
        return m_struct ? m_struct->GetOffset() + m_offset : m_offset;
    }
    inline ShaderDomain GetDomain() const { return m_domain; }

    int32_t                    GetLocation() const { return m_location; }
    inline Type                GetType() const { return m_type; }
    inline bool                IsArray() const { return m_count > 1; }
    inline const ShaderStruct& GetShaderUniformStruct() const
    {
        BR_CORE_ASSERT(m_struct != nullptr, "Structure is null!");
        return *m_struct;
    }

protected:
    void SetOffset(uint32_t offset) override;

public:
    std::uint32_t      SizeOfUniformType(Type type);
    static Type        StringToType(const std::string& type);
    static std::string TypeToString(Type type);
};

struct GLShaderUniformField
{
    OpenGLShaderUniformDeclaration::Type type;
    std::string                          name;
    uint32_t                             count;
    mutable uint32_t                     size;
    mutable int32_t                      location;
};

class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
{
private:
    friend class Shader;

private:
    std::string                  m_name;
    ShaderUniformDeclarationList m_uniforms;
    uint32_t                     m_register;
    uint32_t                     m_size;
    ShaderDomain                 m_domain;

public:
    OpenGLShaderUniformBufferDeclaration(const std::string& name, ShaderDomain domain);

    void PushUniform(OpenGLShaderUniformDeclaration* uniform);

    inline virtual const std::string& GetName() const override { return m_name; }
    inline virtual uint32_t           GetRegister() const override { return m_register; }
    inline virtual uint32_t           GetSize() const override { return m_size; }
    ShaderDomain                      GetDomain() const { return m_domain; }
    inline virtual const ShaderUniformDeclarationList& GetUniformDeclarations() const override
    {
        return m_uniforms;
    }

    ShaderUniformDeclaration* FindUniform(const std::string& name);
};
}    // namespace Brigerad

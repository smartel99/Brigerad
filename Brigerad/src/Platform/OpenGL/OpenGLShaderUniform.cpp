#include "brpch.h"
#include "OpenGLShaderUniform.h"

namespace Brigerad
{

OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(Type               type,
                                                                 const std::string& name,
                                                                 uint32_t           count)
: m_type(type), m_name(name), m_count(count)
{
}

Brigerad::OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::StringToType(
  const std::string& type)
{
    if (type == "sampler2D")
    {
        return Type::TEXTURE2D;
    }
    else
    {
        return Type::NONE;
    }
}

std::string OpenGLShaderResourceDeclaration::TypeToString(Type type)
{
    switch (type)
    {
        case Type::TEXTURE2D:
            return "sampler2D";
        default:
            return "Invalid Type";
    }
}

OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain       domain,
                                                               Type               type,
                                                               const std::string& name,
                                                               uint32_t           count /*= 1*/)
: m_type(type), m_struct(nullptr), m_domain(domain), m_name(name), m_count(count)
{
    m_size = SizeOfUniformType(type) * count;
}

OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain       domain,
                                                               ShaderStruct*      uniformStruct,
                                                               const std::string& name,
                                                               uint32_t           count /*= 1*/)
: m_struct(uniformStruct), m_type(OpenGLShaderUniformDeclaration::Type::STRUCT), m_domain(domain)
{
    m_name  = name;
    m_count = count;
    m_size  = m_struct->GetSize() * count;
}

void OpenGLShaderUniformDeclaration::SetOffset(uint32_t offset)
{
    if (m_type == OpenGLShaderUniformDeclaration::Type::STRUCT)
    {
        m_struct->SetOffset(offset);
    }

    m_offset = offset;
}

std::uint32_t OpenGLShaderUniformDeclaration::SizeOfUniformType(Type type)
{
    switch (type)
    {
        case Type::INT32:
            return 4;
        case Type::FLOAT32:
            return 4;
        case Type::VEC2:
            return 4 * 2;
        case Type::VEC3:
            return 4 * 3;
        case Type::VEC4:
            return 4 * 4;
        case Type::MAT3:
            return 4 * 3 * 3;
        case Type::MAT4:
            return 4 * 4 * 4;
        default:
            return 0;
    }
}

Brigerad::OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(
  const std::string& type)
{
    if (type == "int32")
    {
        return Type::INT32;
    }
    else if (type == "float")
    {
        return Type::FLOAT32;
    }
    else if (type == "vec2")
    {
        return Type::VEC2;
    }
    else if (type == "vec3")
    {
        return Type::VEC3;
    }
    else if (type == "vec4")
    {
        return Type::VEC4;
    }
    else if (type == "mat3")
    {
        return Type::MAT3;
    }
    else if (type == "mat4")
    {
        return Type::MAT4;
    }
    else
    {
        return Type::NONE;
    }
}

std::string OpenGLShaderUniformDeclaration::TypeToString(Type type)
{
    switch (type)
    {
        case OpenGLShaderUniformDeclaration::Type::INT32:
            return "int32";
        case OpenGLShaderUniformDeclaration::Type::FLOAT32:
            return "float";
        case OpenGLShaderUniformDeclaration::Type::VEC2:
            return "vec2";
        case OpenGLShaderUniformDeclaration::Type::VEC3:
            return "vec3";
        case OpenGLShaderUniformDeclaration::Type::VEC4:
            return "vec4";
        case OpenGLShaderUniformDeclaration::Type::MAT3:
            return "mat3";
        case OpenGLShaderUniformDeclaration::Type::MAT4:
            return "mat4";
        default:
            return "Invalid Type";
    }
}

OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(const std::string& name,
                                                                           ShaderDomain domain)
: m_name(name), m_domain(domain), m_size(0), m_register(0)
{
}

void OpenGLShaderUniformBufferDeclaration::PushUniform(OpenGLShaderUniformDeclaration* uniform)
{
    uint32_t offset = 0;
    if (m_uniforms.empty() == false)
    {
        OpenGLShaderUniformDeclaration* previous =
          (OpenGLShaderUniformDeclaration*)m_uniforms.back();
        offset = previous->m_offset + previous->m_size;
    }
    uniform->SetOffset(offset);
    m_size += uniform->GetSize();
    m_uniforms.push_back(uniform);
}

Brigerad::ShaderUniformDeclaration* OpenGLShaderUniformBufferDeclaration::FindUniform(
  const std::string& name)
{
    for (ShaderUniformDeclaration* uniform : m_uniforms)
    {
        if (uniform->GetName() == name)
        {
            return uniform;
        }
    }
    return nullptr;
}

}    // namespace Brigerad

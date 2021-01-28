#pragma once


#include "Brigerad/Core/Core.h"
#include "Brigerad/Core/Log.h"

#include <string>
#include <vector>

namespace Brigerad
{
enum class ShaderDomain
{
    None   = 0,
    Vertex = 0,
    Pixel  = 1
};

class ShaderUniformDeclaration
{
private:
    friend class Shader;
    friend class OpenGLShader;
    friend class ShaderStruct;

public:
    virtual const std::string& GetName() const   = 0;
    virtual uint32_t           GetSize() const   = 0;
    virtual uint32_t           GetCount() const  = 0;
    virtual uint32_t           GetOffset() const = 0;
    virtual ShaderDomain       GetDomain() const = 0;

protected:
    virtual void SetOffset(uint32_t offset) = 0;
};

using ShaderUniformDeclarationList = std::vector<ShaderUniformDeclaration*>;

class ShaderUniformBufferDeclaration
{
public:
    virtual const std::string&                  GetName() const                = 0;
    virtual uint32_t                            GetRegister() const            = 0;
    virtual uint32_t                            GetSize() const                = 0;
    virtual const ShaderUniformDeclarationList& GetUniformDeclarations() const = 0;

    virtual ShaderUniformDeclaration* FindUniform(const std::string& name) = 0;
};

using ShaderUniformBufferList = std::vector<ShaderUniformBufferDeclaration*>;

class ShaderStruct
{
private:
    friend class Shader;

private:
    std::string                  m_name;
    ShaderUniformDeclarationList m_fields;
    uint32_t                     m_size;
    uint32_t                     m_offset;

public:
    ShaderStruct(const std::string& name) : m_name(name), m_size(0), m_offset(0) {}

    void AddField(ShaderUniformDeclaration* field)
    {
        m_size += field->GetSize();
        uint32_t offset = 0;
        if (m_fields.empty() == false)
        {
            ShaderUniformDeclaration* previous = m_fields.back();
            offset                             = previous->GetOffset() + previous->GetSize();
        }
        field->SetOffset(offset);
        m_fields.push_back(field);
    }

    inline void SetOffset(uint32_t offset) { m_offset = offset; }

    inline const std::string&                  GetName() const { return m_name; }
    inline uint32_t                            GetSize() const { return m_size; }
    inline uint32_t                            GetOffset() const { return m_offset; }
    inline const ShaderUniformDeclarationList& GetFields() const { return m_fields; }
};

using ShaderStructList = std::vector<ShaderStruct*>;

class ShaderResourceDeclaration
{
public:
    virtual const std::string& GetName() const     = 0;
    virtual uint32_t           GetRegister() const = 0;
    virtual uint32_t           GetCount() const    = 0;
};

using ShaderResourceList = std::vector<ShaderResourceDeclaration*>;

}    // namespace Brigerad
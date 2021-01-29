#pragma once

#include "Brigerad/Core/Core.h"

#include "Brigerad/Renderer/Shader.h"
#include "Brigerad/Renderer/Texture.h"

#include <unordered_set>


namespace Brigerad
{
class Material
{
    friend class MaterialInstance;

public:
    Material(const Ref<Shader>& shader);
    virtual ~Material();

    void Bind() const;

    template<typename T>
    void Set(const std::string& name, const T& value)
    {
        auto decl = FindUniformDeclaration(name);
        BR_CORE_ASSERT(decl, "Could not find uniform with name '{0}'", name.c_str());

        auto& buffer = GetUniformBufferTarget(decl);
        buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

        for (auto mi : m_materialInstances)
        {
            mi->OnMaterialValueUpdated(decl);
        }
    }

    void Set(const std::string& name, const Ref<Texture>& texture)
    {
        auto     decl = FindResourceDeclaration(name);
        uint32_t slot = decl->GetRegister();
        if (m_textures.size() <= slot)
        {
            m_textures.resize((size_t)slot + 1);
        }
        m_textures[slot] = texture;
    }

    void Set(const std::string& name, const Ref<Texture2D>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

public:
    static Ref<Material> Create(const Ref<Shader>& shader);

private:
    void AllocateStorage();
    void OnShaderReloaded();
    void BindTextures() const;

    ShaderUniformDeclaration*  FindUniformDeclaration(const std::string& name);
    ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name);
    Buffer&                    GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);

private:
    Ref<Shader>                           m_shader;
    std::unordered_set<MaterialInstance*> m_materialInstances;

    Buffer                    m_vSUniformStorageBuffer;
    Buffer                    m_pSUniformStorageBuffer;
    std::vector<Ref<Texture>> m_textures;

    int32_t m_renderFlags = 0;
};

class MaterialInstance
{
    friend class Material;

public:
    MaterialInstance(const Ref<Material>& material);
    virtual ~MaterialInstance();

    template<typename T>
    void Set(const std::string& name, const T& value)
    {
        auto decl = m_material->FindUniformDeclaration(name);
        BR_CORE_ASSERT(decl, "Could not find uniform with name '{}'", name.c_str());
        if (decl == nullptr)
        {
            return;
        }
        auto& buffer = GetUniformBufferTarget(decl);
        buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

        m_overriddenValues.insert(name);
    }

    void Set(const std::string& name, const Ref<Texture>& texture)
    {
        auto decl = m_material->FindResourceDeclaration(name);
        BR_CORE_ASSERT(decl, "Could not find resource with name '{}'", name.c_str());
        if (decl == nullptr)
        {
            return;
        }

        uint32_t slot = decl->GetRegister();
        if (m_textures.size() <= slot)
        {
            m_textures.resize((size_t)slot + 1);
        }
        m_textures[slot] = texture;
    }

    void Set(const std::string& name, const Ref<Texture2D>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

    void Set(const std::string& name, const Ref<TextureCube>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

    void Bind() const;

public:
    static Ref<MaterialInstance> Create(const Ref<Material>& material);

private:
    void    AllocateStorage();
    void    OnShaderReloaded();
    Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
    void    OnMaterialValueUpdated(ShaderUniformDeclaration* decl);

private:
public:
    Ref<Material> m_material;

    Buffer                    m_vSUniformStorageBuffer;
    Buffer                    m_pSUniformStorageBuffer;
    std::vector<Ref<Texture>> m_textures;

    std::unordered_set<std::string> m_overriddenValues;
};

}    // namespace Brigerad
#include "brpch.h"
#include "Material.h"

#include "Brigerad/Renderer/Shader.h"

namespace Brigerad
{
/************************************************************************/
/* Material                                                             */
/************************************************************************/
Material::Material(const Ref<Shader>& shader) : m_shader(shader)
{
    m_shader->AddShaderReloadedCallback(std::bind(&Material::OnShaderReloaded, this));
    AllocateStorage();
}

Material::~Material()
{
}

void Material::Bind() const
{
    m_shader->Bind();

    if (m_vSUniformStorageBuffer)
    {
        m_shader->SetVSMaterialUniformBuffer(m_vSUniformStorageBuffer);
    }
    if (m_pSUniformStorageBuffer)
    {
        m_shader->SetPSMaterialUniformBuffer(m_pSUniformStorageBuffer);
    }

    BindTextures();
}

Brigerad::Ref<Brigerad::Material> Material::Create(const Ref<Shader>& shader)
{
    return CreateRef<Material>(shader);
}

void Material::AllocateStorage()
{
    const auto& vsBuffer = m_shader->GetVSMaterialUniformBuffer();
    m_vSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
    m_vSUniformStorageBuffer.ZeroInitialize();

    const auto& psBuffer = m_shader->GetPSMaterialUniformBuffer();
    m_pSUniformStorageBuffer.Allocate(psBuffer.GetSize());
    m_pSUniformStorageBuffer.ZeroInitialize();
}

void Material::OnShaderReloaded()
{
    AllocateStorage();

    for (auto mi : m_materialInstances)
    {
        mi->OnShaderReloaded();
    }
}

void Material::BindTextures() const
{
    for (size_t i = 0; i < m_textures.size(); i++)
    {
        auto& texture = m_textures[i];
        // Texture vector may contain empty slots.
        if (texture != nullptr)
        {
            texture->Bind((uint32_t)i);
        }
    }
}

ShaderUniformDeclaration* Material::FindUniformDeclaration(const std::string& name)
{
    if (m_vSUniformStorageBuffer)
    {
        auto& declarations = m_shader->GetVSMaterialUniformBuffer().GetUniformDeclarations();
        for (ShaderUniformDeclaration* uniform : declarations)
        {
            if (uniform->GetName() == name)
            {
                return uniform;
            }
        }
    }

    if (m_pSUniformStorageBuffer)
    {
        auto& declarations = m_shader->GetPSMaterialUniformBuffer().GetUniformDeclarations();
        for (ShaderUniformDeclaration* uniform : declarations)
        {
            if (uniform->GetName() == name)
            {
                return uniform;
            }
        }
    }

    // Uniform not found.
    return nullptr;
}

ShaderResourceDeclaration* Material::FindResourceDeclaration(const std::string& name)
{
    auto& resources = m_shader->GetResources();
    for (ShaderResourceDeclaration* resource : resources)
    {
        if (resource->GetName() == name)
        {
            return resource;
        }
    }

    // Resource not found.
    return nullptr;
}

Buffer& Material::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
{
    switch (uniformDeclaration->GetDomain())
    {
        case ShaderDomain::Vertex:
            return m_vSUniformStorageBuffer;
        case ShaderDomain::Pixel:
            return m_pSUniformStorageBuffer;
        default:
            BR_CORE_ASSERT(
              false,
              "Invalid uniform declaration domain! Material does not support this shader type.");
            return m_vSUniformStorageBuffer;
    }
}

/************************************************************************/
/* MaterialInstance                                                     */
/************************************************************************/

MaterialInstance::MaterialInstance(const Ref<Material>& material) : m_material(material)
{
    m_material->m_materialInstances.insert(this);
    AllocateStorage();
}

MaterialInstance::~MaterialInstance()
{
    m_material->m_materialInstances.erase(this);
}

void MaterialInstance::Bind() const
{
    if (m_vSUniformStorageBuffer)
    {
        m_material->m_shader->SetVSMaterialUniformBuffer(m_vSUniformStorageBuffer);
    }
    if (m_pSUniformStorageBuffer)
    {
        m_material->m_shader->SetPSMaterialUniformBuffer(m_pSUniformStorageBuffer);
    }

    m_material->BindTextures();
    for (size_t i = 0; i < m_textures.size(); i++)
    {
        auto& texture = m_textures[i];
        if (texture != nullptr)
        {
            texture->Bind((uint32_t)i);
        }
    }
}

Brigerad::Ref<Brigerad::MaterialInstance> MaterialInstance::Create(const Ref<Material>& material)
{
    return CreateRef<MaterialInstance>(material);
}

void MaterialInstance::AllocateStorage()
{
    const auto& vsBuffer = m_material->m_shader->GetVSMaterialUniformBuffer();
    m_vSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
    memcpy(
      m_vSUniformStorageBuffer.Data, m_material->m_vSUniformStorageBuffer.Data, vsBuffer.GetSize());

    const auto& psBuffer = m_material->m_shader->GetPSMaterialUniformBuffer();
    m_pSUniformStorageBuffer.Allocate(psBuffer.GetSize());
    memcpy(
      m_pSUniformStorageBuffer.Data, m_material->m_pSUniformStorageBuffer.Data, psBuffer.GetSize());
}

void MaterialInstance::OnShaderReloaded()
{
    AllocateStorage();
    m_overriddenValues.clear();
}

Buffer& MaterialInstance::GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration)
{
    switch (uniformDeclaration->GetDomain())
    {
        case ShaderDomain::Vertex:
            return m_vSUniformStorageBuffer;
        case ShaderDomain::Pixel:
            return m_pSUniformStorageBuffer;
        default:
            BR_CORE_ASSERT(
              false,
              "Invalid uniform declaration domain! Material does not support this shader type.");
            return m_vSUniformStorageBuffer;
    }
}

void MaterialInstance::OnMaterialValueUpdated(ShaderUniformDeclaration* decl)
{
    if (m_overriddenValues.find(decl->GetName()) == m_overriddenValues.end())
    {
        auto& buffer         = GetUniformBufferTarget(decl);
        auto& materialBuffer = m_material->GetUniformBufferTarget(decl);
        buffer.Write(materialBuffer.Data + decl->GetOffset(), decl->GetSize(), decl->GetOffset());
    }
}

}    // namespace Brigerad

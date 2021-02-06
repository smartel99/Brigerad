/**
 * @file    Material.cpp
 * @brief   Source for the Material system.
 * @author  Samuel Martel
 * @date    06/02/2021
 */
#include "brpch.h"
#include "Material.h"

#include "Brigerad/Renderer/Shader.h"

namespace Brigerad
{
/************************************************************************/
/* Material                                                             */
/************************************************************************/

/*----------------------------------------------------------------------*/
/* Static Public Method Definitions                                     */
/*----------------------------------------------------------------------*/
Brigerad::Ref<Brigerad::Material> Material::Create(const Ref<Shader>& shader)
{
    return CreateRef<Material>(shader);
}

/*----------------------------------------------------------------------*/
/* Public Method Definitions                                            */
/*----------------------------------------------------------------------*/

Material::Material(const Ref<Shader>& shader) : m_shader(shader)
{
    // Set shader callback for when the shader is reloaded.
    m_shader->AddShaderReloadedCallback(std::bind(&Material::OnShaderReloaded, this));
    // Reset uniform buffers.
    AllocateStorage();
}

Material::~Material()
{
    // Automatically cleans uniform buffers.
}

void Material::Bind() const
{
    // Tell the GPU to use this shaders.
    m_shader->Bind();

    // If the vertex shader has any uniforms:
    if (m_vSUniformStorageBuffer)
    {
        // Upload them to the GPU.
        m_shader->SetVSMaterialUniformBuffer(m_vSUniformStorageBuffer);
    }
    // If the fragment/pixel shader has any uniforms:
    if (m_pSUniformStorageBuffer)
    {
        // Upload them to the GPU.
        m_shader->SetPSMaterialUniformBuffer(m_pSUniformStorageBuffer);
    }

    // Tell the GPU to load all of the shader's textures.
    BindTextures();
}


/*----------------------------------------------------------------------*/
/* Private Method Definitions                                           */
/*----------------------------------------------------------------------*/
/**
 * @brief   Allocate and zero-initialize memory for the vertex shader's uniforms
 *          and the fragment/pixel shader's uniform.
 * @param   None
 * @return  None
 */
void Material::AllocateStorage()
{
    // Get the uniform data buffers from the shader and allocate memory for it.
    const auto& vsBuffer = m_shader->GetVSMaterialUniformBuffer();
    m_vSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
    m_vSUniformStorageBuffer.ZeroInitialize();

    const auto& psBuffer = m_shader->GetPSMaterialUniformBuffer();
    m_pSUniformStorageBuffer.Allocate(psBuffer.GetSize());
    m_pSUniformStorageBuffer.ZeroInitialize();
}

/**
 * @brief   Clears and reallocates all uniform buffers, then notify all of the
 *          material instances.
 * @param   None
 * @return  None
 */
void Material::OnShaderReloaded()
{
    // The shader has been reloaded,
    // we must assume that all uniforms we had are now invalid.
    // We thus recreate all of our buffers.
    AllocateStorage();

    // Tell all of our instances that the shader was reloaded.
    for (auto mi : m_materialInstances)
    {
        mi->OnShaderReloaded();
    }
}

/**
 * @brief   Loads all of the Material's textures into the GPU.
 * @param   None
 * @return  None
 */
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

/**
 * @brief   Find a uniform's declaration, which contain the information critical
 *          to the material: Type, stride, size, and offset in the buffer.
 * @param   name The name of the uniform to find
 * @retval  Pointer to the uniform's declaration, if it is found
 * @retval  nullptr if the uniform isn't found
 */
ShaderUniformDeclaration* Material::FindUniformDeclaration(const std::string& name)
{
    // If we have uniforms in the fragment/pixel shader:
    if (m_pSUniformStorageBuffer)
    {
        // Get a list of all the uniforms.
        auto& declarations = m_shader->GetPSMaterialUniformBuffer().GetUniformDeclarations();
        // Find the desired uniform.
        for (ShaderUniformDeclaration* uniform : declarations)
        {
            if (uniform->GetName() == name)
            {
                return uniform;
            }
        }
    }

    // If we have uniforms in the vertex shader:
    if (m_vSUniformStorageBuffer)
    {
        // Get a list of all the uniforms.
        auto& declarations = m_shader->GetVSMaterialUniformBuffer().GetUniformDeclarations();
        // Find the desired uniform.
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

/**
 * @brief   Find a resource's declaration, which contain the information critical
 *          to the material: register and number of resources linked to it.
 * @param   name The name of the resource to find
 * @retval  Pointer to the resource's declaration, if it is found
 * @retval  nullptr if the resource isn't found
 */
ShaderResourceDeclaration* Material::FindResourceDeclaration(const std::string& name)
{
    // Get a list of all resources.
    auto& resources = m_shader->GetResources();
    // Find the resource we want.
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

/**
 * @brief   Get a reference to the uniform's data buffer based on its domain.
 * @param   uniformDeclaration A pointer to the uniform's declaration
 * @return  A reference to the uniform data buffer for that uniform's domain
 *
 * @note    If the domain of the uniformDeclaration is not valid, a reference to
 *          the vertex shader's uniform storage buffer will still be returned.
 */
Buffer& Material::GetUniformBufferTarget(const ShaderUniformDeclaration* uniformDeclaration)
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

/*----------------------------------------------------------------------*/
/* Static Public Method Definitions                                     */
/*----------------------------------------------------------------------*/
Brigerad::Ref<Brigerad::MaterialInstance> MaterialInstance::Create(const Ref<Material>& material)
{
    return CreateRef<MaterialInstance>(material);
}

/*----------------------------------------------------------------------*/
/* Public Method Definitions                                            */
/*----------------------------------------------------------------------*/
MaterialInstance::MaterialInstance(const Ref<Material>& material) : m_material(material)
{
    // Tell the parent Material that we now exist.
    m_material->m_materialInstances.insert(this);
    // Initialize our data buffers.
    AllocateStorage();
}

MaterialInstance::~MaterialInstance()
{
    // Tell the parent Material that we now don't exist anymore.
    m_material->m_materialInstances.erase(this);
}

void MaterialInstance::Bind() const
{
    // Upload uniform data to the GPU.
    if (m_vSUniformStorageBuffer)
    {
        m_material->m_shader->SetVSMaterialUniformBuffer(m_vSUniformStorageBuffer);
    }
    if (m_pSUniformStorageBuffer)
    {
        m_material->m_shader->SetPSMaterialUniformBuffer(m_pSUniformStorageBuffer);
    }

    // #TODO Check if we actually need to call that function
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

/*----------------------------------------------------------------------*/
/* Private Method Definitions                                           */
/*----------------------------------------------------------------------*/
/**
 * @brief   Allocate and zero-initialize memory for both shader's uniform data.
 * @param   None
 * @return  None
 */
void MaterialInstance::AllocateStorage()
{
    // Copy the data of the parent material into the instance's local buffer.
    const auto& vsBuffer = m_material->m_shader->GetVSMaterialUniformBuffer();
    m_vSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
    memcpy(
      m_vSUniformStorageBuffer.Data, m_material->m_vSUniformStorageBuffer.Data, vsBuffer.GetSize());

    const auto& psBuffer = m_material->m_shader->GetPSMaterialUniformBuffer();
    m_pSUniformStorageBuffer.Allocate(psBuffer.GetSize());
    memcpy(
      m_pSUniformStorageBuffer.Data, m_material->m_pSUniformStorageBuffer.Data, psBuffer.GetSize());
}

/**
 * @brief   Regenerates the uniform buffers and clears the overridden values upon shader reloading.
 * @param   None
 * @return  None
 */
void MaterialInstance::OnShaderReloaded()
{
    AllocateStorage();
    m_overriddenValues.clear();
}

/**
 * @brief   Get a reference to the uniform's data buffer based on its domain.
 * @param   uniformDeclaration A pointer to the uniform's declaration
 * @return  A reference to the uniform data buffer for that uniform's domain
 *
 * @note    If the domain of the uniformDeclaration is not valid, a reference to
 *          the vertex shader's uniform storage buffer will still be returned.
 */
Buffer& MaterialInstance::GetUniformBufferTarget(const ShaderUniformDeclaration* uniformDeclaration)
{
    BR_CORE_ASSERT(uniformDeclaration != nullptr, "");
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

/**
 * @brief   Reloads the uniform if it isn't overridden.
 * @param   decl The uniform to reload
 * @return  None
 */
void MaterialInstance::OnMaterialValueUpdated(const ShaderUniformDeclaration* decl)
{
    BR_CORE_ASSERT(decl != nullptr, "");
    if (m_overriddenValues.find(decl->GetName()) == m_overriddenValues.end())
    {
        auto& buffer         = GetUniformBufferTarget(decl);
        auto& materialBuffer = m_material->GetUniformBufferTarget(decl);
        buffer.Write(materialBuffer.Data + decl->GetOffset(), decl->GetSize(), decl->GetOffset());
    }
}

}    // namespace Brigerad

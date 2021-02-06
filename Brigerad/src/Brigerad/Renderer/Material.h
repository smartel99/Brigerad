/**
 * @file    Material.h
 * @brief   Header for the Material system.
 * @author  Samuel Martel
 * @date    06/02/2021
 */

#pragma once

#include "Brigerad/Core/Core.h"

#include "Brigerad/Renderer/Shader.h"
#include "Brigerad/Renderer/Texture.h"

#include <unordered_set>


namespace Brigerad
{
/**
 * @brief   Platform-agnostic Material class.
 *
 * Materials are a set of uniforms used in a Shader (both vertex and fragment),
 * usually used to set the visual property of the mesh being rendered.
 */
class Material
{
    friend class MaterialInstance;

public:
    /**
     * @brief   Creates a Material from an existing Shader.
     *          The Shader's code is parsed to find all uniforms, allocating
     *          memory in the Material to store the data for each uniform.
     * @param   shader A reference to the Shader to create the Material from.
     */
    Material(const Ref<Shader>& shader);
    virtual ~Material();

    /**
     * @brief   Binds all the information related to the shader:
     *          - The shader itself
     *          - Every uniforms for that shader
     *          - Every textures for that shader
     * @param   None
     * @return  None
     */
    void Bind() const;

    /**
     * @brief   Set an uniform's value.
     * @param   name The name of the uniform to set
     * @param   value The value to assign to the uniform
     *
     * @note    This function asserts false if the uniform doesn't exist.
     */
    template<typename T>
    void Set(const std::string& name, const T& value)
    {
        // Find the declaration of the uniform we want to set the value.
        auto decl = FindUniformDeclaration(name);
        // Make sure that uniform exists.
        BR_CORE_ASSERT(decl, "Could not find uniform with name '{0}'", name.c_str());

        // Write the data into the uniform's local buffer.
        auto& buffer = GetUniformBufferTarget(decl);
        buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

        // Notice every MaterialInstance of the new value.
        for (auto mi : m_materialInstances)
        {
            mi->OnMaterialValueUpdated(decl);
        }
    }

    /**
     * @brief   Set a Texture in the Shader.
     * @param   name The name of the Texture to set
     * @param   texture A reference to the Texture to use.
     *
     * @note    This function asserts false if the resource doesn't exist.
     */
    void Set(const std::string& name, const Ref<Texture>& texture)
    {
        // Find the texture's declaration.
        auto decl = FindResourceDeclaration(name);
        // Make sure that resource exists.
        BR_CORE_ASSERT(decl, "Could not find resource with name '{0}'", name.c_str());

        // Get the slot in which the texture should be put.
        uint32_t slot = decl->GetRegister();
        // If the material doesn't have that much space:
        if (m_textures.size() <= slot)
        {
            // Resize the texture buffer to make that new texture fit.
            m_textures.resize((size_t)slot + 1);
        }
        // Put the texture in the buffer.
        m_textures[slot] = texture;
    }

    /**
     * @brief   Set a Texture2D in the Shader.
     * @param   name The name of the Texture2D to set
     * @param   texture A reference to the Texture2D to use.
     *
     * @note    This function asserts false if the resource doesn't exist.
     */
    void Set(const std::string& name, const Ref<Texture2D>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

    /**
     * @brief   Set a TextureCube in the Shader.
     * @param   name The name of the TextureCube to set
     * @param   texture A reference to the TextureCube to use.
     *
     * @note    This function asserts false if the resource doesn't exist.
     */
    void Set(const std::string& name, const Ref<TextureCube>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

public:
    /**
     * @brief   Creates a Material from an existing Shader.
     *          The Shader's code is parsed to find all uniforms, allocating
     *          memory in the Material to store the data for each uniform.
     * @param   shader A reference to the Shader to create the Material from.
     * @return  A reference to the newly created Material.
     */
    static Ref<Material> Create(const Ref<Shader>& shader);

private:
    void AllocateStorage();
    void OnShaderReloaded();
    void BindTextures() const;

    ShaderUniformDeclaration*  FindUniformDeclaration(const std::string& name);
    ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name);
    Buffer& GetUniformBufferTarget(const ShaderUniformDeclaration* uniformDeclaration);

private:
    //! Reference to the Material's Shader.
    Ref<Shader> m_shader;
    //! Set of all material instances.
    std::unordered_set<MaterialInstance*> m_materialInstances;

    //! Buffer containing all of the Vertex Shader's uniforms.
    Buffer m_vSUniformStorageBuffer;
    //! Buffer containing all of the Fragment (Pixel) Shader's uniforms.
    Buffer m_pSUniformStorageBuffer;
    //! Buffer containing all of the Shader's textures.
    std::vector<Ref<Texture>> m_textures;
};

/**
 * @brief   An instance of a Material.
 */
class MaterialInstance
{
    friend class Material;

public:
    /**
     * @brief   Creates a MaterialInstance from a reference to a Material.
     * @param   material A reference to the Material to create an instance from.
     */
    MaterialInstance(const Ref<Material>& material);
    virtual ~MaterialInstance();

    /**
     * @brief   Set an uniform's value.
     * @param   name The name of the uniform to set
     * @param   value The value to assign to the uniform
     *
     * @note    This function asserts false if the uniform doesn't exist.
     */
    template<typename T>
    void Set(const std::string& name, const T& value)
    {
        // Get the declaration of the desired uniform.
        auto decl = m_material->FindUniformDeclaration(name);
        // Make sure the uniform exists.
        BR_CORE_ASSERT(decl, "Could not find uniform with name '{}'", name.c_str());

        // If it doesn't exist:
        if (decl == nullptr)
        {
            // Do nothing.
            return;
        }

        // Get the local buffer for that uniform.
        auto& buffer = GetUniformBufferTarget(decl);
        // Write the data into that uniform's local buffer.
        buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

        // Mark this uniform as being overwritten.
        m_overriddenValues.insert(name);
    }

    /**
     * @brief   Set a Texture in the Shader.
     * @param   name The name of the Texture to set
     * @param   texture A reference to the Texture to use.
     *
     * @note    This function asserts false if the resource doesn't exist.
     */
    void Set(const std::string& name, const Ref<Texture>& texture)
    {
        // Get the declaration of the desired resource.
        auto decl = m_material->FindResourceDeclaration(name);
        // Make sure that the resource actually exists.
        BR_CORE_ASSERT(decl, "Could not find resource with name '{}'", name.c_str());
        if (decl == nullptr)
        {
            return;
        }

        // Set the texture in the shader.
        uint32_t slot = decl->GetRegister();
        if (m_textures.size() <= slot)
        {
            m_textures.resize((size_t)slot + 1);
        }
        m_textures[slot] = texture;
    }

    /**
     * @brief   Set a Texture2D in the Shader.
     * @param   name The name of the Texture2D to set
     * @param   texture A reference to the Texture2D to use.
     *
     * @note    This function asserts false if the resource doesn't exist.
     */
    void Set(const std::string& name, const Ref<Texture2D>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

    /**
     * @brief   Set a TextureCube in the Shader.
     * @param   name The name of the TextureCube to set
     * @param   texture A reference to the TextureCube to use.
     *
     * @note    This function asserts false if the resource doesn't exist.
     */
    void Set(const std::string& name, const Ref<TextureCube>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

    /**
     * @brief   Binds all the information related to the shader:
     *          - The shader itself
     *          - Every uniforms for that shader
     *          - Every textures for that shader
     * @param   None
     * @return  None
     */
    void Bind() const;

public:
    /**
     * @brief   Creates an instance of a Material.
     * @param   material A reference to a Material to make an instance from.
     * @return  A reference to the created MaterialInstance.
     */
    static Ref<MaterialInstance> Create(const Ref<Material>& material);

private:
    void    AllocateStorage();
    void    OnShaderReloaded();
    Buffer& GetUniformBufferTarget(const ShaderUniformDeclaration* uniformDeclaration);
    void    OnMaterialValueUpdated(const ShaderUniformDeclaration* decl);

private:
public:
    //! A reference to the original Material.
    Ref<Material> m_material;

    //! Local uniform buffer for the Vertex Shader's uniforms.
    Buffer m_vSUniformStorageBuffer;
    //! Local uniform buffer for the Fragment/Pixel Shader's uniforms.
    Buffer m_pSUniformStorageBuffer;
    //! Local Texture buffer for the Shader's textures.
    std::vector<Ref<Texture>> m_textures;

    //! List of all the uniform that has been set.
    std::unordered_set<std::string> m_overriddenValues;
};

}    // namespace Brigerad
/**
 * @file    MeshComponent
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    1/29/2021 10:44:45 AM
 *
 * @brief
 ******************************************************************************
 * Copyright (C) 2021  Samuel Martel
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *****************************************************************************/
#pragma once

/*********************************************************************************************************************/
// [SECTION] Includes
/*********************************************************************************************************************/
#include "Brigerad/Core/Core.h"
#include "Brigerad/Renderer/Mesh.h"
#include "Brigerad/Renderer/Material.h"
#include "Brigerad/Renderer/Texture.h"

#include <string>

/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/
namespace Brigerad
{
struct AlbedoInput
{
    glm::vec3 Color = {
      0.972f,
      0.960f,
      0.915f};    // Silver, from
                  // https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
    Ref<Texture2D> TextureMap = nullptr;
    bool           SRGB       = true;    // Use SRGB color space.
    bool           UseTexture = false;
};

struct NormalInput
{
    Ref<Texture2D> TextureMap = nullptr;
    bool           UseTexture = false;
};

struct MetalnessInput
{
    float          Value      = 1.0f;
    Ref<Texture2D> TextureMap = nullptr;
    bool           UseTexture = false;
};

struct RoughnessInput
{
    float          Value      = 0.5f;
    Ref<Texture2D> TextureMap = nullptr;
    bool           UseTexture = false;
};

struct MeshComponent
{
    Ref<Mesh>             MeshRef       = nullptr;
    Ref<MaterialInstance> MaterialRef   = nullptr;
    std::string           MeshName      = "MeshComponent";
    std::string           MeshPath      = "";
    AlbedoInput           Albedo        = {};
    NormalInput           Normal        = {};
    MetalnessInput        Metalness     = {};
    RoughnessInput        Roughness     = {};
    Ref<Texture2D>        BrdfLut       = nullptr;
    bool                  viewDebugMenu = false;

    MeshComponent() = default;
    MeshComponent(const std::string& meshName, const std::string& meshPath)
    : MeshName(meshName), MeshPath(meshPath)
    {
        ReloadMesh();
    }
    MeshComponent(const std::string& meshName, const Ref<Mesh>& meshRef)
    : MeshName(meshName), MeshRef(meshRef), MeshPath(meshRef->GetFilePath())
    {
        ReloadMesh();
    }
    ~MeshComponent() = default;

    void SetMaterialUniforms()
    {
        MaterialRef->Set("u_AlbedoColor", Albedo.Color);
        MaterialRef->Set("u_Metalness", Metalness.Value);
        MaterialRef->Set("u_Roughness", Roughness.Value);
        MaterialRef->Set("u_AlbedoTexToggle", Albedo.UseTexture ? 1.0f : 0.0f);
        MaterialRef->Set("u_NormalTexToggle", Normal.UseTexture ? 1.0f : 0.0f);
        MaterialRef->Set("u_MetalnessTexToggle", Metalness.UseTexture ? 1.0f : 0.0f);
        MaterialRef->Set("u_RoughnessTexToggle", Roughness.UseTexture ? 1.0f : 0.0f);
        MaterialRef->Set("u_BRDFLUTTexture", BrdfLut);
    }

    void ReloadMesh(const std::string& meshPath = "")
    {
        if (meshPath.empty() == false)
        {
            MeshPath = meshPath;
        }
        if (MeshPath.empty() == true)
        {
            return;
        }
        MeshRef     = Mesh::Create(MeshPath);
        MaterialRef = CreateRef<MaterialInstance>(MeshRef->GetMaterial());
    }
};
}    // namespace Brigerad

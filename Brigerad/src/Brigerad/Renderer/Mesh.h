/**
 * @file    Mesh
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    1/17/2021 3:42:45 PM
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
#include <string>
#include "Brigerad/Core/Core.h"
#include "Brigerad/Core/Timestep.h"
#include "Shader.h"
#include "Texture.h"
#include "Brigerad/Renderer/Material.h"

namespace Brigerad
{
/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 binormal;
    glm::vec2 texCoord;
};

struct AnimatedVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 binormal;
    glm::vec2 texCoord;

    uint32_t ids[4]     = {0, 0, 0, 0};
    float    weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    void AddBoneData(uint32_t boneId, float weight)
    {
        for (size_t i = 0; i < 4; i++)
        {
            if (weights[i] == 0.0f)
            {
                ids[i]     = boneId;
                weights[i] = weight;
                return;
            }
        }

        BR_CORE_WARN(
          "Vertex has more than four bones/weights affecting it, extra data will be discarded: "
          "BoneID={}, Weight={}",
          boneId,
          weight);
    }
};

static constexpr int NUM_ATTRIBUTES = 5;

struct Index
{
    uint32_t v1, v2, v3;
};

static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

struct BoneInfo
{
    glm::mat4 boneOffset;
    glm::mat4 finalTrans;
};

struct VertexBoneData
{
    uint32_t ids[4];
    float    weights[4];

    VertexBoneData()
    {
        memset(ids, 0, sizeof(ids));
        memset(weights, 0, sizeof(weights));
    }

    void AddBoneData(uint32_t boneId, float weight)
    {
        for (size_t i = 0; i < 4; i++)
        {
            if (weights[i] == 0.0f)
            {
                ids[i]     = boneId;
                weights[i] = weight;
                return;
            }
        }

        BR_CORE_WARN(
          "Vertex has more than four bones/weights affecting it, extra data will be discarded: "
          "BoneID={}, Weight={}",
          boneId,
          weight);
    }
};

class Submesh
{
public:
    uint32_t baseVertex;
    uint32_t baseIndex;
    uint32_t materialIndex;
    uint32_t indexCount;

    glm::mat4 transform;
};

class Mesh
{
public:
    virtual ~Mesh() = default;

    virtual void Render(Timestep              ts,
                        Ref<MaterialInstance> materialInstance = Ref<MaterialInstance>()) = 0;
    virtual void Render(Timestep              ts,
                        const glm::mat4&      transform        = glm::mat4(1.0f),
                        Ref<MaterialInstance> materialInstance = Ref<MaterialInstance>()) = 0;
    virtual void OnImGuiRender()                                                          = 0;
    virtual void DumpVertexBuffer()                                                       = 0;

    virtual Ref<Shader>        GetMeshShader()     = 0;
    virtual Ref<Material>      GetMaterial()       = 0;
    virtual const std::string& GetFilePath() const = 0;

    static Ref<Mesh> Create(const std::string& fileName);

private:
};
}    // namespace Brigerad

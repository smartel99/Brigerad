/**
 * @file    OpenGLMesh
 * @author  Samuel Martel
 * @p       https://github.com/smartel99
 * @date    1/17/2021 3:47:12 PM
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
#include "Brigerad/Renderer/Mesh.h"
#include "Brigerad/Renderer/Buffer.h"
#include "Brigerad/Renderer/Shader.h"
#include "Brigerad/Renderer/Texture.h"
#include "Brigerad/Renderer/VertexArray.h"
#include "Brigerad/Scene/Scene.h"

namespace Assimp
{
class Importer;
}

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;
namespace Brigerad
{
/*********************************************************************************************************************/
// [SECTION] Defines
/*********************************************************************************************************************/


/*********************************************************************************************************************/
// [SECTION] Class Declarations
/*********************************************************************************************************************/

class OpenGLMesh : public Mesh
{
public:
    OpenGLMesh(const std::string& filename);
    virtual ~OpenGLMesh();

    virtual void Render(Timestep              ts,
                        Ref<MaterialInstance> materialInstance = Ref<MaterialInstance>()) override;
    virtual void Render(Timestep              ts,
                        const glm::mat4&      transform        = glm::mat4(1.0f),
                        Ref<MaterialInstance> materialInstance = Ref<MaterialInstance>()) override;
    virtual void OnImGuiRender() override;
    virtual void DumpVertexBuffer() override;

    virtual Ref<Shader>   GetMeshShader() override { return m_meshShader; }
    virtual Ref<Material> GetMaterial() override { return m_material; }


    virtual const std::string& GetFilePath() const override;

private:
    void BoneTransform(float time);
    void ReadNodeHierarchy(float            animationTime,
                           const aiNode*    pNode,
                           const glm::mat4& parentTransform);
    void TraverseNode(aiNode* node, int level = 0);

    const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);
    uint32_t          FindPosition(float animationTime, const aiNodeAnim* pNodeAnim);
    uint32_t          FindRotation(float animationTime, const aiNodeAnim* pNodeAnim);
    uint32_t          FindScaling(float animationTime, const aiNodeAnim* pNodeAnim);
    glm::vec3         InterpolateTranslation(float animationTime, const aiNodeAnim* pNodeAnim);
    glm::quat         InterpolateRotation(float animationTime, const aiNodeAnim* pNodeAnim);
    glm::vec3         InterpolateScale(float animationTime, const aiNodeAnim* pNodeAnim);

private:
    std::vector<Submesh>    m_submeshes;
    Scope<Assimp::Importer> m_importer;

    glm::mat4 m_inverseTranform;

    uint32_t              m_boneCount = 0;
    std::vector<BoneInfo> m_boneInfo;

    Ref<VertexArray> m_vertexArray;

    std::vector<Vertex>                       m_staticVertices;
    std::vector<AnimatedVertex>               m_animatedVertices;
    std::vector<Index>                        m_indices;
    std::unordered_map<std::string, uint32_t> m_boneMapping;
    std::vector<glm::mat4>                    m_boneTransforms;
    const aiScene*                            m_scene;

    // Materials
    Ref<Shader>   m_meshShader;
    Ref<Material> m_material;

    // Animation
    bool  m_isAnimated       = false;
    float m_animationTime    = 0.0f;
    float m_worldTime        = 0.0f;
    float m_timeMultiplier   = 1.0f;
    bool  m_animationPlaying = true;

    std::string m_filePath;
};
}    // namespace Brigerad

#include "brpch.h"
#include "OpenGLMesh.h"
#include "Brigerad/Renderer/Renderer.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"

#include "imgui.h"

#include "glad/glad.h"

namespace Brigerad
{
static const uint32_t s_meshImportFlags =
  aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case.
  aiProcess_Triangulate |             // Make sure we're triangles only.
  aiProcess_SortByPType |             // Split meshes by primitive type.
  aiProcess_GenNormals |              // Make sure we have legit normals.
  aiProcess_GenUVCoords |             // Convert UVs if required.
  aiProcess_OptimizeMeshes |          // Batch draws where possible.
  aiProcess_ValidateDataStructure;    // Validation.

struct LogStream : public Assimp::LogStream
{
    static void Initialize()
    {
        if (Assimp::DefaultLogger::isNullLogger())
        {
            Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
            Assimp::DefaultLogger::get()->attachStream(new LogStream,
                                                       Assimp::Logger::Err | Assimp::Logger::Warn);
        }
    }

    virtual void write(const char* message) override { BR_CORE_ERROR("Assimp error: {}", message); }
};

static glm::mat4 AiMatrix4x4ToGlm(const aiMatrix4x4& from)
{
    glm::mat4 to;
    // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1;
    to[1][0] = from.a2;
    to[2][0] = from.a3;
    to[3][0] = from.a4;
    to[0][1] = from.b1;
    to[1][1] = from.b2;
    to[2][1] = from.b3;
    to[3][1] = from.b4;
    to[0][2] = from.c1;
    to[1][2] = from.c2;
    to[2][2] = from.c3;
    to[3][2] = from.c4;
    to[0][3] = from.d1;
    to[1][3] = from.d2;
    to[2][3] = from.d3;
    to[3][3] = from.d4;
    return to;
}

OpenGLMesh::OpenGLMesh(const std::string& filename)
{
    LogStream::Initialize();

    BR_CORE_INFO("Loading mesh: {}", filename);

    m_importer = CreateScope<Assimp::Importer>();

    const aiScene* scene = m_importer->ReadFile(filename, s_meshImportFlags);
    if (scene == nullptr || scene->HasMeshes() == false)
    {
        BR_CORE_ERROR("Unable to load mesh file: {}", filename);
        return;
    }

    m_isAnimated = scene->mAnimations != nullptr;
    m_meshShader = m_isAnimated ? Renderer::GetShaderLibrary()->Get("mesh_anim")
                                : Renderer::GetShaderLibrary()->Get("mesh_static");
    m_material        = CreateRef<Material>(m_meshShader);
    m_inverseTranform = glm::inverse(AiMatrix4x4ToGlm(scene->mRootNode->mTransformation));

    uint32_t vertexCount = 0;
    uint32_t indexCount  = 0;

    m_submeshes.reserve(scene->mNumMeshes);
    for (size_t m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh* mesh = scene->mMeshes[m];

        Submesh submesh;
        submesh.baseVertex    = vertexCount;
        submesh.baseIndex     = indexCount;
        submesh.materialIndex = mesh->mMaterialIndex;
        submesh.indexCount    = mesh->mNumFaces * 3;
        m_submeshes.push_back(submesh);

        vertexCount += mesh->mNumVertices;
        indexCount += submesh.indexCount;

        BR_CORE_ASSERT(mesh->HasPositions() == true, "Meshes require positions!");
        BR_CORE_ASSERT(mesh->HasNormals() == true, "Meshes require normals!");

        // Vertices.
        if (m_isAnimated)
        {
            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                AnimatedVertex vertex;
                vertex.position = {
                  mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
                vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

                if (mesh->HasTangentsAndBitangents() == true)
                {
                    vertex.tangent = {
                      mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                    vertex.binormal = {
                      mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                }

                if (mesh->HasTextureCoords(0) == true)
                {
                    vertex.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
                }

                m_animatedVertices.push_back(vertex);
            }
        }
        else
        {
            for (size_t i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                vertex.position = {
                  mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
                vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

                if (mesh->HasTangentsAndBitangents() == true)
                {
                    vertex.tangent = {
                      mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z};
                    vertex.binormal = {
                      mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z};
                }

                if (mesh->HasTextureCoords(0) == true)
                {
                    vertex.texCoord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
                }

                m_staticVertices.push_back(vertex);
            }
        }

        // Indices.
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            BR_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Face must have 3 indices!");
            m_indices.push_back({mesh->mFaces[i].mIndices[0],
                                 mesh->mFaces[i].mIndices[1],
                                 mesh->mFaces[i].mIndices[2]});
        }
    }

    BR_CORE_TRACE("NODES:");
    BR_CORE_TRACE("-----------------------------");
    TraverseNode(scene->mRootNode);
    BR_CORE_TRACE("-----------------------------");

    // Bones.
    if (m_isAnimated)
    {
        for (size_t m = 0; m < scene->mNumMeshes; m++)
        {
            aiMesh*  mesh    = scene->mMeshes[m];
            Submesh& submesh = m_submeshes[m];

            for (size_t i = 0; i < mesh->mNumBones; i++)
            {
                aiBone*     bone      = mesh->mBones[i];
                std::string boneName  = bone->mName.data;
                int         boneIndex = 0;

                if (m_boneMapping.find(boneName) == m_boneMapping.end())
                {
                    // Allocate an index for a new bone.
                    boneIndex = m_boneCount++;
                    BoneInfo bi;
                    m_boneInfo.push_back(bi);
                    m_boneInfo[boneIndex].boneOffset = AiMatrix4x4ToGlm(bone->mOffsetMatrix);
                    m_boneMapping[boneName]          = boneIndex;
                }
                else
                {
                    BR_CORE_TRACE("Found existing bone in bone map");
                    boneIndex = m_boneMapping[boneName];
                }

                for (size_t j = 0; j < bone->mNumWeights; j++)
                {
                    int   vertexId = submesh.baseVertex + bone->mWeights[j].mVertexId;
                    float weight   = bone->mWeights[j].mWeight;
                    m_animatedVertices[vertexId].AddBoneData(boneIndex, weight);
                }
            }
        }
    }

    // Vertex Array.
    m_vertexArray = VertexArray::Create();
    if (m_isAnimated == true)
    {
        Ref<VertexBuffer> vb = VertexBuffer::Create(
          m_animatedVertices.data(), m_animatedVertices.size() * sizeof(AnimatedVertex));
        vb->SetLayout({{ShaderDataType::Float3, "a_position"},
                       {ShaderDataType::Float3, "a_normal"},
                       {ShaderDataType::Float3, "a_tangent"},
                       {ShaderDataType::Float3, "a_binormal"},
                       {ShaderDataType::Float2, "a_texCoord"},
                       {ShaderDataType::Int4, "a_boneIds"},
                       {ShaderDataType::Float4, "a_boneWeights"}});
        m_vertexArray->AddVertexBuffer(vb);
    }
    else
    {
        Ref<VertexBuffer> vb =
          VertexBuffer::Create(m_staticVertices.data(), m_staticVertices.size() * sizeof(Vertex));
        vb->SetLayout({{ShaderDataType::Float3, "a_position"},
                       {ShaderDataType::Float3, "a_normal"},
                       {ShaderDataType::Float3, "a_tangent"},
                       {ShaderDataType::Float3, "a_binormal"},
                       {ShaderDataType::Float2, "a_texCoord"}});
        m_vertexArray->AddVertexBuffer(vb);
    }

    auto ib = IndexBuffer::Create(m_indices.data(), m_indices.size() * sizeof(Index));
    m_vertexArray->SetIndexBuffer(ib);
    m_scene = scene;
}

OpenGLMesh::~OpenGLMesh()
{
}

void OpenGLMesh::Render(Timestep              ts,
                        Ref<MaterialInstance> materialInstance /*= Ref<MaterialInstance>()*/)
{
    Render(ts, glm::mat4(1.0f), materialInstance);
}

void OpenGLMesh::Render(Timestep              ts,
                        const glm::mat4&      transform /*= glm::mat4(1.0f)*/,
                        Ref<MaterialInstance> materialInstance /*= Ref<MaterialInstance>()*/)
{
    if (m_isAnimated == true)
    {
        if (m_animationPlaying == true)
        {
            m_worldTime += ts;

            float ticksPerSec = (float)(m_scene->mAnimations[0]->mTicksPerSecond != 0
                                          ? m_scene->mAnimations[0]->mTicksPerSecond
                                          : 25.0f) *
                                m_timeMultiplier;

            m_animationTime += ts * ticksPerSec;
            m_animationTime = fmod(m_animationTime, (float)m_scene->mAnimations[0]->mDuration);
        }

        BoneTransform(m_animationTime);
    }

    if (materialInstance != nullptr)
    {
        materialInstance->Bind();
    }

    m_vertexArray->Bind();

    bool materialOverride = materialInstance != nullptr;

    for (Submesh& submesh : m_submeshes)
    {
        if (m_isAnimated == true)
        {
            for (size_t i = 0; i < m_boneTransforms.size(); i++)
            {
                std::string uniformName =
                  std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
                m_meshShader->SetMat4(uniformName, m_boneTransforms[i]);
            }
        }

        if (materialOverride == false)
        {
            m_meshShader->SetMat4("u_ModelMatrix", transform * submesh.transform);
        }

        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 submesh.indexCount,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(uint32_t) * submesh.baseIndex),
                                 submesh.baseVertex);
    }


    m_vertexArray->Unbind();
}

void OpenGLMesh::OnImGuiRender()
{
    ImGui::Begin("Mesh Debug");
    if (ImGui::CollapsingHeader(m_filePath.c_str()))
    {
        if (m_isAnimated == true)
        {
            if (ImGui::CollapsingHeader("Animation"))
            {
                if (ImGui::Button(m_animationPlaying ? "Pause" : "Play"))
                {
                    m_animationPlaying = !m_animationPlaying;
                }

                ImGui::SliderFloat("##AnimationTime",
                                   &m_animationTime,
                                   0.0f,
                                   (float)m_scene->mAnimations[0]->mDuration);
                ImGui::DragFloat("Time Scale", &m_timeMultiplier, 0.05f, 0.0f, 10.0f);
            }
        }
    }

    ImGui::End();
}

void OpenGLMesh::DumpVertexBuffer()
{
    BR_CORE_TRACE("------------------------------------------------------");
    BR_CORE_TRACE("Vertex Buffer Dump");
    BR_CORE_TRACE("Mesh: {}", m_filePath.c_str());

    if (m_isAnimated == true)
    {
        for (size_t i = 0; i < m_animatedVertices.size(); i++)
        {
            auto& vertex = m_animatedVertices[i];
            BR_CORE_TRACE("Vertex: {}", i);
            BR_CORE_TRACE(
              "Position: {0}, {1}, {2}", vertex.position.x, vertex.position.y, vertex.position.z);
            BR_CORE_TRACE(
              "Normal: {0}, {1}, {2}", vertex.normal.x, vertex.normal.y, vertex.normal.z);
            BR_CORE_TRACE(
              "Binormal: {0}, {1}, {2}", vertex.binormal.x, vertex.binormal.y, vertex.binormal.z);
            BR_CORE_TRACE(
              "Tangent: {0}, {1}, {2}", vertex.tangent.x, vertex.tangent.y, vertex.tangent.z);
            BR_CORE_TRACE("TexCoord: {0}, {1}", vertex.texCoord.x, vertex.texCoord.y);
            BR_CORE_TRACE("--");
        }
    }
    else
    {
        for (size_t i = 0; i < m_staticVertices.size(); i++)
        {
            auto& vertex = m_staticVertices[i];
            BR_CORE_TRACE("Vertex: {0}", i);
            BR_CORE_TRACE(
              "Position: {0}, {1}, {2}", vertex.position.x, vertex.position.y, vertex.position.z);
            BR_CORE_TRACE(
              "Normal: {0}, {1}, {2}", vertex.normal.x, vertex.normal.y, vertex.normal.z);
            BR_CORE_TRACE(
              "Binormal: {0}, {1}, {2}", vertex.binormal.x, vertex.binormal.y, vertex.binormal.z);
            BR_CORE_TRACE(
              "Tangent: {0}, {1}, {2}", vertex.tangent.x, vertex.tangent.y, vertex.tangent.z);
            BR_CORE_TRACE("TexCoord: {0}, {1}", vertex.texCoord.x, vertex.texCoord.y);
            BR_CORE_TRACE("--");
        }
    }
    BR_CORE_TRACE("------------------------------------------------------");
}

const std::string& OpenGLMesh::GetFilePath() const
{
    return m_filePath;
}

void OpenGLMesh::BoneTransform(float time)
{
    ReadNodeHierarchy(time, m_scene->mRootNode, glm::mat4(1.0f));
    m_boneTransforms.resize(m_boneCount);
    for (size_t i = 0; i < m_boneCount; i++)
    {
        m_boneTransforms[i] = m_boneInfo[i].finalTrans;
    }
}

void OpenGLMesh::ReadNodeHierarchy(float            animationTime,
                                   const aiNode*    pNode,
                                   const glm::mat4& parentTransform)
{
    std::string        name          = pNode->mName.data;
    const aiAnimation* animation     = m_scene->mAnimations[0];
    glm::mat4          nodeTransform = glm::mat4(AiMatrix4x4ToGlm(pNode->mTransformation));
    const aiNodeAnim*  nodeAnim      = FindNodeAnim(animation, name);

    if (nodeAnim != nullptr)
    {
        glm::vec3 trans    = InterpolateTranslation(animationTime, nodeAnim);
        glm::mat4 transMat = glm::translate(glm::mat4(1.0f), trans);

        glm::quat rot    = InterpolateRotation(animationTime, nodeAnim);
        glm::mat4 rotMat = glm::toMat4(rot);

        glm::vec3 scale    = InterpolateScale(animationTime, nodeAnim);
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), scale);

        nodeTransform = transMat * rotMat * scaleMat;
    }

    glm::mat4 transform = parentTransform * nodeTransform;

    if (m_boneMapping.find(name) != m_boneMapping.end())
    {
        uint32_t boneIdx = m_boneMapping[name];
        m_boneInfo[boneIdx].finalTrans =
          m_inverseTranform * transform * m_boneInfo[boneIdx].boneOffset;
    }

    for (size_t i = 0; i < pNode->mNumChildren; i++)
    {
        ReadNodeHierarchy(animationTime, pNode->mChildren[i], transform);
    }
}

void OpenGLMesh::TraverseNode(aiNode* node, int level /*= 0*/)
{
    std::string levelText;
    for (size_t i = 0; i < level; i++)
    {
        levelText += "-";
    }
    BR_CORE_TRACE("{}Node name: {}", levelText.c_str(), node->mName.data);
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        uint32_t mesh               = node->mMeshes[i];
        m_submeshes[mesh].transform = AiMatrix4x4ToGlm(node->mTransformation);
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        aiNode* child = node->mChildren[i];
        TraverseNode(child, level + 1);
    }
}

const aiNodeAnim* OpenGLMesh::FindNodeAnim(const aiAnimation* animation,
                                           const std::string& nodeName)
{
    for (size_t i = 0; i < animation->mNumChannels; i++)
    {
        const aiNodeAnim* nodeAnim = animation->mChannels[i];
        if (std::string(nodeAnim->mNodeName.data) == nodeName)
        {
            return nodeAnim;
        }
    }

    return nullptr;
}

uint32_t OpenGLMesh::FindPosition(float animationTime, const aiNodeAnim* pNodeAnim)
{
    BR_CORE_ASSERT(pNodeAnim->mNumPositionKeys > 0, "Node has no position keys");

    for (size_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
    {
        if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
        {
            return i;
        }
    }

    return 0;
}

uint32_t OpenGLMesh::FindRotation(float animationTime, const aiNodeAnim* pNodeAnim)
{
    BR_CORE_ASSERT(pNodeAnim->mNumRotationKeys > 0, "Node has no rotation keys");

    for (size_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
    {
        if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }

    return 0;
}

uint32_t OpenGLMesh::FindScaling(float animationTime, const aiNodeAnim* pNodeAnim)
{
    BR_CORE_ASSERT(pNodeAnim->mNumScalingKeys > 0, "Node has no scaling keys");

    for (size_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
    {
        if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
        {
            return i;
        }
    }

    return 0;
}

glm::vec3 OpenGLMesh::InterpolateTranslation(float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1)
    {
        // No interpolation necessary for single value.
        auto v = pNodeAnim->mPositionKeys[0].mValue;
        return {v.x, v.y, v.z};
    }
    else
    {
        uint32_t posIdx     = FindPosition(animationTime, pNodeAnim);
        uint32_t nextPosIdx = posIdx + 1;
        BR_CORE_ASSERT(nextPosIdx < pNodeAnim->mNumPositionKeys, "Invalid position index");
        float deltaTime = (float)(pNodeAnim->mPositionKeys[nextPosIdx].mTime -
                                  pNodeAnim->mPositionKeys[posIdx].mTime);
        float factor = (animationTime - (float)pNodeAnim->mPositionKeys[posIdx].mTime) / deltaTime;

        if (factor < 0.0f)
        {
            factor = 0.0f;
        }
        else if (factor > 1.0f)
        {
            factor = 1.0f;
        }

        const aiVector3D& start        = pNodeAnim->mPositionKeys[posIdx].mValue;
        const aiVector3D& end          = pNodeAnim->mPositionKeys[nextPosIdx].mValue;
        aiVector3D        delta        = end - start;
        aiVector3D        interpolated = start + factor * delta;
        return {interpolated.x, interpolated.y, interpolated.z};
    }
}

glm::quat OpenGLMesh::InterpolateRotation(float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1)
    {
        // No interpolation necessary for single value.
        auto v = pNodeAnim->mRotationKeys[0].mValue;
        return glm::quat {v.w, v.x, v.y, v.z};
    }
    else
    {
        uint32_t rotIdx     = FindRotation(animationTime, pNodeAnim);
        uint32_t nextRotIdx = rotIdx + 1;
        BR_CORE_ASSERT(nextRotIdx < pNodeAnim->mNumRotationKeys, "Invalid position index");
        float deltaTime = (float)(pNodeAnim->mRotationKeys[nextRotIdx].mTime -
                                  pNodeAnim->mRotationKeys[rotIdx].mTime);
        float factor = (animationTime - (float)pNodeAnim->mRotationKeys[rotIdx].mTime) / deltaTime;

        if (factor < 0.0f)
        {
            factor = 0.0f;
        }
        else if (factor > 1.0f)
        {
            factor = 1.0f;
        }

        const aiQuaternion& start = pNodeAnim->mRotationKeys[rotIdx].mValue;
        const aiQuaternion& end   = pNodeAnim->mRotationKeys[nextRotIdx].mValue;
        aiQuaternion        q     = aiQuaternion();
        aiQuaternion::Interpolate(q, start, end, factor);
        q = q.Normalize();
        return glm::quat(q.w, q.x, q.y, q.z);
    }
}

glm::vec3 OpenGLMesh::InterpolateScale(float animationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1)
    {
        // No interpolation necessary for single value.
        auto v = pNodeAnim->mScalingKeys[0].mValue;
        return {v.x, v.y, v.z};
    }
    else
    {
        uint32_t posIdx     = FindScaling(animationTime, pNodeAnim);
        uint32_t nextPosIdx = posIdx + 1;
        BR_CORE_ASSERT(nextPosIdx < pNodeAnim->mNumScalingKeys, "Invalid position index");
        float deltaTime = (float)(pNodeAnim->mScalingKeys[nextPosIdx].mTime -
                                  pNodeAnim->mScalingKeys[posIdx].mTime);
        float factor = (animationTime - (float)pNodeAnim->mScalingKeys[posIdx].mTime) / deltaTime;

        if (factor < 0.0f)
        {
            factor = 0.0f;
        }
        else if (factor > 1.0f)
        {
            factor = 1.0f;
        }

        const aiVector3D& start        = pNodeAnim->mScalingKeys[posIdx].mValue;
        const aiVector3D& end          = pNodeAnim->mScalingKeys[nextPosIdx].mValue;
        aiVector3D        delta        = end - start;
        aiVector3D        interpolated = start + factor * delta;
        return {interpolated.x, interpolated.y, interpolated.z};
    }
}

}    // namespace Brigerad

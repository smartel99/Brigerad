#include "brpch.h"
#include "Renderer2D.h"

#include "Brigerad/Renderer/VertexArray.h"
#include "Brigerad/Renderer/Shader.h"
#include "Brigerad/Renderer/RenderCommand.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Brigerad
{
/**
 * @brief   Data structure that contains all the information needed by a quad.
 */
struct QuadVertex
{
    glm::vec3 position;      // Screen coordinates of the quad.
    glm::vec4 color;         // Color of the quad.
    glm::vec2 texCoord;      // Coordinates to sample the texture from.
    glm::vec2 tilingFactor;  // Scaling applied to the sampled texture.
    float texIndex;          // Index of the texture to use.
};

/**
 * @brief   Runtime data structure that contains all the things needed by the
 *          renderer at runtime.
 */
struct Renderer2DData
{
    // Maximum amount of quads a single draw call can handle.
    const uint32_t maxQuads    = 10000;         // Max 10k quads.
    const uint32_t maxVertices = maxQuads * 4;  // 4 vertices per quad.
    const uint32_t maxIndices  = maxQuads * 6;  // 6 indices per quad.
    static const uint32_t maxTextureSlots = 32;  // Max number of textures per draw call.

    Ref<VertexArray> vertexArray;
    Ref<VertexBuffer> vertexBuffer;
    Ref<Shader> textureShader;    // Shader used at runtime.
    Ref<Texture2D> whiteTexture;  // Default empty texture for flat color quads.

    long long frameCount = 0;  // Frames rendered since the start of the application.

    // Number of quads queued to be drawn in this frame.
    uint32_t quadIndexCount = 0;
    // Origin of the buffer of queue of quads.
    QuadVertex* quadVertexBufferBase = nullptr;
    // Pointer to the location of the last quad currently queued.
    QuadVertex* quadVertexBufferPtr = nullptr;

    // CPU-sided representation of the texture memory in the GPU.
    std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
    // Current index of the last texture in the texture buffer.
    uint32_t textureSlotIndex = 1;  // 0 = white texture.
};

// Instance of the runtime data for the renderer.
static Renderer2DData s_data;

/**
 * @brief   Initialize the 2D renderer.
 *          This sets everything up to be able to render, amongst other things, quads.
 */
void Renderer2D::Init()
{
    BR_PROFILE_FUNCTION();

    // Instantiate the vertex array used by the 2D renderer.
    s_data.vertexArray = VertexArray::Create();

    // Instantiate the vertex buffer to contain the maximum possible number
    // of quads that can be rendered in a single draw call.
    s_data.vertexBuffer = VertexBuffer::Create(s_data.maxVertices * sizeof(QuadVertex));

    // Set up the layout of the shader.
    s_data.vertexBuffer->SetLayout({ { ShaderDataType::Float3, "a_position" },
                                     { ShaderDataType::Float4, "a_color" },
                                     { ShaderDataType::Float2, "a_TexCoord" },
                                     { ShaderDataType::Float2, "a_TilingFactor" },
                                     { ShaderDataType::Float, "a_TexIndex" } });
    s_data.vertexArray->AddVertexBuffer(s_data.vertexBuffer);

    // Heap allocate a buffer for all quads that can be rendered in a single draw call.
    s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertices];

    // Heap allocate a buffer for all quad indices that can be rendered in a single draw call.
    uint32_t* quadIndices = new uint32_t[s_data.maxIndices];

    uint32_t offset = 0;
    for (uint32_t i = 0; i < s_data.maxIndices; i += 6)
    {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }

    Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_data.maxIndices);
    s_data.vertexArray->SetIndexBuffer(quadIB);
    delete[] quadIndices;

    s_data.whiteTexture       = Texture2D::Create(1, 1);
    uint32_t whiteTextureData = 0xFFFFFFFF;
    s_data.whiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

    s_data.textureShader = Shader::Create("assets/shaders/Texture.glsl");
    s_data.textureShader->Bind();

    int32_t samplers[s_data.maxTextureSlots];
    for (int32_t i = 0; i < s_data.maxTextureSlots; i++)
    {
        samplers[i] = i;
    }

    s_data.textureShader->SetIntArray("u_Textures", samplers, s_data.maxTextureSlots);

    // Set all texture slots to 0.
    s_data.textureSlots[0] = s_data.whiteTexture;
}

void Renderer2D::Shutdown() { BR_PROFILE_FUNCTION(); }

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
    BR_PROFILE_FUNCTION();

    s_data.textureShader->Bind();
    s_data.textureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

    s_data.quadIndexCount      = 0;
    s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
    s_data.textureSlotIndex    = 1;

    s_data.frameCount++;
}

void Renderer2D::EndScene()
{
    BR_PROFILE_FUNCTION();

    uint32_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase;
    s_data.vertexBuffer->SetData(s_data.quadVertexBufferBase, dataSize);

    Flush();
}

void Renderer2D::Flush()
{
    BR_PROFILE_FUNCTION();

    // Bind all active textures.
    for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
    {
        s_data.textureSlots[i]->Bind(i);
    }
    RenderCommand::DrawIndexed(s_data.vertexArray, s_data.quadIndexCount);
}

long long Renderer2D::GetFrameCount() { return s_data.frameCount; }

/* ------------------------------------------------------------------------- */
/* Primitives -------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

// ----- DRAW QUAD -----

void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
{
    DrawQuad(glm::vec3(pos.x, pos.y, 0.0f), size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
{
    BR_PROFILE_FUNCTION();

    const float texIndex = 0.0f;  // White Texture.

    s_data.quadVertexBufferPtr->position     = pos;
    s_data.quadVertexBufferPtr->color        = color;
    s_data.quadVertexBufferPtr->texCoord     = { 0.0f, 0.0f };
    s_data.quadVertexBufferPtr->tilingFactor = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->texIndex     = texIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position     = { pos.x + size.x, pos.y, pos.z };
    s_data.quadVertexBufferPtr->color        = color;
    s_data.quadVertexBufferPtr->texCoord     = { 1.0f, 0.0f };
    s_data.quadVertexBufferPtr->tilingFactor = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->texIndex     = texIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position = { pos.x + size.x, pos.y + size.y, pos.z };
    s_data.quadVertexBufferPtr->color        = color;
    s_data.quadVertexBufferPtr->texCoord     = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->tilingFactor = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->texIndex     = texIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position     = { pos.x, pos.y + size.y, pos.z };
    s_data.quadVertexBufferPtr->color        = color;
    s_data.quadVertexBufferPtr->texCoord     = { 0.0f, 1.0f };
    s_data.quadVertexBufferPtr->tilingFactor = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->texIndex     = texIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadIndexCount += 6;

#if 0
    s_data.textureShader->SetFloat2("u_TextureScale", {1.0f, 1.0f});
    s_data.whiteTexture->Bind();

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    s_data.textureShader->SetMat4("u_Transform", transform);

    s_data.vertexArray->Bind();
    RenderCommand::DrawIndexed(s_data.vertexArray);
#endif
}

void Renderer2D::DrawQuad(const glm::vec2& pos,
                          const glm::vec2& size,
                          const Ref<Texture2D>& texture,
                          const glm::vec2& textScale,
                          const glm::vec4& tint)
{
    DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, textScale, tint);
}

void Renderer2D::DrawQuad(const glm::vec3& pos,
                          const glm::vec2& size,
                          const Ref<Texture2D>& texture,
                          const glm::vec2& textScale,
                          const glm::vec4& tint)
{
    BR_PROFILE_FUNCTION();

    float textureIndex = 0.0f;

    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
    {
        if (*s_data.textureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = (float)s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex] = texture;
        s_data.textureSlotIndex++;
    }

    s_data.quadVertexBufferPtr->position     = pos;
    s_data.quadVertexBufferPtr->color        = tint;
    s_data.quadVertexBufferPtr->texCoord     = { 0.0f, 0.0f };
    s_data.quadVertexBufferPtr->tilingFactor = textScale;
    s_data.quadVertexBufferPtr->texIndex     = textureIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position     = { pos.x + size.x, pos.y, pos.z };
    s_data.quadVertexBufferPtr->color        = tint;
    s_data.quadVertexBufferPtr->texCoord     = { 1.0f, 0.0f };
    s_data.quadVertexBufferPtr->tilingFactor = textScale;
    s_data.quadVertexBufferPtr->texIndex     = textureIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position = { pos.x + size.x, pos.y + size.y, pos.z };
    s_data.quadVertexBufferPtr->color        = tint;
    s_data.quadVertexBufferPtr->texCoord     = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->tilingFactor = textScale;
    s_data.quadVertexBufferPtr->texIndex     = textureIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position     = { pos.x, pos.y + size.y, pos.z };
    s_data.quadVertexBufferPtr->color        = tint;
    s_data.quadVertexBufferPtr->texCoord     = { 0.0f, 1.0f };
    s_data.quadVertexBufferPtr->tilingFactor = textScale;
    s_data.quadVertexBufferPtr->texIndex     = textureIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadIndexCount += 6;
#if 0
    s_data.textureShader->Bind();
    s_data.textureShader->SetFloat4("u_Color", tint);
    s_data.textureShader->SetFloat2("u_TextureScale", textScale);

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    s_data.textureShader->SetMat4("u_Transform", transform);

    texture->Bind();

    s_data.vertexArray->Bind();
    RenderCommand::DrawIndexed(s_data.vertexArray);
#endif
}

// ----- DRAW ROTATED QUAD -----

void Renderer2D::DrawRotatedQuad(const glm::vec2& pos,
                                 const glm::vec2& size,
                                 const glm::vec4& color,
                                 float rotation)
{
    DrawRotatedQuad(glm::vec3(pos.x, pos.y, 0.0f), size, color, rotation);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& pos,
                                 const glm::vec2& size,
                                 const glm::vec4& color,
                                 float rotation)
{
    BR_PROFILE_FUNCTION();

    const float texIndex = 0.0f;  // White Texture.

    s_data.quadVertexBufferPtr->position     = pos;
    s_data.quadVertexBufferPtr->color        = color;
    s_data.quadVertexBufferPtr->texCoord     = { 0.0f, 0.0f };
    s_data.quadVertexBufferPtr->tilingFactor = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->texIndex     = texIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position     = { pos.x + size.x, pos.y, pos.z };
    s_data.quadVertexBufferPtr->color        = color;
    s_data.quadVertexBufferPtr->texCoord     = { 1.0f, 0.0f };
    s_data.quadVertexBufferPtr->tilingFactor = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->texIndex     = texIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position = { pos.x + size.x, pos.y + size.y, pos.z };
    s_data.quadVertexBufferPtr->color        = color;
    s_data.quadVertexBufferPtr->texCoord     = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->tilingFactor = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->texIndex     = texIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position     = { pos.x, pos.y + size.y, pos.z };
    s_data.quadVertexBufferPtr->color        = color;
    s_data.quadVertexBufferPtr->texCoord     = { 0.0f, 1.0f };
    s_data.quadVertexBufferPtr->tilingFactor = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->texIndex     = texIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadIndexCount += 6;
#if 0
    BR_PROFILE_FUNCTION();
    s_data.textureShader->SetFloat4("u_Color", color);
    s_data.whiteTexture->Bind();

    glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), pos) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
      glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    s_data.textureShader->SetMat4("u_Transform", transform);

    s_data.vertexArray->Bind();
    RenderCommand::DrawIndexed(s_data.vertexArray);
#endif
}

void Renderer2D::DrawRotatedQuad(const glm::vec2& pos,
                                 const glm::vec2& size,
                                 const Ref<Texture2D>& texture,
                                 const glm::vec2& textScale,
                                 const glm::vec4& tint,
                                 float rotation)
{
    DrawRotatedQuad({ pos.x, pos.y, 0.0f }, size, texture, textScale, tint, rotation);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3& pos,
                                 const glm::vec2& size,
                                 const Ref<Texture2D>& texture,
                                 const glm::vec2& textScale,
                                 const glm::vec4& tint,
                                 float rotation)
{
    BR_PROFILE_FUNCTION();

    float textureIndex = 0.0f;

    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
    {
        if (*s_data.textureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = (float)s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex] = texture;
        s_data.textureSlotIndex++;
    }

    s_data.quadVertexBufferPtr->position     = pos;
    s_data.quadVertexBufferPtr->color        = tint;
    s_data.quadVertexBufferPtr->texCoord     = { 0.0f, 0.0f };
    s_data.quadVertexBufferPtr->tilingFactor = textScale;
    s_data.quadVertexBufferPtr->texIndex     = textureIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position     = { pos.x + size.x, pos.y, pos.z };
    s_data.quadVertexBufferPtr->color        = tint;
    s_data.quadVertexBufferPtr->texCoord     = { 1.0f, 0.0f };
    s_data.quadVertexBufferPtr->tilingFactor = textScale;
    s_data.quadVertexBufferPtr->texIndex     = textureIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position = { pos.x + size.x, pos.y + size.y, pos.z };
    s_data.quadVertexBufferPtr->color        = tint;
    s_data.quadVertexBufferPtr->texCoord     = { 1.0f, 1.0f };
    s_data.quadVertexBufferPtr->tilingFactor = textScale;
    s_data.quadVertexBufferPtr->texIndex     = textureIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadVertexBufferPtr->position     = { pos.x, pos.y + size.y, pos.z };
    s_data.quadVertexBufferPtr->color        = tint;
    s_data.quadVertexBufferPtr->texCoord     = { 0.0f, 1.0f };
    s_data.quadVertexBufferPtr->tilingFactor = textScale;
    s_data.quadVertexBufferPtr->texIndex     = textureIndex;
    s_data.quadVertexBufferPtr++;

    s_data.quadIndexCount += 6;

#if 0
    BR_PROFILE_FUNCTION();

    s_data.textureShader->Bind();
    s_data.textureShader->SetFloat4("u_Color", tint);
    s_data.textureShader->SetFloat2("u_TextureScale", textScale);

    glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), pos) *
      glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
      glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    s_data.textureShader->SetMat4("u_Transform", transform);

    texture->Bind();

    s_data.vertexArray->Bind();
    RenderCommand::DrawIndexed(s_data.vertexArray);
#endif
}


}  // namespace Brigerad
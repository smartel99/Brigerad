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
    glm::vec3 position;        // Screen coordinates of the quad.
    glm::vec4 color;           // Color of the quad.
    glm::vec2 texCoord;        // Coordinates to sample the texture from.
    glm::vec2 tilingFactor;    // Scaling applied to the sampled texture.
    float     texIndex;        // Index of the texture to use.
    float     isText;          // Is the quad text data.
};

/**
 * @brief   Runtime data structure that contains all the things needed by the
 *          renderer at runtime.
 */
struct Renderer2DData
{
    // Maximum amount of quads a single draw call can handle.
    static const uint32_t maxQuads        = 100000;          // Max 10k quads.
    static const uint32_t maxVertices     = maxQuads * 4;    // 4 vertices per quad.
    static const uint32_t maxIndices      = maxQuads * 6;    // 6 indices per quad.
    static const uint32_t maxTextureSlots = 32;    // Max number of textures per draw call.

    Ref<VertexArray>  vertexArray;
    Ref<VertexBuffer> vertexBuffer;
    Ref<Shader>       textureShader;    // Shader used at runtime.
    Ref<Texture2D>    whiteTexture;     // Default empty texture for flat color quads.

    long long frameCount = 0;    // Frames rendered since the start of the application.

    // Number of quads queued to be drawn in this frame.
    uint32_t quadIndexCount = 0;
    // Origin of the buffer of queue of quads.
    QuadVertex* quadVertexBufferBase = nullptr;
    // Pointer to the location of the last quad currently queued.
    QuadVertex* quadVertexBufferPtr = nullptr;

    // CPU-sided representation of the texture memory in the GPU.
    std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
    // Current index of the last texture in the texture buffer.
    uint32_t textureSlotIndex = 1;    // 0 = white texture.

    glm::vec4 quadVertexPosition[4];

    Renderer2D::Statistics stats;
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
    s_data.vertexBuffer->SetLayout({{ShaderDataType::Float3, "a_position"},
                                    {ShaderDataType::Float4, "a_color"},
                                    {ShaderDataType::Float2, "a_TexCoord"},
                                    {ShaderDataType::Float2, "a_TilingFactor"},
                                    {ShaderDataType::Float, "a_TexIndex"},
                                    {ShaderDataType::Float, "a_IsText"}});

    s_data.vertexArray->AddVertexBuffer(s_data.vertexBuffer);

    // Heap allocate a buffer for all quads that can be rendered in a single draw call.
    s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertices];

    // Heap allocate a buffer for all quad indices that can be rendered in a single draw call.
    uint32_t* quadIndices = new uint32_t[s_data.maxIndices];

    // Initialize the indices of all quads possible.
    // Ex:  Quad 1 -> { 0, 1, 2, 2, 3, 0 }
    //      Quad 2 -> { 4, 5, 6, 6, 7, 4 }
    //      ...
    //      Quad n -> { n, n+1, n+2, n+2, n+3, n }
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

    // Create an index buffer for all of the indices we just set.
    Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_data.maxIndices);
    // Bind that index buffer to our vertex array.
    s_data.vertexArray->SetIndexBuffer(quadIB);
    // Free up the memory.
    delete[] quadIndices;

    // Create a 1x1 white texture that we will use with flat colored quads.
    s_data.whiteTexture       = Texture2D::Create(1, 1);
    uint32_t whiteTextureData = 0xFFFFFFFF;
    s_data.whiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

    // Load, compile and link the shader for 2D quads.
    s_data.textureShader = Shader::Create("assets/shaders/Texture.glsl");
    s_data.textureShader->Bind();

    // Setup the texture uniform in the fragment shader.
    // This array contains all of the possible texture slots that the shader
    // can use.
    int32_t samplers[s_data.maxTextureSlots];
    for (int32_t i = 0; i < s_data.maxTextureSlots; i++)
    {
        samplers[i] = i;
    }
    s_data.textureShader->SetIntArray("u_Textures", samplers, s_data.maxTextureSlots);

    // Set the first texture slot to be the 1x1 white texture.
    s_data.textureSlots[0] = s_data.whiteTexture;

    s_data.quadVertexPosition[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    s_data.quadVertexPosition[1] = {0.5f, -0.5f, 0.0f, 1.0f};
    s_data.quadVertexPosition[2] = {0.5f, 0.5f, 0.0f, 1.0f};
    s_data.quadVertexPosition[3] = {-0.5f, 0.5f, 0.0f, 1.0f};
}

/**
 * @brief   Gracefully shutdown the 2D renderer.
 *          Currently, nothing in particular needs to be done in that method.
 */
void Renderer2D::Shutdown()
{
    BR_PROFILE_FUNCTION();
    delete[] s_data.quadVertexBufferBase;
}

/**
 * @brief Set everything up in the 2D renderer to begin accepting new draw
 *        calls for this frame.
 *
 * @param camera An orthographic representation of the scene that is viewable
 *               by the user.
 */
void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
    BR_PROFILE_FUNCTION();

    // Upload the view-projection matrix of the camera into the vertex shader.
    s_data.textureShader->Bind();
    s_data.textureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

    // Reset the quad buffer.
    s_data.quadIndexCount      = 0;
    s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

    // Reset the texture buffer.
    // We set it to 1 instead of 0 because slot 0 is reserved to the 1x1 white texture.
    s_data.textureSlotIndex = 1;

    // Increment the frame rendered count.
    s_data.frameCount++;
}

/**
 * @brief   End a scene.
 *          Calling this method makes the renderer draw the entire quad queue
 *          that was filled since the last call to Renderer2D::BeginScene.
 */
void Renderer2D::EndScene()
{
    BR_PROFILE_FUNCTION();

    // Get how many quads are in the queue.
    uint32_t dataSize =
      (uint32_t)((uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase);
    // Upload the queued quads data into the vertex buffer.
    s_data.vertexBuffer->SetData(s_data.quadVertexBufferBase, dataSize);

    // Draw all queued quads in a single call.
    Flush();
}


/**
 * @brief Bind all queued textures and render the queue.
 */
void Renderer2D::Flush()
{
    BR_PROFILE_FUNCTION();

    s_data.stats.drawCalls++;

    // Bind all active textures.
    for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
    {
        s_data.textureSlots[i]->Bind(i);
    }
    // Draw the entire vertex array.
    RenderCommand::DrawIndexed(s_data.vertexArray, s_data.quadIndexCount);
}

void Renderer2D::FlushAndReset()
{
    EndScene();

    // Reset the quad buffer.
    s_data.quadIndexCount      = 0;
    s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

    // Reset the texture buffer.
    // We set it to 1 instead of 0 because slot 0 is reserved to the 1x1 white texture.
    s_data.textureSlotIndex = 1;
}


/**
 * @brief   Get the number of frames that has been rendered since the beginning
 *          of the application.
 *
 * @return  long long The number of frames rendered since the beginning of the
 *          application.
 */
long long Renderer2D::GetFrameCount()
{
    return s_data.frameCount;
}

/* ------------------------------------------------------------------------- */
/* Primitives -------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

// ----- DRAW QUAD -----

void Renderer2D::DrawChar(const glm::vec2&      pos,
                          const glm::vec2&      size,
                          const Ref<Texture2D>& texture,
                          const glm::vec4&      tint)
{
    DrawChar(glm::vec3(pos.x, pos.y, 0.0f), size, texture, tint);
}

void Renderer2D::DrawChar(const glm::vec3&      pos,
                          const glm::vec2&      size,
                          const Ref<Texture2D>& texture,
                          const glm::vec4&      tint)
{
    BR_PROFILE_FUNCTION();

    constexpr size_t    quadVertexCount = 4;
    constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

    // If the quad queue is full:
    if (s_data.quadIndexCount >= Renderer2DData::maxIndices ||
        s_data.textureSlotIndex >= s_data.maxTextureSlots)
    {
        // Render the queue and start a new one.
        FlushAndReset();
    }

    float textureIndex = 0.0f;

    // Look up in the texture queue for the texture.
    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
    {
        if (*s_data.textureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // If the texture is not already in the queue:
    if (textureIndex == 0.0f)
    {
        // Add in to the queue.
        textureIndex                                 = (float)s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex] = texture;
        s_data.textureSlotIndex++;
    }

    glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    for (int i = 0; i < quadVertexCount; i++)
    {
        // Setup the vertex of the quad.
        s_data.quadVertexBufferPtr->position     = transform * s_data.quadVertexPosition[i];
        s_data.quadVertexBufferPtr->color        = tint;
        s_data.quadVertexBufferPtr->texCoord     = textureCoords[i];
        s_data.quadVertexBufferPtr->tilingFactor = glm::vec2(1.0f);
        s_data.quadVertexBufferPtr->texIndex     = textureIndex;
        s_data.quadVertexBufferPtr->isText       = 1;
        s_data.quadVertexBufferPtr++;
    }

    // A quad has 6 indices, increment the indices count by that many.
    s_data.quadIndexCount += 6;

    s_data.stats.quadCount++;
}

/**
 * @brief Queue a flat-colored quad in a 2D space.
 *
 * @param pos The world coordinates where to render the quad, (X, Y)
 * @param size The size of the quad, (X, Y)
 * @param color The color of the quad, (R, G, B, A)
 */
void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color)
{
    DrawQuad(glm::vec3(pos.x, pos.y, 0.0f), size, color);
}

/**
 * @brief Queue a flat-colored quad in a 3D space.
 *
 * @param pos The world coordinates where to render the quad, (X, Y, Z)
 * @param size The size of the quad, (X, Y)
 * @param color The color of the quad, (R, G, B, A)
 */
void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color)
{
    BR_PROFILE_FUNCTION();

    constexpr size_t    quadVertexCount = 4;
    constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

    // If the quad queue is full:
    if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
    {    // Render the queue and start a new one.
        FlushAndReset();
    }

    const float texIndex = 0.0f;    // White Texture.

    glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    for (int i = 0; i < quadVertexCount; i++)
    {
        // Setup the vertex of the quad.
        s_data.quadVertexBufferPtr->position     = transform * s_data.quadVertexPosition[i];
        s_data.quadVertexBufferPtr->color        = color;
        s_data.quadVertexBufferPtr->texCoord     = textureCoords[i];
        s_data.quadVertexBufferPtr->tilingFactor = {1.0f, 1.0f};
        s_data.quadVertexBufferPtr->texIndex     = texIndex;
        s_data.quadVertexBufferPtr->isText       = 0;
        s_data.quadVertexBufferPtr++;
    }

    // A quad has 6 indices, increment by that many for the next quad.
    s_data.quadIndexCount += 6;

    s_data.stats.quadCount++;
}

/**
 * @brief Queue a textured quad in a 2D space.
 *
 * @param pos The world coordinates where to render the quad, (X, Y)
 * @param size The size of the quad, (X, Y)
 * @param texture The texture to apply on that quad.
 * @param textScale The scaling factor of the texture, (X, Y)
 * @param tint A tint to apply to the texture, (R, G, B, A)
 */
void Renderer2D::DrawQuad(const glm::vec2&      pos,
                          const glm::vec2&      size,
                          const Ref<Texture2D>& texture,
                          const glm::vec2&      textScale,
                          const glm::vec4&      tint)
{
    DrawQuad({pos.x, pos.y, 0.0f}, size, texture, textScale, tint);
}

/**
 * @brief Queue a textured quad in a 3D space.
 *
 * @param pos The world coordinates where to render the quad, (X, Y, Z)
 * @param size The size of the quad, (X, Y)
 * @param texture The texture to apply on that quad.
 * @param textScale The scaling factor of the texture, (X, Y)
 * @param tint A tint to apply to the texture, (R, G, B, A)
 */
void Renderer2D::DrawQuad(const glm::vec3&      pos,
                          const glm::vec2&      size,
                          const Ref<Texture2D>& texture,
                          const glm::vec2&      textScale,
                          const glm::vec4&      tint)
{
    BR_PROFILE_FUNCTION();

    constexpr size_t    quadVertexCount = 4;
    constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

    // If the quad queue is full:
    if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
    {
        // Render the queue and start a new one.
        FlushAndReset();
    }

    float textureIndex = 0.0f;

    // Look up in the texture queue for the texture.
    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
    {
        if (*s_data.textureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // If the texture is not already in the queue:
    if (textureIndex == 0.0f)
    {
        // Add in to the queue.
        textureIndex                                 = (float)s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex] = texture;
        s_data.textureSlotIndex++;
    }

    glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    for (int i = 0; i < quadVertexCount; i++)
    {
        // Setup the vertex of the quad.
        s_data.quadVertexBufferPtr->position     = transform * s_data.quadVertexPosition[i];
        s_data.quadVertexBufferPtr->color        = tint;
        s_data.quadVertexBufferPtr->texCoord     = textureCoords[i];
        s_data.quadVertexBufferPtr->tilingFactor = textScale;
        s_data.quadVertexBufferPtr->texIndex     = textureIndex;
        s_data.quadVertexBufferPtr->isText       = 0;
        s_data.quadVertexBufferPtr++;
    }

    // A quad has 6 indices, increment the indices count by that many.
    s_data.quadIndexCount += 6;

    s_data.stats.quadCount++;
}

void Renderer2D::DrawQuad(const glm::vec2&         pos,
                          const glm::vec2&         size,
                          const Ref<SubTexture2D>& texture,
                          const glm::vec2&         textScale,
                          const glm::vec4&         tint)
{
    DrawQuad({pos.x, pos.y}, size, texture, textScale, tint);
}

void Renderer2D::DrawQuad(const glm::vec3&         pos,
                          const glm::vec2&         size,
                          const Ref<SubTexture2D>& texture,
                          const glm::vec2&         textScale,
                          const glm::vec4&         tint)
{
    BR_PROFILE_FUNCTION();

    constexpr size_t quadVertexCount = 4;
    const glm::vec2* textureCoords   = texture->GetTexCoords();

    // If the quad queue is full:
    if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
    {
        // Render the queue and start a new one.
        FlushAndReset();
    }

    float textureIndex = 0.0f;

    // Look up in the texture queue for the texture.
    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
    {
        if (*s_data.textureSlots[i].get() == *texture->GetTexture().get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // If the texture is not already in the queue:
    if (textureIndex == 0.0f)
    {
        // Add in to the queue.
        textureIndex                                 = (float)s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex] = texture->GetTexture();
        s_data.textureSlotIndex++;
    }

    glm::mat4 transform =
      glm::translate(glm::mat4(1.0f), pos) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    for (int i = 0; i < quadVertexCount; i++)
    {
        // Setup the vertex of the quad.
        s_data.quadVertexBufferPtr->position     = transform * s_data.quadVertexPosition[i];
        s_data.quadVertexBufferPtr->color        = tint;
        s_data.quadVertexBufferPtr->texCoord     = textureCoords[i];
        s_data.quadVertexBufferPtr->tilingFactor = textScale;
        s_data.quadVertexBufferPtr->texIndex     = textureIndex;
        s_data.quadVertexBufferPtr->isText       = 0;
        s_data.quadVertexBufferPtr++;
    }

    // A quad has 6 indices, increment the indicies count by that many.
    s_data.quadIndexCount += 6;

    s_data.stats.quadCount++;
}

// ----- DRAW ROTATED QUAD -----

/**
 * @brief Queued a flat colored rotated quad in 2D space.
 *
 * @param pos The world coordinates where to render the quad, (X, Y)
 * @param size The size of the quad, (X, Y)
 * @param color The color of the quad, (R, G, B, A)
 * @param rotation The rotation to apply to the quad, in degrees
 */
void Renderer2D::DrawRotatedQuad(const glm::vec2& pos,
                                 const glm::vec2& size,
                                 const glm::vec4& color,
                                 float            rotation)
{
    DrawRotatedQuad(glm::vec3(pos.x, pos.y, 0.0f), size, color, rotation);
}

/**
 * @brief Queued a flat colored rotated quad in 3D space.
 *
 * @param pos The world coordinates where to render the quad, (X, Y, Z)
 * @param size The size of the quad, (X, Y)
 * @param color The color of the quad, (R, G, B, A)
 * @param rotation The rotation to apply to the quad, in degrees
 */
void Renderer2D::DrawRotatedQuad(const glm::vec3& pos,
                                 const glm::vec2& size,
                                 const glm::vec4& color,
                                 float            rotation)
{
    BR_PROFILE_FUNCTION();

    constexpr size_t    quadVertexCount = 4;
    constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

    // If the quad queue is full:
    if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
    {
        // Render the queue and start a new one.
        FlushAndReset();
    }

    const float texIndex = 0.0f;    // White Texture.

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          glm::rotate(glm::mat4(1.0f), rotation, {0, 0, 1}) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    for (int i = 0; i < quadVertexCount; i++)
    {
        // Setup the vertex of the quad.
        s_data.quadVertexBufferPtr->position     = transform * s_data.quadVertexPosition[i];
        s_data.quadVertexBufferPtr->color        = color;
        s_data.quadVertexBufferPtr->texCoord     = textureCoords[i];
        s_data.quadVertexBufferPtr->tilingFactor = {1.0f, 1.0f};
        s_data.quadVertexBufferPtr->texIndex     = texIndex;
        s_data.quadVertexBufferPtr->isText       = 0;
        s_data.quadVertexBufferPtr++;
    }

    // A quad has 6 indices, increment the count by that many.
    s_data.quadIndexCount += 6;

    s_data.stats.quadCount++;
}

/**
 * @brief Queue a textured rotated quad in a 2D space.
 *
 * @param pos The world coordinates where to render the quad, (X, Y)
 * @param size The size of the quad, (X, Y)
 * @param texture The texture to apply on that quad.
 * @param textScale The scaling factor of the texture, (X, Y)
 * @param tint A tint to apply to the texture, (R, G, B, A)
 * @param rotation The rotation to apply to the quad, in degrees
 */
void Renderer2D::DrawRotatedQuad(const glm::vec2&      pos,
                                 const glm::vec2&      size,
                                 const Ref<Texture2D>& texture,
                                 const glm::vec2&      textScale,
                                 const glm::vec4&      tint,
                                 float                 rotation)
{
    DrawRotatedQuad({pos.x, pos.y, 0.0f}, size, texture, textScale, tint, rotation);
}

/**
 * @brief Queue a textured rotated quad in a 3D space.
 *
 * @param pos The world coordinates where to render the quad, (X, Y, Z)
 * @param size The size of the quad, (X, Y)
 * @param texture The texture to apply on that quad.
 * @param textScale The scaling factor of the texture, (X, Y)
 * @param tint A tint to apply to the texture, (R, G, B, A)
 * @param rotation The rotation to apply to the quad, in degrees
 */
void Renderer2D::DrawRotatedQuad(const glm::vec3&      pos,
                                 const glm::vec2&      size,
                                 const Ref<Texture2D>& texture,
                                 const glm::vec2&      textScale,
                                 const glm::vec4&      tint,
                                 float                 rotation)
{
    BR_PROFILE_FUNCTION();

    constexpr size_t    quadVertexCount = 4;
    constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};

    // If the quad queue is full:
    if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
    {
        // Render the queue and start a new one.
        FlushAndReset();
    }

    // Check if the texture is already in the texture queue.
    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
    {
        if (*s_data.textureSlots[i].get() == *texture.get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // If the texture isn't already in the texture queue:
    if (textureIndex == 0.0f)
    {
        // Add it to the queue.
        textureIndex                                 = (float)s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex] = texture;
        s_data.textureSlotIndex++;
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          glm::rotate(glm::mat4(1.0f), rotation, {0, 0, 1}) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});


    for (int i = 0; i < quadVertexCount; i++)
    {
        // Setup the vertex of the quad.
        s_data.quadVertexBufferPtr->position     = transform * s_data.quadVertexPosition[i];
        s_data.quadVertexBufferPtr->color        = tint;
        s_data.quadVertexBufferPtr->texCoord     = textureCoords[i];
        s_data.quadVertexBufferPtr->tilingFactor = textScale;
        s_data.quadVertexBufferPtr->texIndex     = textureIndex;
        s_data.quadVertexBufferPtr->isText       = 0;
        s_data.quadVertexBufferPtr++;
    }

    // A quad has 6 indices, increment the number of indices by that many.
    s_data.quadIndexCount += 6;

    s_data.stats.quadCount++;
}

void Renderer2D::DrawRotatedQuad(const glm::vec2&         pos,
                                 const glm::vec2&         size,
                                 const Ref<SubTexture2D>& texture,
                                 const glm::vec2&         textScale,
                                 const glm::vec4&         tint,
                                 float                    rotation)
{
    DrawRotatedQuad({pos.x, pos.y, 0.0f}, size, texture, textScale, tint, rotation);
}

void Renderer2D::DrawRotatedQuad(const glm::vec3&         pos,
                                 const glm::vec2&         size,
                                 const Ref<SubTexture2D>& texture,
                                 const glm::vec2&         textScale,
                                 const glm::vec4&         tint,
                                 float                    rotation)
{

    BR_PROFILE_FUNCTION();

    constexpr size_t quadVertexCount = 4;
    const glm::vec2* textureCoords   = texture->GetTexCoords();

    // If the quad queue is full:
    if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
    {
        // Render the queue and start a new one.
        FlushAndReset();
    }

    // Check if the texture is already in the texture queue.
    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
    {
        if (*s_data.textureSlots[i].get() == *texture->GetTexture().get())
        {
            textureIndex = (float)i;
            break;
        }
    }

    // If the texture isn't already in the texture queue:
    if (textureIndex == 0.0f)
    {
        // Add it to the queue.
        textureIndex                                 = (float)s_data.textureSlotIndex;
        s_data.textureSlots[s_data.textureSlotIndex] = texture->GetTexture();
        s_data.textureSlotIndex++;
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                          glm::rotate(glm::mat4(1.0f), rotation, {0, 0, 1}) *
                          glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});


    for (int i = 0; i < quadVertexCount; i++)
    {
        // Setup the vertex of the quad.
        s_data.quadVertexBufferPtr->position     = transform * s_data.quadVertexPosition[i];
        s_data.quadVertexBufferPtr->color        = tint;
        s_data.quadVertexBufferPtr->texCoord     = textureCoords[i];
        s_data.quadVertexBufferPtr->tilingFactor = textScale;
        s_data.quadVertexBufferPtr->texIndex     = textureIndex;
        s_data.quadVertexBufferPtr->isText       = 0;
        s_data.quadVertexBufferPtr++;
    }

    // A quad has 6 indices, increment the number of indices by that many.
    s_data.quadIndexCount += 6;

    s_data.stats.quadCount++;
}


Renderer2D::Statistics Renderer2D::GetStats()
{
    return s_data.stats;
}

void Renderer2D::ResetStats()
{
    memset(&s_data.stats, 0, sizeof(Statistics));
}

}    // namespace Brigerad

#include "brpch.h"
#include "Renderer2D.h"

#include "Brigerad/Renderer/VertexArray.h"
#include "Brigerad/Renderer/Shader.h"
#include "Brigerad/Renderer/RenderCommand.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Brigerad
{
struct Renderer2DStorage
{
    Ref<VertexArray> vertexArray;
    Ref<Shader> textureShader;
    Ref<Texture2D> whiteTexture;

    long long frameCount = 0;
};

static Renderer2DStorage* s_data;

void Renderer2D::Init()
{
    BR_PROFILE_FUNCTION();

    s_data = new Renderer2DStorage();

    s_data->vertexArray = VertexArray::Create();

    // clang-format off
    float squareVertices[5 * 4] = {
        //  X,     Y,    Z,   Tx,   Ty
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };
    // clang-format on

    BufferLayout squareLayout = { { ShaderDataType::Float3, "a_position" },
                                  { ShaderDataType::Float2, "a_TextCoord" } };

    uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

    Ref<VertexBuffer> squareVB =
        VertexBuffer::Create(squareVertices, sizeof(squareVertices));
    squareVB->SetLayout(squareLayout);
    s_data->vertexArray->AddVertexBuffer(squareVB);

    Ref<IndexBuffer> squareIB =
        IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(squareIndices[0]));
    s_data->vertexArray->SetIndexBuffer(squareIB);

    s_data->whiteTexture = Texture2D::Create(1, 1);
    uint32_t whiteTextureData = 0xFFFFFFFF;
    s_data->whiteTexture->SetData(&whiteTextureData, sizeof(whiteTextureData));

    s_data->textureShader = Shader::Create("assets/shaders/Texture.glsl");
    s_data->textureShader->Bind();
    s_data->textureShader->SetInt("u_Texture", 0);
}

void Renderer2D::Shutdown()
{
    BR_PROFILE_FUNCTION();
    delete s_data;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
    BR_PROFILE_FUNCTION();

    s_data->textureShader->Bind();
    s_data->textureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

    s_data->frameCount++;
}

void Renderer2D::EndScene()
{
}

long long Renderer2D::GetFrameCount()
{
    return s_data->frameCount;
}

// Primitives
void Renderer2D::DrawQuad(const glm::vec2& pos, const glm::vec2& size, const glm::vec4& color, float rotation)
{
    DrawQuad(glm::vec3(pos.x, pos.y, 0.0f), size, color, rotation);
}

void Renderer2D::DrawQuad(const glm::vec3& pos, const glm::vec2& size, const glm::vec4& color, float rotation)
{
    BR_PROFILE_FUNCTION();
    s_data->textureShader->SetFloat4("u_Color", color);
    s_data->whiteTexture->Bind();

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), pos) *
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    s_data->textureShader->SetMat4("u_Transform", transform);

    s_data->vertexArray->Bind();
    RenderCommand::DrawIndexed(s_data->vertexArray);
}

void Renderer2D::DrawQuad(const glm::vec2& pos,
                          const glm::vec2& size,
                          const Ref<Texture>& texture,
                          const glm::vec2& textScale,
                          const glm::vec4& tint,
                          float rotation)
{
    DrawQuad({ pos.x, pos.y, 0.0f }, size, texture, textScale, tint, rotation);
}

void Renderer2D::DrawQuad(const glm::vec3& pos,
                          const glm::vec2& size,
                          const Ref<Texture>& texture,
                          const glm::vec2& textScale,
                          const glm::vec4& tint,
                          float rotation)
{
    BR_PROFILE_FUNCTION();

    s_data->textureShader->Bind();
    s_data->textureShader->SetFloat4("u_Color", tint);
    s_data->textureShader->SetFloat2("u_TextureScale", textScale);

    glm::mat4 transform =
        glm::translate(glm::mat4(1.0f), pos) *
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0, 0, 1)) *
        glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
    s_data->textureShader->SetMat4("u_Transform", transform);

    texture->Bind();

    s_data->vertexArray->Bind();
    RenderCommand::DrawIndexed(s_data->vertexArray);
}


}  // namespace Brigerad
#pragma once

#include "Brigerad/Renderer/OrthographicCamera.h"
#include "Brigerad/Renderer/Texture.h"


namespace Brigerad
{
class Renderer2D
{
    public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();

    static long long GetFrameCount();

    // Primitives
    static void DrawQuad(const glm::vec2& pos,
                         const glm::vec2& size,
                         const glm::vec4& color,
                         float rotation = 0);
    static void DrawQuad(const glm::vec3& pos,
                         const glm::vec2& size,
                         const glm::vec4& color,
                         float rotation = 0);
    static void DrawQuad(const glm::vec2& pos,
                         const glm::vec2& size,
                         const Ref<Texture>& texture,
                         const glm::vec2& textScale = glm::vec2(1.0f),
                         const glm::vec4& tint      = glm::vec4(1.0f),
                         float rotation             = 0);
    static void DrawQuad(const glm::vec3& pos,
                         const glm::vec2& size,
                         const Ref<Texture>& texture,
                         const glm::vec2& textScale = glm::vec2(1.0f),
                         const glm::vec4& tint      = glm::vec4(1.0f),
                         float rotation             = 0);
};
}  // namespace Brigerad
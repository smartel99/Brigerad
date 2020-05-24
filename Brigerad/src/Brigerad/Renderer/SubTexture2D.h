#pragma once

#include <glm/glm.hpp>
#include "Brigerad/Core/Core.h"
#include "Brigerad/Renderer/Texture.h"

namespace Brigerad
{
class SubTexture2D
{
    public:
    SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

    const Ref<Texture2D> GetTexture() const { return m_texture; }
    const glm::vec2* GetTexCoords() const { return m_texCoords; }

    static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture,
                                              const glm::vec2& pos,
                                              const glm::vec2& cellSize,
                                              const glm::vec2& spriteSize = { 1, 1 });

    private:
    Ref<Texture2D> m_texture;

    glm::vec2 m_texCoords[4];
};
}  // namespace Brigerad
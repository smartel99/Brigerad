#include "brpch.h"
#include "Text.h"

#include "Brigerad/Renderer/Texture.h"


struct UITextData
{
    glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };  // Defaults to white.
    Brigerad::UI::TextAlignment Alignment = Brigerad::UI::TextAlignment::Left;  // Defaults to left-aligned.
    float FontSize = 14.0f;  // Defaults to 14x14 pixels.
};
static UITextData s_uiTextData;

static Brigerad::Ref<Brigerad::Texture2D> AssembleTextIntoTexture(const char* text);
static uint32_t FindNumberOfLines(const char* text);


namespace Brigerad
{
namespace UI
{
    class TextObject
    {
        public:
        TextObject(const char* text) : m_text(text)
        {
            m_texture = AssembleTextIntoTexture(m_text);
        }

        private:
        const char* m_text;
        Ref<Texture2D> m_texture;
    };

}  // namespace UI
}  // namespace Brigerad

Brigerad::Ref<Brigerad::Texture2D> AssembleTextIntoTexture(const char* text)
{
    uint32_t width  = strlen(text) * s_uiTextData.FontSize;
    uint32_t height = FindNumberOfLines(text) * s_uiTextData.FontSize;

    Brigerad::Ref<Brigerad::Texture2D> texture =
      Brigerad::Texture2D::Create(width, height);
}


uint32_t FindNumberOfLines(const char* text) { size_t len = strlen(text); }
#include "brpch.h"
#include "Text.h"

#include "Brigerad/Renderer/Texture.h"
#include "Brigerad/Renderer/Renderer2D.h"

#include "arial.h"

#include "ft2build.h"
#include "glad/glad.h"
#include FT_FREETYPE_H
#include FT_BITMAP_H


struct Character
{
    Brigerad::Ref<Brigerad::Texture2D> Texture;
    glm::ivec2 Size = { 0, 0 };    // Size of glyph.
    glm::ivec2 Bearing = { 0, 0 }; // Offset from baseline to left/top of glyph
    uint32_t Advance = 0;   // Offset to advance to next glyph
};


struct UITextData
{
    glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };  // Defaults to white.
    Brigerad::UI::TextAlignment Alignment = Brigerad::UI::TextAlignment::Left;  // Defaults to left-aligned.
    float FontSize = 14.0f;  // Defaults to 14x14 pixels.

    FT_Library FontLib = {};
    FT_Face CurrentFont = {};
    std::map<char, Character> Characters;
};
static UITextData s_uiTextData;



namespace Brigerad
{
namespace UI
{

void InitFont()
{
    if (FT_Init_FreeType(&s_uiTextData.FontLib))
    {
        BR_ERROR("Could not init FreeType Library!");
    }

    if (FT_New_Memory_Face(s_uiTextData.FontLib, (uint8_t*)arialData, arialDataSize, 0, &s_uiTextData.CurrentFont))
    {
        BR_ERROR("Could not load font!");
    }

    FT_Set_Pixel_Sizes(s_uiTextData.CurrentFont, 0, 128);

    if (FT_Load_Char(s_uiTextData.CurrentFont, 'X', FT_LOAD_RENDER))
    {
        BR_ERROR("Failed to load glyph!");
    }


    for (uint8_t c = 0; c < 128; c++)
    {
        // Load character glyph.
        if (FT_Load_Char(s_uiTextData.CurrentFont, c, FT_LOAD_COLOR | FT_LOAD_RENDER))
        {
            BR_ERROR("Unable to load glyph of character {0}", c);
            continue;
        }

        const FT_Bitmap& data = s_uiTextData.CurrentFont->glyph->bitmap;

        Ref<Texture2D> texture = Texture2D::Create(data.width, data.rows, 4);
        size_t size = (size_t)data.width * (size_t)data.rows;
        uint32_t* glyphData = new uint32_t[size];

        for (size_t i = 0; i < size; i++)
        {
            glyphData[i] = (uint32_t)data.buffer[i];
        }

        texture->SetData(glyphData, size * sizeof(uint32_t));
        delete[] glyphData;

        // Now store character for later use.
        Character character = {
            texture,
            glm::ivec2(data.width, data.rows),
            glm::ivec2(s_uiTextData.CurrentFont->glyph->bitmap_left, s_uiTextData.CurrentFont->glyph->bitmap_top),
            (uint32_t)s_uiTextData.CurrentFont->glyph->advance.x };
        s_uiTextData.Characters.insert(std::pair<char, Character>(c, character));
    }


    // Clear up resources since we're done.
//     FT_Done_Face(s_uiTextData.CurrentFont);
//     FT_Done_FreeType(s_uiTextData.FontLib);
}


void TextUnformatted(const glm::vec2& pos, const std::string& text)
{
    TextUnformatted(glm::vec3(pos.x, pos.y, 0.0f), text);
}

void TextUnformatted(const glm::vec3& pos, const std::string& text)
{
    BR_PROFILE_FUNCTION();
    static float scale = s_uiTextData.FontSize / 128;
    float x = pos.x;
    float y = pos.y;

    for (const auto& c : text)
    {
        Character character = s_uiTextData.Characters[c];

        glm::vec2 cPos = { x + character.Bearing.x * scale,
                           y - (character.Size.y - character.Bearing.y) * scale };

        glm::vec2 size = { character.Size.x * scale,
                          character.Size.y * scale };

        Renderer2D::DrawChar(cPos, size, character.Texture);

        // Bit shift by 6 to get value in pixels (2^6 = 64).
        x += (character.Advance >> 6) * scale;
    }
}

}  // namespace UI
}  // namespace Brigerad


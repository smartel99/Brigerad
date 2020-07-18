#include "brpch.h"
#include "Text.h"

#include "Brigerad/Renderer/Texture.h"
#include "Brigerad/Renderer/Renderer2D.h"

#include "arial.h"
#include "arialbd.h"
#include "arialbi.h"
#include "ariali.h"

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_BITMAP_H


struct Character
{
    Brigerad::Ref<Brigerad::Texture2D> Texture;
    glm::ivec2                         Size = {0, 0};    // Size of glyph.
    glm::ivec2 Bearing = {0, 0};    // Offset from baseline to left/top of glyph
    uint32_t   Advance = 0;         // Offset to advance to next glyph
};


struct UITextData
{
    glm::vec4                   Color = {1.0f, 1.0f, 1.0f, 1.0f};    // Defaults to white.
    Brigerad::UI::TextAlignment Alignment =
      Brigerad::UI::TextAlignment::Left;    // Defaults to left-aligned.
    float FontSize = 14.0f;                 // Defaults to 14x14 pixels.

    FT_Library                                       FontLib     = {};
    std::map<const char*, std::map<char, Character>> LoadedFonts = {
      {"arial", {}}, {"arialbd", {}}, {"arialbi", {}}, {"ariali", {}}};
    std::map<char, Character>* CurrentFont     = &LoadedFonts["arial"];
    const char*                CurrentFontName = "arial";
};
static UITextData s_uiTextData;



namespace Brigerad
{
namespace UI
{
static void LoadFont(const char* fontname, const FT_Face& font);

void InitFont()
{
    static const std::map<const char*, std::pair<size_t, const uint32_t*>> arialFonts = {
      {"arial", {arialDataSize, arialData}},
      {"arialbd", {arialBbDataSize, arialBbData}},
      {"arialbi", {arialBiDataSize, arialBiData}},
      {"ariali", {arialBiDataSize, arialIData}}};

    FT_Face currentFont = {};

    if (FT_Init_FreeType(&s_uiTextData.FontLib))
    {
        BR_ERROR("Could not init FreeType Library!");
    }

    for (const auto& [fontName, font] : arialFonts)
    {
        if (FT_New_Memory_Face(
              s_uiTextData.FontLib, (uint8_t*)font.second, font.first, 0, &currentFont))
        {
            BR_ERROR("Could not load font!");
        }

        FT_Set_Pixel_Sizes(currentFont, 0, 14);

        LoadFont(fontName, currentFont);

        FT_Done_Face(currentFont);
    }

    auto res = FT_New_Face(s_uiTextData.FontLib, "assets/fonts/Baloo2-Bold.ttf", 0, &currentFont);
    if (res != 0)
    {
        BR_ERROR("Could not load font: {0:x}", res);
    }

    FT_Set_Pixel_Sizes(currentFont, 0, 128);
    LoadFont("Baloo2-Bold", currentFont);

    s_uiTextData.CurrentFont = &s_uiTextData.LoadedFonts["Baloo2-Bold"];

    // Clear up resources since we're done.
    //     FT_Done_FreeType(s_uiTextData.FontLib);
}


void TextUnformatted(const glm::vec2& pos, const std::string& text)
{
    TextUnformatted(glm::vec3(pos.x, pos.y, 0.0f), text);
}

void TextUnformatted(const glm::vec3& pos, const std::string& text)
{
    BR_PROFILE_FUNCTION();
    static const float scale = 14.f / 128.0f;
    float              x     = pos.x;
    float              y     = pos.y;

    for (const auto& c : text)
    {
        auto      char_it   = s_uiTextData.CurrentFont->find(c);
        Character character = char_it != s_uiTextData.CurrentFont->end()
                                ? char_it->second
                                : s_uiTextData.CurrentFont->at('#');

        glm::vec2 cPos = {x + (character.Bearing.x * scale),
                          y + ((character.Size.y - character.Bearing.y) * scale)};

        if (cPos.x <= x)
        {
            //             BR_WARN("Char '{0}' has smaller X than previous char! ({1} vs {2})", c,
            //             cPos.x, x);
        }

        glm::vec2 size = {character.Size.x * scale, character.Size.y * scale};

        Renderer2D::DrawChar(cPos, size, character.Texture, s_uiTextData.Color);

        // Bit shift by 6 to get value in pixels (2^6 = 64).
        x += ((float)character.Advance / 64.0f) * scale;
        //         x = cPos.x + (character.Size.x * scale);
    }
}

void LoadFont(const char* fontName, const FT_Face& font)
{
    std::map<char, Character> loadedFontData;

    for (uint8_t c = 0; c < 128; c++)
    {
        FT_UInt err = FT_Load_Char(font, c, FT_LOAD_COLOR | FT_LOAD_RENDER);
        if (err != 0)
        {
            BR_CORE_ERROR("Unable to get index of character {0}: {1:x}!", (char)c, err);
            continue;
        }

        const FT_Bitmap& data = font->glyph->bitmap;

        Ref<Texture2D> texture   = Texture2D::Create(data.width, data.rows, 4);
        size_t         size      = (size_t)data.width * (size_t)data.rows;
        uint32_t*      glyphData = new uint32_t[size];

        for (size_t i = 0; i < size; i++)
        {
            glyphData[i] = (uint32_t)data.buffer[i];
        }

        texture->SetData(glyphData, size * sizeof(uint32_t));
        delete[] glyphData;

        // Now store character for later use.
        Character character = {texture,
                               glm::ivec2(font->glyph->bitmap.width, font->glyph->bitmap.rows),
                               glm::ivec2(font->glyph->bitmap_left, font->glyph->bitmap_top),
                               (uint32_t)font->glyph->advance.x};
        loadedFontData.insert(std::pair<char, Character>(c, character));
    }

    s_uiTextData.LoadedFonts[fontName] = loadedFontData;
}

}    // namespace UI
}    // namespace Brigerad

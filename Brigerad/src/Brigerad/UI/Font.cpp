/**
 * @file   D:\dev\Brigerad\Brigerad\src\Brigerad\UI\Font.cpp
 * @author Samuel Martel
 * @date   2020/06/06
 *
 * @brief  Source for the Font module.
 */
#include "brpch.h"
#include "Font.h"
#include "Brigerad/Core/Memory.h"
#include "Brigerad/Core/File.h"
#include "stbTypes.h"
#include "imstb_truetype.h"



namespace Brigerad
{
namespace UI
{
static unsigned int stb_decompress_length(const unsigned char* input);
static unsigned int stb_decompress(unsigned char* output,
                                   const unsigned char* input, unsigned int length);
static const char* GetDefaultCompressedFontDataTTFBase85();
static unsigned int Decode85Byte(char c);
static void Decode85(const unsigned char* src, unsigned char* dst);
static bool FontAtlasBuildWithStbTrueType(Brigerad::UI::FontAtlas* atlas);
static void FontAtlasBuildRegisterDefaultCustomRects(Brigerad::UI::FontAtlas* atlas);
static void UnpackBoolVectorToFlatIndexList(const BrBoolVector* in, std::vector<int>* out);

//-----------------------------------------------------------------------------
// [SECTION] ImFontConfig
//-----------------------------------------------------------------------------
FontConfig::FontConfig()
{
    FontData = nullptr;
    FontDataSize = 0;
    FontDataOwnedByAtlas = true;
    FontNo = 0;
    SizePixels = 0.0f;
    OversampleH = 3;
    OversampleV = 1;
    PixelSnapH = false;
    GlyphExtraSpacing = Vec2(0.0f, 0.0f);
    GlyphOffset = Vec2(0.0f, 0.0f);
    GlyphRanges = nullptr;
    GlyphMinAdvanceX = 0.0f;
    GlyphMaxAdvanceX = FLT_MAX;
    MergeMode = false;
    RasterizerFlags = 0x00;
    RasterizerMultiply = 1.0f;
    memset(Name, 0, sizeof(Name));
    DstFont = nullptr;
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontGlyphRangesBuilder
//-----------------------------------------------------------------------------

void FontGlyphRangesBuilder::AddText(const char* text, const char* textEnd /*= nullptr*/)
{
    while (textEnd ? (text < textEnd) : *text)
    {
        unsigned int c = 0;
        int cLen = TextCharFromUtf8(&c, text, textEnd);
        text += cLen;
        if (cLen == 0)
        {
            break;
        }
        if (c < 0x10000)
        {
            AddChar((uint16_t)c);
        }
    }
}


void FontGlyphRangesBuilder::AddRanges(const uint16_t* ranges)
{
    for (; ranges[0]; ranges += 2)
    {
        for (uint16_t c = ranges[0]; c < ranges[1]; c++)
        {
            AddChar(c);
        }
    }
}


void FontGlyphRangesBuilder::BuildRanges(std::vector<uint16_t>* outRanges)
{
    int maxCodepoint = 0x10000;
    for (int n = 0; n < maxCodepoint; n++)
    {
        if (GetBit(n))
        {
            outRanges->emplace_back((uint16_t)n);
            while (n < maxCodepoint - 1 && GetBit(n + 1))
            {
                n++;
            }
            outRanges->emplace_back((uint16_t)n);
        }
    }
    outRanges->emplace_back(0);
}

//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas
//-----------------------------------------------------------------------------

// A work of art lies ahead! (. = white layer, X = black layer, others are blank)
// The white texels on the top left are the ones we'll use everywhere to render filled shapes.
const int FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF = 108;
const int FONT_ATLAS_DEFAULT_TEX_DATA_H = 27;
const unsigned int FONT_ATLAS_DEFAULT_TEX_DATA_ID = 0x80000000;
static const char FONT_ATLAS_DEFAULT_TEX_DATA_PIXELS[FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * FONT_ATLAS_DEFAULT_TEX_DATA_H + 1] =
{
    "..-         -XXXXXXX-    X    -           X           -XXXXXXX          -          XXXXXXX-     XX          "
    "..-         -X.....X-   X.X   -          X.X          -X.....X          -          X.....X-    X..X         "
    "---         -XXX.XXX-  X...X  -         X...X         -X....X           -           X....X-    X..X         "
    "X           -  X.X  - X.....X -        X.....X        -X...X            -            X...X-    X..X         "
    "XX          -  X.X  -X.......X-       X.......X       -X..X.X           -           X.X..X-    X..X         "
    "X.X         -  X.X  -XXXX.XXXX-       XXXX.XXXX       -X.X X.X          -          X.X X.X-    X..XXX       "
    "X..X        -  X.X  -   X.X   -          X.X          -XX   X.X         -         X.X   XX-    X..X..XXX    "
    "X...X       -  X.X  -   X.X   -    XX    X.X    XX    -      X.X        -        X.X      -    X..X..X..XX  "
    "X....X      -  X.X  -   X.X   -   X.X    X.X    X.X   -       X.X       -       X.X       -    X..X..X..X.X "
    "X.....X     -  X.X  -   X.X   -  X..X    X.X    X..X  -        X.X      -      X.X        -XXX X..X..X..X..X"
    "X......X    -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -         X.X   XX-XX   X.X         -X..XX........X..X"
    "X.......X   -  X.X  -   X.X   -X.....................X-          X.X X.X-X.X X.X          -X...X...........X"
    "X........X  -  X.X  -   X.X   - X...XXXXXX.XXXXXX...X -           X.X..X-X..X.X           - X..............X"
    "X.........X -XXX.XXX-   X.X   -  X..X    X.X    X..X  -            X...X-X...X            -  X.............X"
    "X..........X-X.....X-   X.X   -   X.X    X.X    X.X   -           X....X-X....X           -  X.............X"
    "X......XXXXX-XXXXXXX-   X.X   -    XX    X.X    XX    -          X.....X-X.....X          -   X............X"
    "X...X..X    ---------   X.X   -          X.X          -          XXXXXXX-XXXXXXX          -   X...........X "
    "X..X X..X   -       -XXXX.XXXX-       XXXX.XXXX       -------------------------------------    X..........X "
    "X.X  X..X   -       -X.......X-       X.......X       -    XX           XX    -           -    X..........X "
    "XX    X..X  -       - X.....X -        X.....X        -   X.X           X.X   -           -     X........X  "
    "      X..X          -  X...X  -         X...X         -  X..X           X..X  -           -     X........X  "
    "       XX           -   X.X   -          X.X          - X...XXXXXXXXXXXXX...X -           -     XXXXXXXXXX  "
    "------------        -    X    -           X           -X.....................X-           ------------------"
    "                    ----------------------------------- X...XXXXXXXXXXXXX...X -                             "
    "                                                      -  X..X           X..X  -                             "
    "                                                      -   X.X           X.X   -                             "
    "                                                      -    XX           XX    -                             "
};



static const Vec2 FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[size_t(MouseCursor::COUNT)][3] =
{
    // Pos ........ Size ......... Offset ......
    { Vec2(0,3), Vec2(12,19), Vec2(0, 0) }, // MouseCursor_Arrow
    { Vec2(13,0), Vec2(7,16), Vec2(1, 8) }, // MouseCursor_TextInput
    { Vec2(31,0), Vec2(23,23), Vec2(11,11) }, // MouseCursor_ResizeAll
    { Vec2(21,0), Vec2(9,23), Vec2(4,11) }, // MouseCursor_ResizeNS
    { Vec2(55,18),Vec2(23, 9), Vec2(11, 4) }, // MouseCursor_ResizeEW
    { Vec2(73,0), Vec2(17,17), Vec2(8, 8) }, // MouseCursor_ResizeNESW
    { Vec2(55,0), Vec2(17,17), Vec2(8, 8) }, // MouseCursor_ResizeNWSE
    { Vec2(91,0), Vec2(17,22), Vec2(5, 0) }, // MouseCursor_Hand
};

FontAtlas::FontAtlas()
    : m_locked(false),
    m_flags(FontAtlasFlags::None),
    m_texID(uint32_t(nullptr)),
    m_texDesiredWidth(0),
    m_texGlyphPadding(1),
    m_texPixelsAlpha8(nullptr),
    m_texPixelsRGBA32(nullptr),
    m_texWidth(0), m_texHeight(0),
    m_texUvScale(Vec2(0.0f, 0.0f)),
    m_texUvWhitePixel(Vec2(0.0f, 0.0f))
{
    for (int& customRectId : m_customRectIds)
    {
        customRectId = -1;
    }
}


FontAtlas::~FontAtlas()
{
    BR_CORE_ASSERT(!m_locked,
                   "Cannot modify a locked FontAtlas between the start and the endo of a frame!");
    Clear();
}


Brigerad::UI::Font* FontAtlas::AddFont(const FontConfig* fontCfg)
{
    BR_CORE_ASSERT(!m_locked,
                   "Cannot modify a locked FontAtlas between the start and the endo of a frame!");
    BR_CORE_ASSERT(fontCfg->FontData != nullptr && fontCfg->FontDataSize > 0,
                   "Invalid configuration!");
    BR_CORE_ASSERT(fontCfg->SizePixels > 0.0f, "Font size must be >0!");

    // Create new font.
    if (!fontCfg->MergeMode)
    {
        m_fonts.emplace_back(BR_NEW(Font));
    }
    else
    {
        // When using merge mode, make sure that a font has already been added before.
        // You can use Brigerad::UI::Fonts->AddFontDefault() to add the default font.
        BR_CORE_ASSERT(!m_fonts.empty(),
                       "Cannot use MergeMode for the first font!");
    }

    m_configData.emplace_back(*fontCfg);
    FontConfig& newFontCfg = m_configData.back();
    if (newFontCfg.DstFont == nullptr)
    {
        newFontCfg.DstFont = m_fonts.back();
    }
    if (!newFontCfg.FontDataOwnedByAtlas)
    {
        newFontCfg.FontData = BR_ALLOC(newFontCfg.FontDataSize);
        newFontCfg.FontDataOwnedByAtlas = true;
        memcpy(newFontCfg.FontData, fontCfg->FontData, (size_t)newFontCfg.FontDataSize);
    }

    // Invalidate Texture.
    ClearTexData();
    return newFontCfg.DstFont;
}


Brigerad::UI::Font* FontAtlas::AddFontDefault(const FontConfig* fontCfg /*= nullptr*/)
{
    FontConfig cfg = fontCfg ? *fontCfg : FontConfig();

    if (!fontCfg)
    {
        cfg.OversampleH = cfg.OversampleV = 1;
        cfg.PixelSnapH = true;
    }
    if (cfg.SizePixels <= 0.0f)
    {
        cfg.SizePixels = 13.0f;
    }
    if (cfg.Name[0] == '\0')
    {
        sprintf_s(cfg.Name, sizeof(cfg.Name), "ProggyClean.ttf, %dpx", (int)cfg.SizePixels);
    }

    const char* ttfCompressedBase85 = GetDefaultCompressedFontDataTTFBase85();
    const uint16_t* glyphRanges = cfg.GlyphRanges != nullptr ?
        cfg.GlyphRanges : GetGlyphRangesDefault();

    Font* font = AddFontFromMemoryCompressedBase85TTF(ttfCompressedBase85,
                                                      cfg.SizePixels, &cfg, glyphRanges);
    font->m_displayOffset.y = 1.0f;
    return font;

}


Brigerad::UI::Font* FontAtlas::AddFontFromFileTTF(const char* filename,
                                                  float sizePixels,
                                                  const FontConfig* fontCfgTemplate,
                                                  const uint16_t* glyphRanges /*= nullptr*/)
{
    BR_CORE_ASSERT(!m_locked, "Cannot modify a locked FontAtlas between the beginning and "
                   " the end of a frame!");
    size_t dataSize = 0;
    void* data = FileLoadToMemory(filename, "rb", &dataSize, 0);

    if (!data)
    {
        BR_CORE_ASSERT(false, "Could not load file!");
        return nullptr;
    }

    FontConfig fontCfg = fontCfgTemplate ? *fontCfgTemplate : FontConfig();
    if (fontCfg.Name[0] == '\0')
    {
        // Store a short copy of the filename into the font name for convenience.
        const char* p;
        for (p = filename + strlen(filename);
             p > filename && p[-1] != '/' && p[-1] != '\\';
             p--)
        {
        }
        sprintf_s(fontCfg.Name, sizeof(fontCfg.Name), "%s, %.0fpx", p, sizePixels);
    }
    return AddFontFromMemoryTTF(data, (int)dataSize, sizePixels, &fontCfg, glyphRanges);
}


Brigerad::UI::Font* FontAtlas::AddFontFromMemoryTTF(void* fontData,
                                                    int fontSize,
                                                    float sizePixels,
                                                    const FontConfig* fontCfgTemplate,
                                                    const uint16_t* glyphRanges)
{
    BR_CORE_ASSERT(!m_locked, "Cannot modify a locked FontAtlas between the beginning and "
                   " the end of a frame!");
    FontConfig fontCfg = fontCfgTemplate ? *fontCfgTemplate : FontConfig();
    BR_CORE_ASSERT(fontCfg.FontData == nullptr, "Font Data is not null!");
    fontCfg.FontData = fontData;
    fontCfg.FontDataSize = fontSize;
    fontCfg.SizePixels = sizePixels;
    if (glyphRanges)
    {
        fontCfg.GlyphRanges = glyphRanges;
    }
    return AddFont(&fontCfg);
}


Brigerad::UI::Font* FontAtlas::AddFontFromMemoryCompressedTTF(const void* compressedFontData,
                                                              int compressedFontSize,
                                                              float sizePixels,
                                                              const FontConfig* fontCfgTemplate,
                                                              const uint16_t* glyphRanges)
{
    const unsigned int bufDecompressedSize =
        stb_decompress_length((const unsigned char*)compressedFontData);
    unsigned char* bufDecompressedData = (unsigned char*)BR_ALLOC(bufDecompressedSize);
    stb_decompress(bufDecompressedData,
                   (const unsigned char*)compressedFontData,
                   (unsigned int)compressedFontSize);

    FontConfig fontCfg = fontCfgTemplate ? *fontCfgTemplate : FontConfig();
    BR_CORE_ASSERT(fontCfg.FontData == nullptr, "Font data is not NULL!");
    fontCfg.FontDataOwnedByAtlas = true;
    return AddFontFromMemoryTTF(bufDecompressedData, (int)bufDecompressedSize,
                                sizePixels, &fontCfg, glyphRanges);
}


Brigerad::UI::Font* FontAtlas::AddFontFromMemoryCompressedBase85TTF(const char* compressedFontDataBase85,
                                                                    float sizePixels,
                                                                    const FontConfig* fontCfg,
                                                                    const uint16_t* glyphRange)
{
    int compressedTtfSize = (((int)strlen(compressedFontDataBase85) + 4) / 5) * 4;
    void* compressedTtf = BR_ALLOC((size_t)compressedTtfSize);
    Decode85((const unsigned char*)compressedFontDataBase85, (unsigned char*)compressedTtf);
    Font* font = AddFontFromMemoryCompressedTTF(compressedTtf, compressedTtfSize,
                                                sizePixels, fontCfg, glyphRange);
    BR_FREE(compressedTtf);
    return font;
}


void FontAtlas::ClearInputData()
{
    BR_CORE_ASSERT(!m_locked,
                   "Cannot modify a locked FontAtlas between the start and the end of a frame!");

    for (auto& i : m_configData)
    {
        if (i.FontData && i.FontDataOwnedByAtlas)
        {
            free(i.FontData);
            i.FontData = nullptr;
        }
    }

    // When clearing this we lose access to the font name
    // and other information used to build the font.
    for (auto& font : m_fonts)
    {
        if (font->m_configData >= m_configData.data() && font->m_configData < m_configData.data() + m_configData.size())
        {
            font->m_configData = nullptr;
            font->m_configDataCount = 0;
        }
    }
    m_configData.clear();
    m_customRects.clear();
    for (auto& rectId : m_customRectIds)
    {
        rectId = -1;
    }
}


void FontAtlas::ClearTexData()
{
    BR_CORE_ASSERT(!m_locked,
                   "Cannot modify a locked FontAtlas between the start and the end of a frame!");

    if (m_texPixelsAlpha8)
    {
        free(m_texPixelsAlpha8);
    }
    if (m_texPixelsRGBA32)
    {
        free(m_texPixelsRGBA32);
    }
    m_texPixelsAlpha8 = nullptr;
    m_texPixelsRGBA32 = nullptr;
}


void FontAtlas::ClearFonts()
{
    BR_CORE_ASSERT(!m_locked,
                   "Cannot modify a locked FontAtlas between the start and the end of a frame!");
    for (auto& font : m_fonts)
    {
        free(static_cast<void*>(font));
    }
    m_fonts.clear();
}


void FontAtlas::Clear()
{
    ClearInputData();
    ClearTexData();
    ClearFonts();
}


bool FontAtlas::Build()
{
    BR_CORE_ASSERT(!m_locked,
                   "Cannot modify a locked FontAtlas between the start and the end of a frame!");
    return FontAtlasBuildWithStbTrueType(this);
}


void FontAtlas::GetTexDataAsAlpha8(unsigned char** outPixels, int* outWidth, int* outHeight, int* outBPP /*= nullptr*/)
{
    // Build atlas on demand.
    if (m_texPixelsAlpha8 == nullptr)
    {
        if (m_configData.empty())
        {
            AddFontDefault();
        }
        Build();
    }

    *outPixels = m_texPixelsAlpha8;
    if (outWidth)
    {
        *outWidth = m_texWidth;
    }
    if (outHeight)
    {
        *outHeight = m_texHeight;
    }
    if (outBPP)
    {
        *outBPP = 1;
    }

}


void FontAtlas::GetTexDataAsRGBA32(unsigned char** outPixels, int* outWidth, int* outHeight, int* outBPP /*= nullptr*/)
{
    // Convert to RGBA32 format on demand.
    // Although it is likely to be the most commonly used format,
    // our font rendering is 1 channel / 8bpp.
    if (m_texPixelsRGBA32 == nullptr)
    {
        unsigned char* pixels = nullptr;
        GetTexDataAsAlpha8(&pixels, nullptr, nullptr);
        if (pixels)
        {
            m_texPixelsRGBA32 = (unsigned int*)malloc(
                (size_t)m_texWidth * (size_t)m_texHeight * 4);
            const unsigned char* src = pixels;
            unsigned int* dst = m_texPixelsRGBA32;
            for (int n = m_texWidth * m_texHeight; n > 0; n--)
            {
                *dst++ = BR_COL32(255, 255, 255, (unsigned int)(*src++));
            }
        }
    }

    *outPixels = (unsigned char*)m_texPixelsRGBA32;
    if (outWidth)
    {
        *outWidth = m_texWidth;
    }
    if (outHeight)
    {
        *outHeight = m_texHeight;
    }
    if (outBPP)
    {
        *outBPP = 4;
    }
}


uint16_t* FontAtlas::GetGlyphRangesDefault()
{

}


uint16_t* FontAtlas::GetGlyphRangesKorean()
{

}


uint16_t* FontAtlas::GetGlyphRangesJapanese()
{

}


uint16_t* FontAtlas::GetGlyphRangesChineseFull()
{

}


uint16_t* FontAtlas::GetGlyphRangesChineseSimplifiedCommon()
{

}


uint16_t* FontAtlas::GetGlyphRangesCyrillic()
{

}


uint16_t* FontAtlas::GetGlyphRangesThai()
{

}


uint16_t* FontAtlas::GetGlyphRangesVietnamese()
{

}


int FontAtlas::AddCustomRectRegular(unsigned int id, int width, int height)
{
    BR_CORE_ASSERT(id >= 0x10000, "ID < 0x10000 are reserved and cannot be used!");
    BR_CORE_ASSERT(width > 0 && width <= 0xFFFF, "Invalid width value!");
    BR_CORE_ASSERT(height > 0 && height <= 0xFFFF, "Invalid height value!");
    FontAtlasCustomRect r;
    r.ID = id;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    m_customRects.emplace_back(r);
    return m_customRects.size() - 1; // Index of the new item.
}


int FontAtlas::AddCustomRectFontGlyph(Font* font, uint16_t id, int width, int height, float advanceX, const Vec2& offset /*= Vec2(0, 0)*/)
{
    BR_CORE_ASSERT(font != nullptr, "A font must be provided!");
    BR_CORE_ASSERT(width > 0 && width <= 0xFFFF, "Invalid width value!");
    BR_CORE_ASSERT(height > 0 && height <= 0xFFFF, "Invalid height value!");

    FontAtlasCustomRect r;
    r.ID = id;
    r.Width = (unsigned short)width;
    r.Height = (unsigned short)height;
    r.GlyphAdvanceX = advanceX;
    r.GlyphOffset = offset;
    r.FontData = font;
    m_customRects.emplace_back(r);
    return m_customRects.size() - 1; // Index of the new item.
}


void FontAtlas::CalcCustomRectUV(const FontAtlasCustomRect* rect, Vec2* outUvMin, Vec2* outUvMax)
{
    BR_CORE_ASSERT(m_texWidth > 0 && m_texHeight > 0,
                   "Font atlas needs to be built before we can calculate UV coordinates");
    BR_CORE_ASSERT(rect->IsPacked(), "FontAtlasCustomRect must be packed beforehand!");
    *outUvMin = Vec2((float)rect->X * m_texUvScale.x, (float)rect->Y * m_texUvScale.y);
    *outUvMax = Vec2((float)(rect->X + rect->Width) * m_texUvScale.x,
                     (float)(rect->Y + rect->Height) * m_texUvScale.y);
}


bool FontAtlas::GetMouseCursorTexData(MouseCursor cursor, Vec2* outOffset, Vec2* outSize, Vec2 outUvBorder[2], Vec2 outUvFill[2])
{
    if (cursor <= MouseCursor::None || cursor >= MouseCursor::COUNT)
    {
        return false;
    }
    if ((size_t)m_flags & (size_t)FontAtlasFlags::NoMouseCursors)
    {
        return false;
    }

    BR_CORE_ASSERT(m_customRectIds[0] != -1,
                   "There must be custom rectangles already in the atlas!");
    FontAtlasCustomRect& r = m_customRects[m_customRectIds[0]];
    BR_CORE_ASSERT(r.ID == FONT_ATLAS_DEFAULT_TEX_DATA_ID, "Invalid ID!");
    Vec2 pos = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[(size_t)cursor][0] +
        Vec2((float)r.X, (float)r.Y);
    Vec2 size = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[(size_t)cursor][1];
    *outSize = size;
    *outOffset = FONT_ATLAS_DEFAULT_TEX_CURSOR_DATA[(size_t)cursor][2];
    outUvBorder[0] = (pos)*m_texUvScale;
    outUvBorder[1] = (pos + size) * m_texUvScale;
    pos.x += FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF + 1;
    outUvFill[0] = (pos)*m_texUvScale;
    outUvFill[1] = (pos + size) * m_texUvScale;
    return true;
}



//-----------------------------------------------------------------------------
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
//-----------------------------------------------------------------------------

/**
 * Convert UTF-8 to 32-bits character, process single character input.
 * Based on stb_from_utf8() from github.com/nothings/stb/
 * We handle UTF-8 decoding error by skipping forward.
 */
int TextCharFromUtf8(unsigned int* outChar, const char* inText, const char* inTextEnd)
{
    unsigned int c = (unsigned int)-1;
    const unsigned char* str = (const unsigned char*)inText;

    if (!(*str & 0x80))
    {
        c = (unsigned int)(*str++);
        *outChar = c;
        return 1;
    }
    else if ((*str & 0xE0) == 0xC0)
    {
        // Will be invalid but not end of string.
        *outChar = 0xFFFD;
        if (inTextEnd && inTextEnd - (const char*)str < 2)
        {
            return 1;
        }
        else if (*str < 0xC2)
        {
            return 2;
        }
        c = (unsigned int)((*str++ & 0x1F) << 6);
        if ((*str & 0xC0) != 0x80)
        {
            return 2;
        }
        c += (*str++ & 0x3F);
        *outChar = c;
        return 2;
    }
    else if ((*str & 0xF0) == 0xE0)
    {
        // Will be invalid but not end of string.
        *outChar = 0xFFFD;
        if (inTextEnd && inTextEnd - (const char*)str < 3)
        {
            return 1;
        }
        else if (*str == 0xE0 && (str[1] < 0xA0 || str[1] > 0xBF))
        {
            return 3;
        }
        // str[1] < 0x80 is checked below.
        else if (*str == 0xED && str[1] > 0x9F)
        {
            return 3;
        }
        c = (unsigned int)((*str++ & 0x0F) << 12);
        if ((*str & 0xC0) != 0x80)
        {
            return 3;
        }
        c += (unsigned int)((*str++ & 0x3F) << 6);
        if ((*str & 0xC0) != 0x80)
        {
            return 3;
        }
        c += (*str++ & 0x3F);
        *outChar = c;
        return 3;
    }
    else if ((*str & 0xf8) == 0xf0)
    {
        *outChar = 0xFFFD; // will be invalid but not end of string
        if (inTextEnd && inTextEnd - (const char*)str < 4)
        {
            return 1;
        }
        else if (*str > 0xf4)
        {
            return 4;
        }
        else if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf))
        {
            return 4;
        }
        // str[1] < 0x80 is checked below
        else if (*str == 0xf4 && str[1] > 0x8f)
        {
            return 4;
        }
        c = (unsigned int)((*str++ & 0x07) << 18);
        if ((*str & 0xc0) != 0x80)
        {
            return 4;
        }
        c += (unsigned int)((*str++ & 0x3f) << 12);
        if ((*str & 0xc0) != 0x80)
        {
            return 4;
        }
        c += (unsigned int)((*str++ & 0x3f) << 6);
        if ((*str & 0xc0) != 0x80)
        {
            return 4;
        }
        c += (*str++ & 0x3f);
        // UTF-8 encodings of values used in surrogate pairs are invalid
        if ((c & 0xFFFFF800) == 0xD800)
        {
            return 4;
        }
        *outChar = c;
        return 4;
    }

    *outChar = 0;
    return 0;
}

int TextStrFromUtf8(uint16_t* buf, int bufSize, const char* inText, const char* inTextEnd, const char** inTextRemaining)
{
    uint16_t* bufOut = buf;
    uint16_t* bufEnd = buf + bufSize;
    while (bufOut < bufEnd - 1 && (!inTextEnd || inText < inTextEnd) && *inText)
    {
        unsigned int c;
        inText += TextCharFromUtf8(&c, inText, inTextEnd);
        if (c == 0)
        {
            break;
        }
        if (c < 0x10000)
        {
            *bufOut++ = (uint16_t)c;
        }
    }
    *bufOut = 0;
    if (inTextRemaining)
    {
        *inTextRemaining = inText;
    }
    return (int)(bufOut - buf);
}


//-----------------------------------------------------------------------------
// [SECTION] Decompression code
//-----------------------------------------------------------------------------
// Compressed with stb_compress() then converted to a C array and encoded as base85.
// Use the program in misc/fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
// Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------- 

unsigned int stb_decompress_length(const unsigned char* input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

static unsigned char* stb__barrier_out_e, * stb__barrier_out_b;
static const unsigned char* stb__barrier_in_b;
static unsigned char* stb__dout;
static void stb__match(const unsigned char* data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    BR_ASSERT(stb__dout + length <= stb__barrier_out_e, "");
    if (stb__dout + length > stb__barrier_out_e)
    {
        stb__dout += length; return;
    }
    if (data < stb__barrier_out_b)
    {
        stb__dout = stb__barrier_out_e + 1; return;
    }
    while (length--) *stb__dout++ = *data++;
}

static void stb__lit(const unsigned char* data, unsigned int length)
{
    BR_ASSERT(stb__dout + length <= stb__barrier_out_e, "");
    if (stb__dout + length > stb__barrier_out_e)
    {
        stb__dout += length; return;
    }
    if (data < stb__barrier_in_b)
    {
        stb__dout = stb__barrier_out_e + 1; return;
    }
    memcpy(stb__dout, data, length);
    stb__dout += length;
}

#define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
#define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
#define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

static const unsigned char* stb_decompress_token(const unsigned char* i)
{
    if (*i >= 0x20)
    { // use fewer if's for cases that expand small
        if (*i >= 0x80)
        {
            stb__match(stb__dout - i[1] - 1, i[0] - 0x80 + 1);
            i += 2;
        }
        else if (*i >= 0x40)
        {
            stb__match(stb__dout - (stb__in2(0) - 0x4000 + 1), i[2] + 1);
            i += 3;
        }
        else /* *i >= 0x20 */
        {
            stb__lit(i + 1, i[0] - 0x20 + 1);
            i += 1 + (i[0] - 0x20 + 1);
        }
    }
    else
    { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)
        {
            stb__match(stb__dout - (stb__in3(0) - 0x180000 + 1), i[3] + 1);
            i += 4;
        }
        else if (*i >= 0x10)
        {
            stb__match(stb__dout - (stb__in3(0) - 0x100000 + 1), stb__in2(3) + 1);
            i += 5;
        }
        else if (*i >= 0x08)
        {
            stb__lit(i + 2, stb__in2(0) - 0x0800 + 1);
            i += 2 + (stb__in2(0) - 0x0800 + 1);
        }
        else if (*i == 0x07)
        {
            stb__lit(i + 3, stb__in2(1) + 1);
            i += 3 + (stb__in2(1) + 1);
        }
        else if (*i == 0x06)
        {
            stb__match(stb__dout - (stb__in3(1) + 1), i[4] + 1);
            i += 5;
        }
        else if (*i == 0x04)
        {
            stb__match(stb__dout - (stb__in3(1) + 1), stb__in2(4) + 1);
            i += 6;
        }
    }
    return i;
}

static unsigned int stb_adler32(unsigned int adler32, unsigned char* buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen, i;

    blocklen = buflen % 5552;
    while (buflen)
    {
        for (i = 0; i + 7 < blocklen; i += 8)
        {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

unsigned int stb_decompress(unsigned char* output, const unsigned char* i, unsigned int /*length*/)
{
    unsigned int olen;
    if (stb__in4(0) != 0x57bC0000) return 0;
    if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
    olen = stb_decompress_length(i);
    stb__barrier_in_b = i;
    stb__barrier_out_e = output + olen;
    stb__barrier_out_b = output;
    i += 16;

    stb__dout = output;
    for (;;)
    {
        const unsigned char* old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i)
        {
            if (*i == 0x05 && i[1] == 0xfa)
            {
                BR_ASSERT(stb__dout == output + olen, "");
                if (stb__dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int)stb__in4(2))
                    return 0;
                return olen;
            }
            else
            {
                BR_ASSERT(0, ""); /* NOTREACHED */
                return 0;
            }
        }
        BR_ASSERT(stb__dout <= output + olen, "");
        if (stb__dout > output + olen)
            return 0;
    }
}

unsigned int Decode85Byte(char c)
{
    return c >= '\\' ? c - 36 : c - 35;
}

void Decode85(const unsigned char* src, unsigned char* dst)
{
    while (*src)
    {
        unsigned int tmp = Decode85Byte(src[0])
            + 85 * (Decode85Byte(src[1])
                    + 85 * (Decode85Byte(src[2])
                            + 85 * (Decode85Byte(src[3])
                                    + 85 * (Decode85Byte(src[4])))));
        dst[0] = ((tmp >> 0) & 0xFF);
        dst[1] = ((tmp >> 8) & 0xFF);
        dst[2] = ((tmp >> 16) & 0xFF);
        dst[3] = ((tmp >> 24) & 0xFF);
        src += 5;
        dst += 4;
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Default font data (ProggyClean.ttf)
//-----------------------------------------------------------------------------
// ProggyClean.ttf
// Copyright (c) 2004, 2005 Tristan Grimmer
// MIT license (see License.txt in http://www.upperbounds.net/download/ProggyClean.ttf.zip)
// Download and more information at http://upperbounds.net
//-----------------------------------------------------------------------------
// File: 'ProggyClean.ttf' (41208 bytes)
// Exported using misc/fonts/binary_to_compressed_c.cpp (with compression + base85 string encoding).
// The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
//-----------------------------------------------------------------------------
static const char proggy_clean_ttf_compressed_data_base85[11980 + 1] =
R"(7])#######hV0qs'/###[),##/l:$#Q6>##5[n42>c-TH`->>#/e>11NNV=Bv(*:.F?uu#(gRU.o0XGH`$vhLG1hxt9?W`#,5LsCp#-i>.r$<$6pD>Lb';9Crc6tgXmKVeU2cD4Eo3R/2*>]b(MC;$jPfY.;h^`IWM9<Lh2TlS+f-s$o6Q<BWH`YiU.xfLq$N;$0iR/GX:U(jcW2p/W*q?-qmnUCI;jHSAiFWM.R*kU@C=GH?a9wp8f$e.-4^Qg1)Q-GL(lf(r/7GrRgwV%MS=C#`8ND>Qo#t'X#(v#Y9w0#1D$CIf;W'#pWUPXOuxXuU(H9M(1<q-UE31#^-V'8IRUo7Qf./L>=Ke$$'5F%)]0^#0X@U.a<r:QLtFsLcL6##lOj)#.Y5<-R&KgLwqJfLgN&;Q?gI^#DY2uLi@^rMl9t=cWq6##weg>$FBjVQTSDgEKnIS7EM9>ZY9w0#L;>>#Mx&4Mvt//L[MkA#W@lK.N'[0#7RL_&#w+F%HtG9M#XL`N&.,GM4Pg;-<nLENhvx>-VsM.M0rJfLH2eTM`*oJMHRC`NkfimM2J,W-jXS:)r0wK#@Fge$U>`w'N7G#$#fB#$E^$#:9:hk+eOe--6x)F7*E%?76%^GMHePW-Z5l'&GiF#$956:rS?dA#fiK:)Yr+`&#0j@'DbG&#^$PG.Ll+DNa<XCMKEV*N)LN/N*b=%Q6pia-Xg8I$<MR&,VdJe$<(7G;Ckl'&hF;;$<_=X(b.RS%%)###MPBuuE1V:v&cX&#2m#(&cV]`k9OhLMbn%s$G2,B$BfD3X*sp5#l,$R#]x_X1xKX%b5U*[r5iMfUo9U`N99hG)tm+/Us9pG)XPu`<0s-)WTt(gCRxIg(%6sfh=ktMKn3j)<6<b5Sk_/0(^]AaN#(p/L>&VZ>1i%h1S9u5o@YaaW$e+b<TWFn/Z:Oh(Cx2$lNEoN^e)#CFY@@I;BOQ*sRwZtZxRcU7uW6CXow0i(?$Q[cjOd[P4d)]>ROPOpxTO7Stwi1::iB1q)C_=dV26J;2,]7op$]uQr@_V7$q^%lQwtuHY]=DX,n3L#0PHDO4f9>dC@O>HBuKPpP*E,N+b3L#lpR/MrTEH.IAQk.a>D[.e;mc.x]Ip.PH^'/aqUO/$1WxLoW0[iLA<QT;5HKD+@qQ'NQ(3_PLhE48R.qAPSwQ0/WK?Z,[x?-J;jQTWA0X@KJ(_Y8N-:/M74:/-ZpKrUss?d#dZq]DAbkU*JqkL+nwX@@47`5>w=4h(9.`GCRUxHPeR`5Mjol(dUWxZa(>STrPkrJiWx`5U7F#.g*jrohGg`cg:lSTvEY/EV_7H4Q9[Z%cnv;JQYZ5q.l7Zeas:HOIZOB?G<Nald$qs]@]L<J7bR*>gv:[7MI2k).'2($5FNP&EQ(,)U]W]+fh18.vsai00);D3@4ku5P?DP8aJt+;qUM]=+b'8@;mViBKx0DE[-auGl8:PJ&Dj+M6OC]O^((##]`0i)drT;-7X`=-H3[igUnPG-NZlo.#k@h#=Ork$m>a>$-?Tm$UV(?#P6YY#'/###xe7q.73rI3*pP/$1>s9)W,JrM7SN]'/4C#v$U`0#V.[0>xQsH$fEmPMgY2u7Kh(G%siIfLSoS+MK2eTM$=5,M8p`A.;_R%#u[K#$x4AG8.kK/HSB==-'Ie/QTtG?-.*^N-4B/ZM_3YlQC7(p7q)&](`6_c)$/*JL(L-^(]$wIM`dPtOdGA,U3:w2M-0<q-]L_?^)1vw'.,MRsqVr.L;aN&#/EgJ)PBc[-f>+WomX2u7lqM2iEumMTcsF?-aT=Z-97UEnXglEn1K-bnEO`guFt(c%=;Am_Qs@jLooI&NX;]0#j4#F14;gl8-GQpgwhrq8'=l_f-b49'UOqkLu7-##oDY2L(te+Mch&gLYtJ,MEtJfLh'x'M=$CS-ZZ%P]8bZ>#S?YY#%Q&q'3^Fw&?D)UDNrocM3A76//oL?#h7gl85[qW/NDOk%16ij;+:1a'iNIdb-ou8.P*w,v5#EI$TWS>Pot-R*H'-SEpA:g)f+O$%%`kA#G=8RMmG1&O`>to8bC]T&$,n.LoO>29sp3dt-52U%VM#q7'DHpg+#Z9%H[K<L%a2E-grWVM3@2=-k22tL]4$##6We'8UJCKE[d_=%wI;'6X-GsLX4j^SgJ$##R*w,vP3wK#iiW&#*h^D&R?jp7+/u&#(AP##XU8c$fSYW-J95_-Dp[g9wcO&#M-h1OcJlc-*vpw0xUX&#OQFKNX@QI'IoPp7nb,QU//MQ&ZDkKP)X<WSVL(68uVl&#c'[0#(s1X&xm$Y%B7*K:eDA323j998GXbA#pwMs-jgD$9QISB-A_(aN4xoFM^@C58D0+Q+q3n0#3U1InDjF682-SjMXJK)(h$hxua_K]ul92%'BOU&#BRRh-slg8KDlr:%L71Ka:.A;%YULjDPmL<LYs8i#XwJOYaKPKc1h:'9Ke,g)b),78=I39B;xiY$bgGw-&.Zi9InXDuYa%G*f2Bq7mn9^#p1vv%#(Wi-;/Z5ho;#2:;%d&#x9v68C5g?ntX0X)pT`;%pB3q7mgGN)3%(P8nTd5L7GeA-GL@+%J3u2:(Yf>et`e;)f#Km8&+DC$I46>#Kr]]u-[=99tts1.qb#q72g1WJO81q+eN'03'eM>&1XxY-caEnOj%2n8)),?ILR5^.Ibn<-X-Mq7[a82Lq:F&#ce+S9wsCK*x`569E8ew'He]h:sI[2LM$[guka3ZRd6:t%IG:;$%YiJ:Nq=?eAw;/:nnDq0(CYcMpG)qLN4$##&J<j$UpK<Q4a1]MupW^-sj_$%[HK%'F####QRZJ::Y3EGl4'@%FkiAOg#p[##O`gukTfBHagL<LHw%q&OV0##F=6/:chIm0@eCP8X]:kFI%hl8hgO@RcBhS-@Qb$%+m=hPDLg*%K8ln(wcf3/'DW-$.lR?n[nCH-eXOONTJlh:.RYF%3'p6sq:UIMA945&^HFS87@$EP2iG<-lCO$%c`uKGD3rC$x0BL8aFn--`ke%#HMP'vh1/R&O_J9'um,.<tx[@%wsJk&bUT2`0uMv7gg#qp/ij.L56'hl;.s5CUrxjOM7-##.l+Au'A&O:-T72L]P`&=;ctp'XScX*rU.>-XTt,%OVU4)S1+R-#dg0/Nn?Ku1^0f$B*P:Rowwm-`0PKjYDDM'3]d39VZHEl4,.j']Pk-M.h^&:0FACm$maq-&sgw0t7/6(^xtk%LuH88Fj-ekm>GA#_>568x6(OFRl-IZp`&b,_P'$M<Jnq79VsJW/mWS*PUiq76;]/NM_>hLbxfc$mj`,O;&%W2m`Zh:/)Uetw:aJ%]K9h:TcF]u_-Sj9,VK3M.*'&0D[Ca]J9gp8,kAW]%(?A%R$f<->Zts'^kn=-^@c4%-pY6qI%J%1IGxfLU9CP8cbPlXv);C=b),<2mOvP8up,UVf3839acAWAW-W?#ao/^#%KYo8fRULNd2.>%m]UK:n%r$'sw]J;5pAoO_#2mO3n,'=H5(etHg*`+RLgv>=4U8guD$I%D:W>-r5V*%j*W:Kvej.Lp$<M-SGZ':+Q_k+uvOSLiEo(<aD/K<CCc`'Lx>'?;++O'>()jLR-^u68PHm8ZFWe+ej8h:9r6L*0//c&iH&R8pRbA#Kjm%upV1g:a_#Ur7FuA#(tRh#.Y5K+@?3<-8m0$PEn;J:rh6?I6uG<-`wMU'ircp0LaE_OtlMb&1#6T.#FDKu#1Lw%u%+GM+X'e?YLfjM[VO0MbuFp7;>Q&#WIo)0@F%q7c#4XAXN-U&VB<HFF*qL($/V,;(kXZejWO`<[5??ewY(*9=%wDc;,u<'9t3W-(H1th3+G]ucQ]kLs7df($/*JL]@*t7Bu_G3_7mp7<iaQjO@.kLg;x3B0lqp7Hf,^Ze7-##@/c58Mo(3;knp0%)A7?-W+eI'o8)b<nKnw'Ho8C=Y>pqB>0ie&jhZ[?iLR@@_AvA-iQC(=ksRZRVp7`.=+NpBC%rh&3]R:8XDmE5^V8O(x<<aG/1N$#FX$0V5Y6x'aErI3I$7x%E`v<-BY,)%-?Psf*l?%C3.mM(=/M0:JxG'?7WhH%o'a<-80g0NBxoO(GH<dM]n.+%q@jH?f.UsJ2Ggs&4<-e47&Kl+f//9@`b+?.TeN_&B8Ss?v;^Trk;f#YvJkl&w$]>-+k?'(<S:68tq*WoDfZu';mM?8X[ma8W%*`-=;D.(nc7/;)g:T1=^J$&BRV(-lTmNB6xqB[@0*o.erM*<SWF]u2=st-*(6v>^](H.aREZSi,#1:[IXaZFOm<-ui#qUq2$##Ri;u75OK#(RtaW-K-F`S+cF]uN`-KMQ%rP/Xri.LRcB##=YL3BgM/3MD?@f&1'BW-)Ju<L25gl8uhVm1hL$##*8###'A3/LkKW+(^rWX?5W_8g)a(m&K8P>#bmmWCMkk&#TR`C,5d>g)F;t,4:@_l8G/5h4vUd%&%950:VXD'QdWoY-F$BtUwmfe$YqL'8(PWX(P?^@Po3$##`MSs?DWBZ/S>+4%>fX,VWv/w'KD`LP5IbH;rTV>n3cEK8U#bX]l-/V+^lj3;vlMb&[5YQ8#pekX9JP3XUC72L,,?+Ni&co7ApnO*5NK,((W-i:$,kp'UDAO(G0Sq7MVjJsbIu)'Z,*[>br5fX^:FPAWr-m2KgL<LUN098kTF&#lvo58=/vjDo;.;)Ka*hLR#/k=rKbxuV`>Q_nN6'8uTG&#1T5g)uLv:873UpTLgH+#FgpH'_o1780Ph8KmxQJ8#H72L4@768@Tm&Qh4CB/5OvmA&,Q&QbUoi$a_%3M01H)4x7I^&KQVgtFnV+;[Pc>[m4k//,]1?#`VY[Jr*3&&slRfLiVZJ:]?=K3Sw=[$=uRB?3xk48@aeg<Z'<$#4H)6,>e0jT6'N#(q%.O=?2S]u*(m<-V8J'(1)G][68hW$5'q[GC&5j`TE?m'esFGNRM)j,ffZ?-qx8;->g4t*:CIP/[Qap7/9'#(1sao7w-.qNUdkJ)tCF&#B^;xGvn2r9FEPFFFcL@.iFNkTve$m%#QvQS8U@)2Z+3K:AKM5isZ88+dKQ)W6>J%CL<KE>`.d*(B`-n8D9oK<Up]c$X$(,)M8Zt7/[rdkqTgl-0cuGMv'?>-XV1q['-5k'cAZ69e;D_?$ZPP&s^+7])$*$#@QYi9,5P&#9r+$%CE=68>K8r0=dSC%%(@p7.m7jilQ02'0-VWAg<a/''3u.=4L$Y)6k/K:_[3=&jvL<L0C/2'v:^;-DIBW,B4E68:kZ;%?8(Q8BH=kO65BW?xSG&#@uU,DS*,?.+(o(#1vCS8#CHF>TlGW'b)Tq7VT9q^*^$$.:&N@@$&)WHtPm*5_rO0&e%K&#-30j(E4#'Zb.o/(Tpm$>K'f@[PvFl,hfINTNU6u'0pao7%XUp9]5.>%h`8_=VYbxuel.NTSsJfLacFu3B'lQSu/m6-Oqem8T+oE--$0a/k]uj9EwsG>%veR*hv^BFpQj:K'#SJ,sB-'#](j.Lg92rTw-*n%@/;39rrJF,l#qV%OrtBeC6/,;qB3ebNW[?,Hqj2L.1NP&GjUR=1D8QaS3Up&@*9wP?+lo7b?@%'k4`p0Z$22%K3+iCZj?XJN4Nm&+YF]u@-W$U%VEQ/,,>>#)D<h#`)h0:<Q6909ua+&VU%n2:cG3FJ-%@Bj-DgLr`Hw&HAKjKjseK</xKT*)B,N9X3]krc12t'pgTV(Lv-tL[xg_%=M_q7a^x?7Ubd>#%8cY#YZ?=,`Wdxu/ae&#w6)R89tI#6@s'(6Bf7a&?S=^ZI_kS&ai`&=tE72L_D,;^R)7[$s<Eh#c&)q.MXI%#v9ROa5FZO%sF7q7Nwb&#ptUJ:aqJe$Sl68%.D###EC><?-aF&#RNQv>o8lKN%5/$(vdfq7+ebA#u1p]ovUKW&Y%q]'>$1@-[xfn$7ZTp7mM,G,Ko7a&Gu%G[RMxJs[0MM%wci.LFDK)(<c`Q8N)jEIF*+?P2a8g%)$q]o2aH8C&<SibC/q,(e:v;-b#6[$NtDZ84Je2KNvB#$P5?tQ3nt(0d=j.LQf./Ll33+(;q3L-w=8dX$#WF&uIJ@-bfI>%:_i2B5CsR8&9Z&#=mPEnm0f`<&c)QL5uJ#%u%lJj+D-r;BoF&#4DoS97h5g)E#o:&S4weDF,9^Hoe`h*L+_a*NrLW-1pG_&2UdB86e%B/:=>)N4xeW.*wft-;$'58-ESqr<b?UI(_%@[P46>#U`'6AQ]m&6/`Z>#S?YY#Vc;r7U2&326d=w&H####?TZ`*4?&.MK?LP8Vxg>$[QXc%QJv92.(Db*B)gb*BM9dM*hJMAo*c&#b0v=Pjer]$gG&JXDf->'StvU7505l9$AFvgYRI^&<^b68?j#q9QX4SM'RO#&sL1IM.rJfLUAj221]d##DW=m83u5;'bYx,*Sl0hL(W;;$doB&O/TQ:(Z^xBdLjL<Lni;''X.`$#8+1GD:k$YUWsbn8ogh6rxZ2Z9]%nd+>V#*8U_72Lh+2Q8Cj0i:6hp&$C/:p(HK>T8Y[gHQ4`4)'$Ab(Nof%V'8hL&#<NEdtg(n'=S1A(Q1/I&4([%dM`,Iu'1:_hL>SfD07&6D<fp8dHM7/g+tlPN9J*rKaPct&?'uBCem^jn%9_K)<,C5K3s=5g&GmJb*[SYq7K;TRLGCsM-$$;S%:Y@r7AK0pprpL<Lrh,q7e/%KWK:50I^+m'vi`3?%Zp+<-d+$L-Sv:@.o19n$s0&39;kn;S%BSq*$3WoJSCLweV[aZ'MQIjO<7;X-X;&+dMLvu#^UsGEC9WEc[X(wI7#2.(F0jV*eZf<-Qv3J-c+J5AlrB#$p(H68LvEA'q3n0#m,[`*8Ft)FcYgEud]CWfm68,(aLA$@EFTgLXoBq/UPlp7:d[/;r_ix=:TF`S5H-b<LI&HY(K=h#)]Lk$K14lVfm:x$H<3^Ql<M`$OhapBnkup'D#L$Pb_`N*g]2e;X/Dtg,bsj&K#2[-:iYr'_wgH)NUIR8a1n#S?Yej'h8^58UbZd+^FKD*T@;6A7aQC[K8d-(v6GI$x:T<&'Gp5Uf>@M.*J:;$-rv29'M]8qMv-tLp,'886iaC=Hb*YJoKJ,(j%K=H`K.v9HggqBIiZu'QvBT.#=)0ukruV&.)3=(^1`o*Pj4<-<aN((^7('#Z0wK#5GX@7u][`*S^43933A4rl][`*O4CgLEl]v$1Q3AeF37dbXk,.)vj#x'd`;qgbQR%FW,2(?LO=s%Sc68%NP'##Aotl8x=BE#j1UD([3$M(]UI2LX3RpKN@;/#f'f/&_mt&F)XdF<9t4)Qa.*kTLwQ'(TTB9.xH'>#MJ+gLq9-##@HuZPN0]u:h7.T..G:;$/Usj(T7`Q8tT72LnYl<-qx8;-HV7Q-&Xdx%1a,hC=0u+HlsV>nuIQL-5<N?)NBS)QN*_I,?&)2'IM%L3I)X((e/dl2&8'<M:^#M*Q+[T.Xri.LYS3v%fF`68h;b-X[/En'CR.q7E)p'/kle2HM,u;^%OKC-N+Ll%F9CF<Nf'^#t2L,;27W:0O@6##U6W7:$rJfLWHj$#)woqBefIZ.PK<b*t7ed;p*_m;4ExK#h@&]>_>@kXQtMacfD.m-VAb8;IReM3$wf0''hra*so568'Ip&vRs849'MRYSp%:t:h5qSgwpEr$B>Q,;s(C#$)`svQuF$##-D,##,g68@2[T;.XSdN9Qe)rpt._K-#5wF)sP'##p#C0c%-Gb%hd+<-j'Ai*x&&HMkT]C'OSl##5RG[JXaHN;d'uA#x._U;.`PU@(Z3dt4r152@:v,'R.Sj'w#0<-;kPI)FfJ&#AYJ&#//)>-k=m=*XnK$>=)72L]0I%>.G690a:$##<,);?;72#?x9+d;^V'9;jY@;)br#q^YQpx:X#Te$Z^'=-=bGhLf:D6&bNwZ9-ZD#n^9HhLMr5G;']d&6'wYmTFmL<LD)F^%[tC'8;+9E#C$g%#5Y>q9wI>P(9mI[>kC-ekLC/R&CH+s'B;K-M6$EB%is00:+A4[7xks.LrNk0&E)wILYF@2L'0Nb$+pv<(2.768/FrY&h$^3i&@+G%JT'<-,v`3;_)I9M^AE]CN?Cl2AZg+%4iTpT3<n-&%H%b<FDj2M<hH=&Eh<2Len$b*aTX=-8QxN)k11IM1c^j%9s<L<NFSo)B?+<-(GxsF,^-Eh@$4dXhN$+#rxK8'je'D7k`e;)2pYwPA'_p9&@^18ml1^[@g4t*[JOa*[=Qp7(qJ_oOL^('7fB&Hq-:sf,sNj8xq^>$U4O]GKx'm9)b@p7YsvK3w^YR-CdQ*:Ir<($u&)#(&?L9Rg3H)4fiEp^iI9O8KnTj,]H?D*r7'M;PwZ9K0E^k&-cpI;.p/6_vwoFMV<->#%Xi.LxVnrU(4&8/P+:hLSKj$#U%]49t'I:rgMi'FL@a:0Y-uA[39',(vbma*hU%<-SRF`Tt:542R_VV$p@[p8DV[A,?1839FWdF<TddF<9Ah-6&9tWoDlh]&1SpGMq>Ti1O*H&#(AL8[_P%.M>v^-))qOT*F5Cq0`Ye%+$B6i:7@0IX<N+T+0MlMBPQ*Vj>SsD<U4JHY8kD2)2fU/M#$e.)T4,_=8hLim[&);?UkK'-x?'(:siIfL<$pFM`i<?%W(mGDHM%>iWP,##P`%/L<eXi:@Z9C.7o=@(pXdAO/NLQ8lPl+HPOQa8wD8=^GlPa8TKI1CjhsCTSLJM'/Wl>-S(qw%sf/@%#B6;/U7K]uZbi^Oc^2n<bhPmUkMw>%t<)'mEVE''n`WnJra$^TKvX5B>;_aSEK',(hwa0:i4G?.Bci.(X[?b*($,=-n<.Q%`(X=?+@Am*Js0&=3bh8K]mL<LoNs'6,'85`0?t/'_U59@]ddF<#LdF<eWdF<OuN/45rY<-L@&#+fm>69=Lb,OcZV/);TTm8VI;?%OtJ<(b4mq7M6:u?KRdF<gR@2L=FNU-<b[(9c/ML3m;Z[$oF3g)GAWqpARc=<ROu7cL5l;-[A]%/+fsd;l#SafT/f*W]0=O'$(Tb<[)*@e775R-:Yob%g*>l*:xP?Yb.5)%w_I?7uk5JC+FS(m#i'k.'a0i)9<7b'fs'59hq$*5Uhv##pi^8+hIEBF`nvo`;'l0.^S1<-wUK2/Coh58KKhLjM=SO*rfO`+qC`W-On.=AJ56>>i2@2LH6A:&5q`?9I3@@'04&p2/LVa*T-4<-i3;M9UvZd+N7>b*eIwg:CC)c<>nO&#<IGe;__.thjZl<%w(Wk2xmp4Q@I#I9,DF]u7-P=.-_:YJ]aS@V?6*C()dOp7:WL,b&3Rg/.cmM9&r^>$(>.Z-I&J(Q0Hd5Q%7Co-b`-c<N(6r@ip+AurK<m86QIth*#v;-OBqi+L7wDE-Ir8K['m+DDSLwK&/.?-V%U_%3:qKNu$_b*B-kp7NaD'QdWQPKYq[@>P)hI;*_F]u`Rb[.j8_Q/<&>uu+VsH$sM9TA%?)(vmJ80),P7E>)tjD%2L=-t#fK[%`v=Q8<FfNkgg^oIbah*#8/Qt$F&:K*-(N/'+1vMB,u()-a.VUU*#[e%gAAO(S>WlA2);Sa>gXm8YB`1d@K#n]76-a$U,mF<fX]idqd)<3,]J7JmW4`6]uks=4-72L(jEk+:bJ0M^q-8Dm_Z?0olP1C9Sa&H[d&c$ooQUj]Exd*3ZM@-WGW2%s',B-_M%>%Ul:#/'xoFM9QX-$.QN'>[%$Z$uF6pA6Ki2O5:8w*vP1<-1`[G,)-m#>0`P&#eb#.3i)rtB61(o'$?X3B</R90;eZ]%Ncq;-Tl]#F>2Qft^ae_5tKL9MUe9b*sLEQ95C&`=G?@Mj=wh*'3E>=-<)Gt*Iw)'QG:`@IwOf7&]1i'S01B+Ev/Nac#9S;=;YQpg_6U`*kVY39xK,[/6Aj7:'1Bm-_1EYfa1+o&o4hp7KN_Q(OlIo@S%;jVdn0'1<Vc52=u`3^o-n1'g4v58Hj&6_t7$##?M)c<$bgQ_'SY((-xkA#Y(,p'H9rIVY-b,'%bCPF7.J<Up^,(dU1VY*5#WkTU>h19w,WQhLI)3S#f$2(eb,jr*b;3Vw]*7NH%$c4Vs,eD9>XW8?N]o+(*pgC%/72LV-u<Hp,3@e^9UB1J+ak9-TN/mhKPg+AJYd$MlvAF_jCK*.O-^(63adMT->W%iewS8W6m2rtCpo'RS1R84=@paTKt)>=%&1[)*vp'u+x,VrwN;&]kuO9JDbg=pO$J*.jVe;u'm0dr9l,<*wMK*Oe=g8lV_KEBFkO'oU]^=[-792#ok,)i]lR8qQ2oA8wcRCZ^7w/Njh;?.stX?Q1>S1q4Bn$)K1<-rGdO'$Wr.Lc.CG)$/*JL4tNR/,SVO3,aUw'DJN:)Ss;wGn9A32ijw%FL+Z0Fn.U9;reSq)bmI32U==5ALuG&#Vf1398/pVo1*c-(aY168o<`JsSbk-,1N;$>0:OUas(3:8Z972LSfF8eb=c-;>SPw7.6hn3m`9^Xkn(r.qS[0;T%&Qc=+STRxX'q1BNk3&*eu2;&8q$&x>Q#Q7^Tf+6<(d%ZVmj2bDi%.3L2n+4W'$PiDDG)g,r%+?,$@?uou5tSe2aN_AQU*<h`e-GI7)?OK2A.d7_c)?wQ5AS@DL3r#7fSkgl6-++D:'A,uq7SvlB$pcpH'q3n0#_%dY#xCpr-l<F0NR@-##FEV6NTF6##$l84N1w?AO>'IAOURQ##V^Fv-XFbGM7Fl(N<3DhLGF%q.1rC$#:T__&Pi68%0xi_&[qFJ(77j_&JWoF.V735&T,[R*:xFR*K5>>#`bW-?4Ne_&6Ne_&6Ne_&n`kr-#GJcM6X;uM6X;uM(.a..^2TkL%oR(#;u.T%fAr%4tJ8&><1=GHZ_+m9/#H1F^R#SC#*N=BA9(D?v[UiFY>>^8p,KKF.W]L29uLkLlu/+4T<XoIB&hx=T1PcDaB&;HH+-AFr?(m9HZV)FKS8JCw;SD=6[^/DZUL`EUDf]GGlG&>w$)F./^n3+rlo+DB;5sIYGNk+i1t-69Jg--0pao7Sm#K)pdHW&;LuDNH@H>#/X-TI(;P>#,Gc>#0Su>#4`1?#8lC?#<xU?#@.i?#D:%@#HF7@#LRI@#P_[@#Tkn@#Xw*A#]-=A#a9OA#d<F&#*;G##.GY##2Sl##6`($#:l:$#>xL$#B.`$#F:r$#JF.%#NR@%#R_R%#Vke%#Zww%#_-4&#3^Rh%Sflr-k'MS.o?.5/sWel/wpEM0%3'/1)K^f1-d>G21&v(35>V`39V7A4=onx4A1OY5EI0;6Ibgr6M$HS7Q<)58C5w,;WoA*#[%T*#`1g*#d=#+#hI5+#lUG+#pbY+#tnl+#x$),#&1;,#*=M,#.I`,#2Ur,#6b.-#;w[H#iQtA#m^0B#qjBB#uvTB##-hB#'9$C#+E6C#/QHC#3^ZC#7jmC#;v)D#?,<D#C8ND#GDaD#KPsD#O]/E#g1A5#KA*1#gC17#MGd;#8(02#L-d3#rWM4#Hga1#,<w0#T.j<#O#'2#CYN1#qa^:#_4m3#o@/=#eG8=#t8J5#`+78#4uI-#m3B2#SB[8#Q0@8#i[*9#iOn8#1Nm;#^sN9#qh<9#:=x-#P;K2#$%X9#bC+.#Rg;<#mN=.#MTF.#RZO.#2?)4#Y#(/#[)1/#b;L/#dAU/#0Sv;#lY$0#n`-0#sf60#(F24#wrH0#%/e0#TmD<#%JSMFove:CTBEXI:<eh2g)B,3h2^G3i;#d3jD>)4kMYD4lVu`4m`:&5niUA5@(A5BA1]PBB:xlBCC=2CDLXMCEUtiCf&0g2'tN?PGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CPGT4CP-qekC`.9kEg^+F$kwViFJTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5KTB&5o,^<-28ZI'O?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xpO?;xp;7q-#lLYI:xvD=#)";

const char* GetDefaultCompressedFontDataTTFBase85()
{
    return proggy_clean_ttf_compressed_data_base85;
}

bool FontAtlasBuildWithStbTrueType(Brigerad::UI::FontAtlas* atlas)
{
    BR_CORE_ASSERT(!atlas->GetConfig().empty(), "Atlas's config data can't be empty!");

    // Temporary data for one source font (multiple source fonts can be merged into one destination ImFont)
    struct FontBuildSrcData
    {
        stbtt_fontinfo      FontInfo;
        stbtt_pack_range    PackRange;          // Hold the list of codepoints to pack (essentially points to Codepoints.Data)
        stbrp_rect* Rects;              // Rectangle to pack. We first fill in their size and the packer will give us their position.
        stbtt_packedchar* PackedChars;        // Output glyphs
        const uint16_t* SrcRanges;          // Ranges as requested by user (user is allowed to request too much, e.g. 0x0020..0xFFFF)
        int                 DstIndex;           // Index into atlas->Fonts[] and dst_tmp_array[]
        int                 GlyphsHighest;      // Highest requested codepoint
        int                 GlyphsCount;        // Glyph count (excluding missing glyphs and glyphs already set by an earlier source font)
        BrBoolVector        GlyphsSet;          // Glyph bit map (random access, 1-bit per codepoint. This will be a maximum of 8KB)
        std::vector<int>       GlyphsList;         // Glyph codepoints list (flattened version of GlyphsMap)
    };

    // Temporary data for one destination ImFont* (multiple source fonts can be merged into one destination ImFont)
    struct FontBuildDstData
    {
        int                 SrcCount;           // Number of source fonts targeting this destination font.
        int                 GlyphsHighest;
        int                 GlyphsCount;
        BrBoolVector        GlyphsSet;          // This is used to resolve collision when multiple sources are merged into a same destination font.
    };

    FontAtlasBuildRegisterDefaultCustomRects(atlas);

    // Clear atlas.
    atlas->SetTexID(0);
    atlas->SetTexWidth(0);
    atlas->SetTexHeight(0);
    atlas->SetTexUvScale(Vec2(0.0f, 0.0f));
    atlas->ClearTexData();

    // Temporary storage for building.
    std::vector<FontBuildSrcData> srcTmpArray;
    std::vector<FontBuildDstData> dstTmpArray;
    srcTmpArray.resize(atlas->GetConfig().size());
    dstTmpArray.resize(atlas->GetFonts().size());
    memset(srcTmpArray.data(), 0, (size_t)srcTmpArray.size() * sizeof(FontBuildSrcData));
    memset(dstTmpArray.data(), 0, (size_t)dstTmpArray.size() * sizeof(FontBuildDstData));

    // 1. Initialize font loading structure, check font data validity.
    for (int srcI = 0; srcI < atlas->GetConfig().size(); srcI);
    {
        FontBuildSrcData& srcTmp = srcTmpArray[srcI];
        FontConfig& cfg = atlas->GetConfig()[srcI];
        BR_CORE_ASSERT(cfg.DstFont &&
                       (!cfg.DstFont->IsLoaded() || cfg.DstFont->m_containerAtlas == atlas),
                       "");

        // Find index from cfg.DstFont.
        // (We allow the user to set cfg.DstFont. It also makes casual debugging
        // nicer than when storing indices)
        srcTmp.DstIndex = -1;
        for (int outI = 0; outI < atlas->GetFonts().size() && srcTmp.DstIndex == -1;
             outI++)
        {
            if (cfg.DstFont == atlas->GetFonts()[outI])
            {
                srcTmp.DstIndex = outI;
            }
        }
        BR_CORE_ASSERT(srcTmp.DstIndex != -1,
                       "cfg.DstFont not pointing within atlas->m_fonts[]");
        if (srcTmp.DstIndex == -1)
        {
            return false;
        }

        // Initialize helper structure for font loading 
        // and verify that the TTF/OTF data is correct.
        const int fontOffset = stbtt_GetFontOffsetForIndex((unsigned char*)cfg.FontData,
                                                           cfg.FontNo);
        BR_CORE_ASSERT(fontOffset >= 0, "FontData is incorrect, or FontNo cannot be found.");
        if (!stbtt_InitFont(&srcTmp.FontInfo, (unsigned char*)cfg.FontData, fontOffset))
        {
            return false;
        }

        // Measure highest code points.
        FontBuildDstData& dstTmp = dstTmpArray[srcTmp.DstIndex];
        srcTmp.SrcRanges = cfg.GlyphRanges
            ? cfg.GlyphRanges : atlas->GetGlyphRangesDefault();
        for (const uint16_t* srcRange = srcTmp.SrcRanges;
             srcRange[0] && srcRange[1]; srcRange += 2)
        {
            srcTmp.GlyphsHighest = std::max(srcTmp.GlyphsHighest, (int)srcRange[1]);
        }
        dstTmp.SrcCount++;
        dstTmp.GlyphsHighest = std::max(dstTmp.GlyphsHighest, srcTmp.GlyphsHighest);
    }

    // 2. For every requested code point, check for their presence in the font data,
    // and handle redundancy or overlaps between source fonts to avoid unused glyphs.
    int totalGlyphsCount = 0;
    for (int i = 0; i < srcTmpArray.size(); i++)
    {
        FontBuildSrcData& srcTmp = srcTmpArray[i];
        FontBuildDstData& dstTmp = dstTmpArray[srcTmp.DstIndex];
        srcTmp.GlyphsSet.Resize(srcTmp.GlyphsHighest + 1);
        if (dstTmp.GlyphsSet.Storage.empty())
        {
            dstTmp.GlyphsSet.Resize(dstTmp.GlyphsHighest + 1);
        }

        for (const uint16_t* srcRange = srcTmp.SrcRanges;
             srcRange[0] && srcRange[1];
             srcRange += 2)
        {
            for (int codepoint = srcRange[0]; codepoint <= srcRange[1]; codepoint++)
            {
                if (dstTmp.GlyphsSet.GetBit(codepoint))
                {
                    continue;
                }
                if (!stbtt_FindGlyphIndex(&srcTmp.FontInfo, codepoint))
                {
                    continue;
                }

                // Add to avail set/counters.
                srcTmp.GlyphsCount++;
                dstTmp.GlyphsCount++;
                srcTmp.GlyphsSet.SetBit(codepoint, true);
                dstTmp.GlyphsSet.SetBit(codepoint, true);
                totalGlyphsCount++;
            }
        }
    }

    // 3. Unpack our bit map into a flat list.
    // We now have all the Unicode points that we know are requested _and_ 
    // available _and_ not overlapping another.
    for (FontBuildSrcData& srcTmp : srcTmpArray)
    {
        srcTmp.GlyphsList.reserve(srcTmp.GlyphsCount);
        UnpackBoolVectorToFlatIndexList(&srcTmp.GlyphsSet, &srcTmp.GlyphsList);
        srcTmp.GlyphsSet.Clear();
        BR_CORE_ASSERT(srcTmp.GlyphsList.size() == srcTmp.GlyphsCount,
                       "Not the same amount of glyphs!");
    }
    for (auto& i : dstTmpArray)
    {
        i.GlyphsSet.Clear();
    }
    dstTmpArray.clear();
}

void FontAtlasBuildRegisterDefaultCustomRects(Brigerad::UI::FontAtlas* atlas)
{
    if (atlas->GetCustomRectIds()[0] >= 0)
    {
        return;
    }
    if (!((size_t)atlas->GetFlags() & (size_t)Brigerad::UI::FontAtlasFlags::NoMouseCursors))
    {
        atlas->GetCustomRectIds()[0] = atlas->AddCustomRectRegular(
            FONT_ATLAS_DEFAULT_TEX_DATA_ID,
            FONT_ATLAS_DEFAULT_TEX_DATA_W_HALF * 2 + 1,
            FONT_ATLAS_DEFAULT_TEX_DATA_H);
    }
    else
    {
        atlas->GetCustomRectIds()[0] = atlas->AddCustomRectRegular(
            FONT_ATLAS_DEFAULT_TEX_DATA_ID, 2, 2);
    }
}

void UnpackBoolVectorToFlatIndexList(const BrBoolVector* in, std::vector<int>* out)
{
    BR_CORE_ASSERT(sizeof(in->Storage.data()[0]) == sizeof(int), "Not enough data!");
    const int* it_begin = &in->Storage.data()[0];
    const int* it_end = &in->Storage.data()[in->Storage.size()];
    for (const int* it = it_begin; it < it_end; it++)
    {
        if (int entries_32 = *it)
        {
            for (int bit_n = 0; bit_n < 32; bit_n++)
            {
                if (entries_32 & (1u << bit_n))
                {
                    out->emplace_back((int)((it - it_begin) << 5) + bit_n);
                }
            }
        }
    }
}

}
}

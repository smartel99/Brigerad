#pragma once

#include "Brigerad/Core/Types.h"

#include <vector>

namespace Brigerad
{
namespace UI
{
#if defined BR_USE_BGRA_PACKED_COLOR
#define BR_COL32_R_SHIFT 24
#define BR_COL32_G_SHIFT 16
#define BR_COL32_B_SHIFT 8
#define BR_COL32_A_SHIFT 0
#define BR_COL32_A_MASK 0x000000FF
#else
#define BR_COL32_R_SHIFT 0
#define BR_COL32_G_SHIFT 8
#define BR_COL32_B_SHIFT 16
#define BR_COL32_A_SHIFT 24
#define BR_COL32_A_MASK 0xFF000000
#endif
#define BR_COL32(R,G,B,A) (((uint32_t)(A)<<BR_COL32_A_SHIFT)|\
                           ((uint32_t)(B)<<BR_COL32_B_SHIFT)|\
                           ((uint32_t)(G)<<BR_COL32_G_SHIFT)|\
                           ((uint32_t)(R)<<BR_COL32_R_SHIFT))
#define BR_COL32_WHITE      BR_COL32(255,255,255,255)   // Opaque white
#define BR_COL32_BLACK      BR_COL32(0,0,0,255)         // Opaque Black
#define BR_COL32_BLACK_TRANS BR_COL32(0,0,0,0)          // Transparent black.


int TextCharFromUtf8(unsigned int* outChar,
                     const char* inText, const char* inTextEnd);
int TextStrFromUtf8(uint16_t* buf,
                    int bufSize,
                    const char* inText,
                    const char* intTextEnd,
                    const char** inTextRemaining);

class FontConfig;
struct FontGlyph;
class ImFontGlyphRangesBuilder;
struct FontAtlasCustomRect;
class FontAtlas;
class Font;

enum class MouseCursor
{
    None = -1,
    Arrow = 0,
    TextInput,  //! When hovering over InputText, etc.
    ResizeAll,  //! Unused.
    ResizeNS,   //! When hovering over an horizontal border.
    ResizeEW,   //! When hovering over a vertical border or a column
    ResizeNESW, //! When hovering over the bottom-left corner of a window.
    ResizeNWSE, //! When hovering over the bottom-right corner of a window.
    Hand,       //! Unused. Usually for hyper links and stuff like that.
    COUNT
};

struct FontConfig
{
    /**
     * TTF/OTF data.
     */
    void* FontData;                 //          // 
    /**
     * TTF/OTF data size.
     */
    int FontDataSize;               //          // 
    /**
     * TTF/OTF data ownership taken by the container FontAtlas
     * (Will delete memory itself).
     */
    bool FontDataOwnedByAtlas;      // true     // 
    /**
     * Index of font within TTF/OTF file.
     */
    int FontNo;                     // 0        // 
    /**
     * Size in pixels for rasterizer.
     * More or less maps to the resulting font height.
     */
    float SizePixels;               //          // 
    /**
     * Rasterize at higher quality for sub-pixel positioning.
     * Read https://github.com/nothings/stb/blob/master/tests/oversample/README.md
     * for details
     */
    int OversampleH;                // 3        // 
    /**
     * Rasterize at higher quality for sub-pixel positioning.
     * We don't use sub-pixel positions on the Y axis.
     */
    int OversampleV;                // 1        // 
    /**
     * Align every glyph to pixel boundary.
     * Useful for example if you are merging a non-pixel
     * aligned font with the default font.
     * If enabled, you can set OversampleH/V to 1.
     */
    bool PixelSnapH;                // false    //
    /**
     * Extra spacing (in pixels) between glyphs.
     * Only X axis is supported for now.
     */
    Vec2 GlyphExtraSpacing;         // 0, 0     //
    /**
     * Offset all glyphs from this font input.
     */
    Vec2 GlyphOffset;               // 0, 0     //
    /**
     * Pointer to a user-provided list of Unicode range
     * (2 value per range, values are inclusive, zero-terminated list).
     * THE ARRAY DATA NEEDS TO PERSIST AS LONG AS THE FONT IS ALIVE!
     */
    const uint16_t* GlyphRanges;    // nullptr  //
    /**
     * Minimum AdvanceX for glyphs.
     * Set Min to align font icons,
     * Set both Min/Max to enforce mono-space font.
     */
    float GlyphMinAdvanceX;         // 0        //
    /**
     * Maximum AdvanceX for glyphs.
     */
    float GlyphMaxAdvanceX;         // FLT_MAX  //
    /**
     * Merge into previous Font,
     * so you can combine multiple inputs fonts into one Font
     * (e.g. ASCII font + icons + Japanese glyphs).
     * You may want to use GlyphOffset.y when merging fonts
     * of different heights.
     */
    bool MergeMode;                 // false    //
    /**
     * Settings for custom font rasterizer
     * (e.g. FreeType).
     * Leave as zero if you aren't using one.
     */
    unsigned int RasterizerFlags;   // 0x00     //
    /**
     * Brighten (>1.0f) or darken (<1.0f) font output.
     * Brightening small fonts may be
     * a good workaround to make them more readable.
     */
    float RasterizerMultiply;       // 1.0f     //

    // [Internal]
    /**
     * Name, strictly to ease debugging.
     */
    char    Name[40];
    Font* DstFont;

    FontConfig();
};


struct FontGlyph
{
    uint16_t Codepoint;     // 0x0000..0xFFFF
    float AdvanceX;         // Distance to next character. 
                            // (= data from font + 
                            //   FontConfig::GlyphExtraSpacing.x baked in)
    float X0, Y0, X1, Y1;   // Glyph corners.
    float U0, V0, U1, V1;   // Texture coordinates.
};

/**
 * Helper to build glyph ranges from text/string data.
 * Feed your application strings/characters to it,
 * then call BuildRanges().
 * This is essentially a tightly packed of vector of 64k booleans -> 8kB storage.
 */
class FontGlyphRangesBuilder
{
public:
    FontGlyphRangesBuilder()
    {
        Clear();
    }
    inline void Clear()
    {
        int sizeInBytes = 0x10000 / 8;
        m_usedChars.resize(sizeInBytes / (int)sizeof(uint32_t));
        memset(m_usedChars.data(), 0, (size_t)sizeInBytes);
    }

    /**
     * Get bit n in the array.
     */
    inline bool GetBit(int n) const
    {
        int off = (n >> 5);
        uint32_t mask = 1u << (n & 31);
        return (m_usedChars[off] & mask) != 0;
    }

    /**
     * Set bit n in the array.
     */
    inline void SetBit(int n)
    {
        int off = (n >> 5);
        uint32_t mask = 1u << (n & 31);
        m_usedChars[off] |= mask;
    }

    /**
     * Add character.
     */
    inline void AddChar(uint16_t c)
    {
        SetBit(c);
    }

    /**
     * Add string.
     * Each character of the UTF-8 string are added.
     */
    void AddText(const char* text, const char* textEnd = nullptr);

    /**
     * Add ranges.
     * For example:
     *  builder.AddRanges(FontAtlas::GetGlyphRangesDefault())
     * to force add all of ASCII/Latin+ext
     */
    void AddRanges(const uint16_t* ranges);

    /**
     * Output new ranges.
     */
    void BuildRanges(std::vector<uint16_t>* outRanges);
private:
    // Store 1-bit per Unicode code point (0=unused, 1=used)
    std::vector<uint32_t> m_usedChars;
};

/**
 * See FontAtlas::AddCustomRectXXXX functions.
 */
struct FontAtlasCustomRect
{
    /**
     * User ID.
     * Use < 0x10000 to map into a font glyph,
     * Use >= 0x10000 for other/internal/custom texture data.
     */
    unsigned int ID;                // Input
    /**
     * Desired rectangle dimension.
     */
    unsigned short Width, Height;   // Input
    /**
     * Packed position in Atlas.
     */
    unsigned short X, Y;            // Output
    /**
     * For custom font glyphs only (ID<0x10000):
     *  Glyph X advance.
     */
    float GlyphAdvanceX;            // Input
    /**
     * For custom font glyphs only (ID<0x10000):
     *  Glyph display offset.
     */
    Vec2 GlyphOffset;               // Input
    /**
     * For custom font glyphs only (ID<0x10000):
     *  Target font.
     */
    Font* FontData;                 // Input

    FontAtlasCustomRect()
        : ID(0xFFFFFFFF),
        Width(0), Height(0),
        X(0xFFFF), Y(0xFFFF),
        GlyphAdvanceX(0.0f),
        GlyphOffset(Vec2(0.0f, 0.0f)),
        FontData(nullptr)
    {
    }

    bool IsPacked() const
    {
        return X != 0xFFFF;
    }
};

enum class FontAtlasFlags
{
    None = 0,
    NoPowerOfTwoHeight = 1 << 0,    //! Don't round the height to next power of two.
    NoMouseCursors = 1 << 1         //! Don't build software mouse cursors into the atlas.
};


/**
 * Load and rasterize multiple TTF/OTF fonts into a same texture.
 * The font atlas will build a single texture holding:
 *  - One or more fonts.
 *  - Custom graphics data needed to render the shapes needed different shapes.
 *  - Mouse cursor shapes for software cursor rendering
 *      (unless setting flags |= FontAtlasFlags::NoMouseCursors)
 * You can call any of the AddFont*** functions. If you don't call any, the default
 * font embedded in the code will be loaded for you.
 * - Call GetTexDataAsAlpha8() or GetTextDataAsRGBA32() to build and retrieve pixels data.
 * - Upload the pixels data into a texture within the graphic system.
 * - Call SetTexID(myTexId); and pass the pointer/identifier to your texture
 *      in a format natural to your graphics API.
 * - This value will be passed back to you during rendering to identify the texture.
 *
 * Common pitfalls:
 *  - If you pass a glyphRanges array to AddFont*** functions,
 *      you need to make sure that your array persists up until the atlas is build
 *      (when calling GetTexData*** or Build()).
 *      We only copy the pointer, not the data.
 *  - Important: By default, AddFontFromMemoryTTF() takes ownership of the data.
 *      Even though we are not writing to it, we will free the pointer on destruction.
 *      You can set fontCfd->FontDataOwnedByAtlas=false to keep ownership of your data
 *      and it won't be freed.
 *  - Even though many functions are suffixed with "TTF",
 *      OTF data is supported just as well.
 */
class FontAtlas
{
public:
    FontAtlas();
    ~FontAtlas();
    Font* AddFont(const FontConfig* fontCfg);
    Font* AddFontDefault(const FontConfig* fontCfg = nullptr);
    Font* AddFontFromFileTTF(const char* filename,
                             float sizePixels,
                             const FontConfig* fontCfg = nullptr,
                             const uint16_t* glyphRanges = nullptr);
    /**
     * Note: Transfers ownership of fontData to FontAtlas!
     * It will be deleted (freed) after destruction of the atlas.
     * Set fontCfg->FontDataOwnedByAtlas=false to keep ownership of your data
     * and it won't be freed.
     */
    Font* AddFontFromMemoryTTF(void* fontData,
                               int fontSize,
                               float sizePixels,
                               const FontConfig* fontCfg = nullptr,
                               const uint16_t* glyphRanges = nullptr);
    /**
     * compressedFontData still owned by caller.
     * Compress with binary_to_compressed_c.cpp.
     */
    Font* AddFontFromMemoryCompressedTTF(const void* compressedFontData,
                                         int compressedFontSize,
                                         float sizePixels,
                                         const FontConfig* fontCfg = nullptr,
                                         const uint16_t* glyphRanges = nullptr);

    /**
     * compressedFontDataBase85 still owned by caller.
     * Compress with binary_to_compressed_c.cpp with -base85 parameter.
     */
    Font* AddFontFromMemoryCompressedBase85TTF(const char* compressedFontDataBase85,
                                               float sizePixels,
                                               const FontConfig* fontCfg = nullptr,
                                               const uint16_t* glyphRange = nullptr);

    /**
     * Clear input data (all FontConfig structures including sizes,
     * TTF data, glyph ranges, etc.) = all the data used to build the texture and fonts.
     */
    void ClearInputData();

    /**
     * Clear output texture data (CPU side).
     * Saves RAM once the texture has been copied to GPU memory.
     */
    void ClearTexData();

    /**
     * Clear output font data (glyphs storage, UV coordinates).
     */
    void ClearFonts();

    /**
     * Clear all input and output.
     */
    void Clear();

    /**
     * Build atlas, retrieve pixel data.
     * User is in charge of copying the pixels into GPU memory
     * (i.e. create a texture with the engine).
     * Then store the texture handle with SetTexID().
     * The pitch is always = Width * BytesPerPixels (1 or 4).
     * Building in RGBA32 format is provided for convenience and compatibility,
     * but note that unless you manually manipulate or copy color data into
     * the texture (e.g. when using the AddCustomRect*** api), then the RGB pixels
     * emitted will always be white (~75% of memory/bandwidth wasted).
     */

    /**
     * Build pixels data.
     * This is called automatically for you by the GetTexData*** functions.
     */
    bool Build();

    /**
     * 1 byte per pixel
     */
    void GetTexDataAsAlpha8(unsigned char** outPixels,
                            int* outWidth, int* outHeight,
                            int* outBPP = nullptr);
    /**
     * 4 bytes per pixel
     */
    void GetTexDataAsRGBA32(unsigned char** outPixels,
                            int* outWidth, int* outHeight,
                            int* outBPP = nullptr);

    bool IsBuilt() const
    {
        return !m_fonts.empty()
            && (m_texPixelsAlpha8 != nullptr || m_texPixelsRGBA32 != nullptr);
    }
    void SetTexID(uint32_t id)
    {
        m_texID = id;
    }

    //-------------------------------------------------------------------------
    // Glyph Ranges
    //-------------------------------------------------------------------------

    /**
     * Helpers to retrieve list of common Unicode ranges (2 value per range,
     * values are inclusive, zero-terminated list).
     * NB: Make sure that your string are UTF-8 and NOT in you local code page.
     * In C++11, you can create UTF-8 string literal using the u8"Hello world" syntax.
     * NB: Consider using FontGlyphRangesBuilder to build glyph ranges from textual data.
     */
    /**
     * Basic Latin, extended Latin
     */
    uint16_t* GetGlyphRangesDefault();
    /**
     * Default + Korean Characters.
     */
    uint16_t* GetGlyphRangesKorean();
    /**
     * Default + Hiragana, Katakana, Half-width, Selection of 1946 Ideographs.
     */
    uint16_t* GetGlyphRangesJapanese();
    /**
     * Default + Half-Width + Japanese Hiragana/Katakana +
     * full set of about 21000 CJK Unified Ideographs
     */
    uint16_t* GetGlyphRangesChineseFull();
    /**
     * Default + Half-Width + Japanese Hiragana/Katakana +
     * set of 2500 CJK Unified Ideographs for common simplified Chinese
     */
    uint16_t* GetGlyphRangesChineseSimplifiedCommon();
    /**
     * Default + about 400 Cyrillic characters.
     */
    uint16_t* GetGlyphRangesCyrillic();
    /**
     * Default + Thai characters.
     */
    uint16_t* GetGlyphRangesThai();
    /**
     * Default + Vietnamese characters.
     */
    uint16_t* GetGlyphRangesVietnamese();

    /************************************************************************/
    /* [BETA] Custom Rectangles/Glyphs API                                  */
    /************************************************************************/

    /**
     * You can request arbitrary rectangles to be packed into the atlas,
     * for your own purposes.
     * After calling Build(), you can query the rectangle position
     * and render your pixels.
     * You can also request your rectangles to be mapped as font glyph
     * (given a font + Unicode point),
     * so you can render for example custom colorful icons and use them
     * as regular glyphs.
     */
    /**
     * ID needs to be >= 0x10000.
     * ID >= 0x80000000 are reserved for Brigerad.
     */
    int AddCustomRectRegular(unsigned int id, int width, int height);
    /**
     * ID needs to be < 0x10000 to register a rectangle to map into a specific font.
     */
    int AddCustomRectFontGlyph(Font* font, uint16_t id,
                               int width, int height,
                               float advanceX, const Vec2& offset = Vec2(0, 0));

    const FontAtlasCustomRect* GetCustomRectByIndex(int index) const
    {
        if (index < 0)
        {
            return nullptr;
        }
        return &m_customRects[index];
    }

    /************************************************************************/
    /* [INTERNAL]                                                           */
    /************************************************************************/

    void CalcCustomRectUV(const FontAtlasCustomRect* rect, Vec2* outUvMin, Vec2* outUvMax);
    bool GetMouseCursorTexData(MouseCursor cursor,
                               Vec2* outOffset,
                               Vec2* outSize,
                               Vec2 OutUvBorder[2], Vec2 outUvFill[2]);

    int* GetCustomRectIds()
    {
        return m_customRectIds;
    }

    FontAtlasFlags GetFlags() const
    {
        return m_flags;
    }

    const std::vector<FontConfig>& GetConfig() const
    {
        return m_configData;
    }


    int GetTexWidth() const
    {
        return m_texWidth;
    }
    void SetTexWidth(int width)
    {
        m_texWidth = width;
    }

    int GetTexHeight() const
    {
        return m_texHeight;
    }
    void SetTexHeight(int height)
    {
        m_texHeight = height;
    }

    const Vec2& GetTexUvScale() const
    {
        return m_texUvScale;
    }
    void SetTexUvScale(const Vec2& scale)
    {
        m_texUvScale = scale;
    }

    const std::vector<Font*>& GetFonts() const
    {
        return m_fonts;
    }
private:
    /**
     * Marked as locked at the start of a frame, unlocked at the end.
     */
    bool m_locked;
    /**
     * Build flags.
     * See FontAtlasFlags.
     */
    FontAtlasFlags m_flags;
    /**
     * User data to refer to the texture once it has been uploaded to user's graphic
     * system. It is passed back to you during rendering via the DrawCmd structure.
     */
    uint32_t m_texID;
    /**
     * Texture width desired by user befor Build().
     * Must be a power of 2.
     * If you have many glyphs and your graphics API has texture size restrictions,
     * you may want to increase texture width to decrease height.
     */
    int m_texDesiredWidth;
    /**
     * Padding between glyphs within texture in pixels.
     * Defaults to 1. If your rendering method doesn't rely on
     * bilinear filtering you may set this to 0.
     */
    int m_texGlyphPadding;

    ///////////////////////////////////////////////////////////////////////////
    // [INTERNAL]                                                            //
    ///////////////////////////////////////////////////////////////////////////
    // NB: Access texture data via GetTexData*() calls!
    // It will setup a default font for you.

    /**
     * 1 component per pixel.
     * Each component is an unsigned 8-bit.
     * Total size = m_texWidth * m_texHeight
     */
    unsigned char* m_texPixelsAlpha8;
    /**
     * 4 component per pixel.
     * Each component is an unsigned 8-bit.
     * Total size = m_texWidth * m_texHeight * 4
     */
    unsigned int* m_texPixelsRGBA32;
    /**
     * Texture width calculated during Build().
     */
    int m_texWidth;
    /**
     * Texture height calculated during Build().
     */
    int m_texHeight;
    /**
     * = (1.0f/m_texWidth, 1.0f/m_texHeight)
     */
    Vec2 m_texUvScale;
    /**
     * Texture coordinates to a white pixel
     */
    Vec2 m_texUvWhitePixel;
    /**
     * Hold all the fonts returned by AddFont*.
     * m_fonts[0] is the default font upon initiating a new frame.
     * Use Brigerad::UI::PushFont()/PopFont() to change the current font.
     */
    std::vector<Font*> m_fonts;
    /**
     * Rectangles for packing custom texture data into the atlas.
     */
    std::vector<FontAtlasCustomRect> m_customRects;
    /**
     * Internal data.
     */
    std::vector<FontConfig> m_configData;
    /**
     * Identifiers of custom texture rectangle used by FontAtlas/DrawList.
     */
    int m_customRectIds[1];


};

/**
 * Font runtime data and rendering.
 */
class Font
{
public:
    Font();
    ~Font();

    const FontGlyph* FindGlyph(uint16_t c) const;
    const FontGlyph* FindGlyphNoFallback(uint16_t c) const;

    float GetCharAdvance(uint16_t c) const
    {
        return ((int)c < m_indexAdvanceX.size())
            ? m_indexAdvanceX[(int)c] : m_fallbackAdvanceX;
    }

    bool IsLoaded() const
    {
        return m_containerAtlas != nullptr;
    }

    const char* GetDebugName() const
    {
        return m_configData ? m_configData->Name : "<unknown>";
    }

    /**
     * 'maxWidth' stops rendering after a certain width (could be turned into a 2d size).
     * FLT_MAX to disable.
     *
     * 'wrapWidth' enable automatic word-wrapping across multiple lines
     * to fit into given width. 0.0f to disable.
     */
    Vec2 CalcTextSizeA(float size, float maxWidth, float wrapWidth,
                       const char* textBegin,
                       const char* textEnd = nullptr,
                       const char** remaining = nullptr) const;
    const char* CalcWordWrapPositionA(float scale,
                                      const char* text, const char* textEnd,
                                      float wrapWidth) const;
    void RenderChar(float size, Vec2 pos,
                    uint32_t col, uint16_t c) const;
    void RenderText(float size, Vec2 pos,
                    uint32_t col, const Vec4& clipRect,
                    const char* textBegin, const char* textEnd,
                    float wrapWidth = 0.0f,
                    bool cpuFineClip = false) const;

    // [INTERNAL]
    void BuildLookupTable();
    void ClearOutputData();
    void GrowIndex(int newSize);
    void AddGlyph(uint16_t c,
                  float x0, float y0,
                  float x1, float y1,
                  float u0, float v0,
                  float u1, float v1,
                  float advanceX);
    /**
     * Makes 'dst' character/glyph point to 'src' character/glyph.
     * Currently needs to be called AFTER fonts have been built.
     */
    void AddRemapChar(uint16_t dst, uint16_t src, bool overwriteDst = true);

public:
    // Members: Hot ~20/24 bytes (for CalcTextSize)

    // Sparse. Glyphs->AdvanceX in a directly indexable way.
    // (Cache-friendly for CalcTextSize functions which only use this info,
    // and are often bottleneck in large UI).
    std::vector<float> m_indexAdvanceX;         // 12-16    // Out

    // FallbackGlyph->AdvanceX
    float m_fallbackAdvanceX;                   // 4        // Out

    // Height of characters/lines, set during loading. 
    // Doesn't change after loading.
    float m_fontSize;                           // 4        // In

    //-------------------------------------------------------------------------
    // Members: Hot ~36/48 bytes (for CalcTextSize + render loop)

    // Sparse. Index glyphs by Unicode code-point.
    std::vector<uint16_t> m_indexLookup;        // 12-16    // Out
    // All glyphs.
    std::vector<FontGlyph> m_glyphs;            // 12-16    // Out
    // = FindGlyph(FontFallbackChar)
    const FontGlyph* m_fallbackGlyph;           // 4-8      // Out
    // Offset font rendering by xx pixels
    Vec2 m_displayOffset = Vec2(0, 0);          // 8        // In

    //-------------------------------------------------------------------------
    // Members: Cold ~32/40 bytes

    // What we have been loaded into
    FontAtlas* m_containerAtlas;                // 4-8      // Out
    // Pointer withing m_containerAtlas->m_configData
    FontConfig* m_configData;                   // 4-8      // In
    // Number of FontConfig involved in creating this font.
    // Bigger than 1 when merging multiple font sources into one Font.
    short m_configDataCount = 1;                // 2        // In
    // Replacement glyph if one isn't found. Only set via SetFallbackChar()
    uint16_t m_fallbackChar = L'?';             // 2        // In
    // Base font scale, multiplied by the per-window font scale which you can adjust with
    // SetWindowFontScale()
    float m_scale = 1.0f;                       // 4        // In
    // Ascent: distance front top to bottom of e.g. 'A' [0..m_fontSize]
    float m_ascent, m_descent;                  // 4+4      // Out
    // Total surface in pixels to get an idea of the font rasterization/texture cost
    // (not exact, we approximate the cost of padding between glyphs).
    int m_metricsTotlaSurface;                  // 4        // Out
    bool m_dirtyLookupTables;                   // 1        // Out
};
}
}
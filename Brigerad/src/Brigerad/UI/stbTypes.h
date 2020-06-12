#pragma once



typedef struct
{
    unsigned char* data;
    int cursor;
    int size;
} stbtt__buf;

typedef struct
{
    unsigned short x0, y0, x1, y1; // coordinates of bbox in bitmap
    float xoff, yoff, xadvance;
    float xoff2, yoff2;
} stbtt_packedchar;

// this is an opaque structure that you shouldn't mess with which holds
// all the context needed from PackBegin to PackEnd.
struct stbtt_pack_context
{
    void* user_allocator_context;
    void* pack_info;
    int   width;
    int   height;
    int   stride_in_bytes;
    int   padding;
    int   skip_missing;
    unsigned int   h_oversample, v_oversample;
    unsigned char* pixels;
    void* nodes;
};

// The following structure is defined publicly so you can declare one on
// the stack or as a global or etc, but you should treat it as opaque.
struct stbtt_fontinfo
{
    void* userdata;
    unsigned char* data;              // pointer to .ttf file
    int              fontstart;         // offset of start of font

    int numGlyphs;                     // number of glyphs, needed for range checking

    int loca, head, glyf, hhea, hmtx, kern, gpos; // table locations as offset from start of .ttf
    int index_map;                     // a cmap mapping for our chosen character encoding
    int indexToLocFormat;              // format needed to map from glyph index to glyph

    stbtt__buf cff;                    // cff font data
    stbtt__buf charstrings;            // the charstring index
    stbtt__buf gsubrs;                 // global charstring subroutines index
    stbtt__buf subrs;                  // private charstring subroutines index
    stbtt__buf fontdicts;              // array of font dicts
    stbtt__buf fdselect;               // map from glyph to fontdict
};

typedef struct
{
    float font_size;
    int first_unicode_codepoint_in_range;  // if non-zero, then the chars are continuous, and this is the first codepoint
    int* array_of_unicode_codepoints;       // if non-zero, then this is an array of unicode codepoints
    int num_chars;
    stbtt_packedchar* chardata_for_range; // output
    unsigned char h_oversample, v_oversample; // don't set these, they're used internally
} stbtt_pack_range;

struct stbrp_rect
{
   // reserved for your use:
    int            id;

    // input:
    unsigned short    w, h;

    // output:
    unsigned short    x, y;
    int            was_packed;  // non-zero if valid packing
}; // 16 bytes, nominally
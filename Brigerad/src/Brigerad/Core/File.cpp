/**
 * @file   D:\dev\Brigerad\Brigerad\src\Brigerad\Core\File.cpp
 * @author Samuel Martel
 * @date   2020/06/12
 *
 * @brief  Source for the File module.
 */
#include "brpch.h" 
#include "File.h"
#include "Brigerad/UI/Font.h"

namespace Brigerad
{
static FILE* FileOpen(const char* filename, const char* mode);
static int TextCountCharsFromUtf8(const char* inText, const char* inTextEnd);

void* FileLoadToMemory(const char* filename, const char* mode, size_t* outFileSize, int paddingBytes)
{
    BR_CORE_ASSERT(filename && mode, "Invalid parameters!");
    if (outFileSize)
    {
        *outFileSize = 0;
    }

    FILE* f;
    if ((f = FileOpen(filename, mode)) == nullptr)
    {
        return nullptr;
    }

    long fileSizeSigned;
    if (fseek(f, 0, SEEK_END) || (fileSizeSigned = ftell(f)) == -1 || fseek(f, 0, SEEK_SET))
    {
        fclose(f);
        return nullptr;
    }

    size_t fileSize = (size_t)fileSizeSigned;
    void* fileData = BR_ALLOC(fileSize + paddingBytes);
    if (fileData == nullptr)
    {
        fclose(f);
        return nullptr;
    }
    if (fread(fileData, 1, fileSize, f) != fileSize)
    {
        fclose(f);
        BR_FREE(fileData);
        return nullptr;
    }
    if (paddingBytes > 0)
    {
        memset((void*)(((char*)fileData) + fileSize), 0, (size_t)paddingBytes);
    }

    fclose(f);
    if (outFileSize)
    {
        *outFileSize = fileSize;
    }

    return fileData;
}


FILE* FileOpen(const char* filename, const char* mode)
{
    // We need a fopen() wrapper because MSVC/Windows fopen doesn't 
    // handle UTF-8 filenames. Converting both strings from UTF-8 to wchar
    // format (using a single allocation, because we can)
    #if defined(_WIN32) && !defined(__CYGWIN__) && !defined(__GNUC__)
    const int filenameWSize = TextCountCharsFromUtf8(filename, nullptr) + 1;
    const int modeWSize = TextCountCharsFromUtf8(mode, nullptr) + 1;
    std::vector<uint16_t> buf;
    buf.resize(filenameWSize + modeWSize);
    UI::TextStrFromUtf8(&buf.data()[0], filenameWSize, filename, nullptr);
    UI::TextStrFromUtf8(&buf.data()[filenameWSize], modeWSize, mode, nullptr);
    return _wfopen((wchar_t*)&buf.data()[0], (wchar_t*)&buf.data()[filenameWSize]);
    #else
    return fopen(filename, mode);
    #endif
}

int TextCountCharsFromUtf8(const char* inText, const char* inTextEnd)
{
    int charCount = 0;
    while ((!inTextEnd || inText < inTextEnd) && *inText)
    {
        unsigned int c;
        inText += UI::TextCharFromUtf8(&c, inText, inTextEnd);
        if (c == 0)
        {
            break;
        }
        if (c < 0x10000)
        {
            charCount++;
        }
    }
    return charCount;
}

}


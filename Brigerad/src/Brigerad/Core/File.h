#pragma once



namespace Brigerad
{
void* FileLoadToMemory(const char* filename,
                       const char* mode,
                       size_t* outFileSize,
                       int paddingBytes);
}
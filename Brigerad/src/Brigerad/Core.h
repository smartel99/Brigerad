#pragma once


#ifdef BR_PLATFORM_WINDOWS
#ifdef BR_BUILD_DLL
#define BRIGERAD_API __declspec(dllexport)
#else
#define BRIGERAD_API __declspec(dllimport)
#endif
#else
#error Brigerad only support Windows
#endif
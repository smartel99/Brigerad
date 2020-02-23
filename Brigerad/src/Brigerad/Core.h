#pragma once


#if BR_PLATFORM_WINDOWS
#if BR_BUILD_DLL
#define BRIGERAD_API __declspec(dllexport)
#else
#define BRIGERAD_API __declspec(dllimport)
#endif
#else
#error Brigerad only support Windows
#endif


#define BIT(x) (1 << (x))
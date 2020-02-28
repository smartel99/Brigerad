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

#ifdef BR_ENABLE_ASSERTS
#define BR_ASSERT(x, ...) {if(!(x)) { BR_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
                                      __debugbreak(); } }
#define BR_CORE_ASSERT(x, ...) {if(!(x)) { BR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
                                           __debugbreak(); } }
#else
#define BR_ASSERT(x, ...)
#define BR_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << (x))

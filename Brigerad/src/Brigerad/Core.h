#pragma once

#include <memory>

#if !defined(BR_PLATFORM_WINDOWS) && !defined(BR_PLATFORM_LINUX)
    #if defined(__WIN32) || defined(WIN32) || defined(__WIN32__)
        #undef BR_PLATFORM_LINUX
        #define BR_PLATFORM_WINDOWS
    #endif
    #if defined(__LINUX__) || defined(__unix) || defined(__unix__)
        #undef BR_PLATFORM_WINDOWS
        #define BR_PLATFORM_LINUX
    #endif
#endif

#if defined(BR_PLATFORM_WINDOWS)
    #if defined(BR_DYNAMIC_LINK)
        #if defined(BR_BUILD_DLL)
            #define BRIGERAD_API __declspec(dllexport)
        #else
            #define BRIGERAD_API __declspec(dllimport)
        #endif // BR_BUILD_DLL
    #else
        #define BRIGERAD_API
    #endif // BR_DYNAMIC_LINK
#elif defined(BR_PLATFORM_LINUX)
    #define BRIGERAD_API
#else
    #error Brigerad only support Windows and Linux
#endif // BR_PLATFROM_WINDOWS

// This should check the compiler used, not the OS.
#if defined(BR_PLATFORM_WINDOWS)
    #define DEBUG_BREAK(...) __debugbreak()
#elif defined(BR_PLATFORM_LINUX)
    #include <signal.h>
    #define DEBUG_BREAK(...) raise(SIGTRAP)
#else
    #error Brigerad only support Windows and Linux
#endif

#ifdef BR_ENABLE_ASSERTS
#define BR_ASSERT(x, ...)                                   \
    {                                                       \
        if (!(x))                                           \
        {                                                   \
            BR_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            DEBUG_BREAK();                                  \
        }                                                   \
    }
#define BR_CORE_ASSERT(x, ...)                                   \
    {                                                            \
        if (!(x))                                                \
        {                                                        \
            BR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
            DEBUG_BREAK();                                       \
        }                                                        \
    }
#else
    #define BR_ASSERT(x, ...)
    #define BR_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << (x))

#define BR_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Brigerad
{

template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T>
using Ref = std::shared_ptr<T>;

} // namespace Brigerad

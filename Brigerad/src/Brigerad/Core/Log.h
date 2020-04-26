#pragma once
#include "Core.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace Brigerad
{
class BRIGERAD_API Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger()
    {
        return s_CoreLogger;
    }

    inline static std::shared_ptr<spdlog::logger>& GetClientLogger()
    {
        return s_ClientLogger;
    }

private:
    /* Suppress "'Brigerad::Log::s_CoreLogger' : class 'std::shared_ptr<spdlog::logger>'
     * needs to have dll - interface to be used by clients of class 'Brigerad::Log'"
     */
    #pragma warning(suppress: 4251)
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    #pragma warning(suppress: 4251)
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

}

// Core Log Macros.
#define BR_CORE_TRACE(...)      ::Brigerad::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define BR_CORE_INFO(...)       ::Brigerad::Log::GetCoreLogger()->info(__VA_ARGS__)
#define BR_CORE_WARN(...)       ::Brigerad::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define BR_CORE_ERROR(...)      ::Brigerad::Log::GetCoreLogger()->error(__VA_ARGS__)
#define BR_CORE_CRITICAL(...)   ::Brigerad::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Log Macros.
#define BR_TRACE(...)           ::Brigerad::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BR_INFO(...)            ::Brigerad::Log::GetClientLogger()->info(__VA_ARGS__)
#define BR_WARN(...)            ::Brigerad::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BR_ERROR(...)           ::Brigerad::Log::GetClientLogger()->error(__VA_ARGS__)
#define BR_CRITICAL(...)        ::Brigerad::Log::GetClientLogger()->critical(__VA_ARGS__)

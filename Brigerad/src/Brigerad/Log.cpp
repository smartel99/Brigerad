#include "brpch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_sinks.h"

namespace Brigerad
{

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
    // %T -> Timestamp
    // %n -> Name of the logger
    // %v -> Message
    spdlog::set_pattern("%^[%T] %n: %v%$");

    s_CoreLogger = spdlog::stdout_color_mt("BRIGERAD");
    s_CoreLogger->set_level(spdlog::level::trace);

    s_ClientLogger = spdlog::stdout_color_mt("APP");
    s_ClientLogger->set_level(spdlog::level::trace);
}
}
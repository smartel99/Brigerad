#pragma once

#include <filesystem>

#if defined(BR_PLATFORM_WINDOWS) || defined(BR_PLATFORM_LINUX)

extern Brigerad::Application* Brigerad::CreateApplication();

int main(int argc, char** argv)
{
    BR_PROFILE_BEGIN_SESSION("Init", "BrigeradProfile-Startup.json");
    Brigerad::Log::Init();

    BR_CORE_WARN("Running from: {0}", std::filesystem::current_path());

    auto app = Brigerad::CreateApplication();
    BR_PROFILE_END_SESSION();

    app->Run();

    BR_PROFILE_BEGIN_SESSION("Shutdown", "BrigeradProfile-Shutdown.json");
    delete app;
    BR_PROFILE_END_SESSION();
}

#endif
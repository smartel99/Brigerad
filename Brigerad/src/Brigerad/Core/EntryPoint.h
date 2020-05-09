#pragma once

#include <filesystem>

#if defined(BR_PLATFORM_WINDOWS) || defined(BR_PLATFORM_LINUX)

extern Brigerad::Application* Brigerad::CreateApplication();

int main(int argc, char** argv)
{
    Brigerad::Log::Init();

    BR_CORE_WARN("Running from: {0}", std::filesystem::current_path());

    auto app = Brigerad::CreateApplication();

    app->Run();

    delete app;
}

#endif
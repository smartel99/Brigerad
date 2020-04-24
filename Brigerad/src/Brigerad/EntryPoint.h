#pragma once

// #include "Brigerad/Core.h"

#if defined(BR_PLATFORM_WINDOWS) || defined(BR_PLATFORM_LINUX)

extern Brigerad::Application *Brigerad::CreateApplication();

int main(int argc, char **argv)
{
    Brigerad::Log::Init();
    BR_CORE_WARN("Initialized Log!");
    int a = 5;
    BR_INFO("Initialized Client! Var = {0}", a);

    auto app = Brigerad::CreateApplication();
    app->Run();
    delete app;
}

#endif
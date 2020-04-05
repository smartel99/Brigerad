#pragma once

#include "Platform/Windows/WindowsTime.h"


namespace Brigerad
{
double GetTime()
{
    #if BR_PLATFORM_WINDOWS
    return WindowsGetTime();
    #else
    #error Unsuported OS
    #endif
}

}
#pragma once


#include "Brigerad/Scene/Entity.h"

#include "imgui.h"

#include <string>
#include <vector>

namespace Brigerad
{

struct ImGuiSeparatorComponent
{
    size_t _padding                                         = 0;
    ImGuiSeparatorComponent()                               = default;
    ImGuiSeparatorComponent(const ImGuiSeparatorComponent&) = default;
};

struct ImGuiSameLineComponent
{
    float offsetFromStartX = 0.0f;
    float spacing          = -1.0f;

    ImGuiSameLineComponent(float offset, float space) : offsetFromStartX(offset), spacing(space) {}
    ImGuiSameLineComponent()                              = default;
    ImGuiSameLineComponent(const ImGuiSameLineComponent&) = default;
};
}    // namespace Brigerad
#include "brpch.h"
#include "Components.h"

#include <algorithm>

namespace Brigerad
{
void ParentEntityComponent::RemoveChild(const Entity& child)
{
    childs.erase(std::remove(childs.begin(), childs.end(), child), childs.end());
}
}    // namespace Brigerad

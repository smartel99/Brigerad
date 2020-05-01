#pragma once

#include "Brigerad/Renderer/OrthographicCamera.h"


namespace Brigerad
{
class Renderer2D
{
    public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();

    // Primitives
};
}  // namespace Brigerad
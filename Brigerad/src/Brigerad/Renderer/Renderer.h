#pragma once

#include "RendererAPI.h"
#include "RenderCommand.h"
#include "RenderCommandQueue.h"

#include "OrthographicCamera.h"
#include "Shader.h"

namespace Brigerad
{
class Renderer
{
public:
    typedef void (*RenderCommandFn)(void*);

    static void Init();
    static void OnWindowResize(uint32_t width, uint32_t height);

    static void BeginScene(OrthographicCamera& camera);
    static void EndScene();

    static long long GetFrameCount();

    static const Scope<ShaderLibrary>& GetShaderLibrary() { return Get().m_shaderLibrary; }

    static void Submit(const Ref<Shader>&      shader,
                       const Ref<VertexArray>& vertexArray,
                       const glm::mat4&        transform = glm::mat4(1.0f));

    inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    inline static Renderer&        Get() { return *s_instance; }

private:
    struct SceneData
    {
        glm::mat4 ViewProjectionMatrix;
        long long FrameCount;
    };

    static Renderer*     s_instance;
    Scope<ShaderLibrary> m_shaderLibrary;

    static SceneData* m_sceneData;
};
}    // namespace Brigerad

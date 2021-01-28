/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\Renderer\Renderer.cpp
 * @author Samuel Martel
 * @date   2020/03/08
 *
 * @brief  Source for the Renderer module.
 */
#include "brpch.h"
#include "Renderer.h"

#include "RenderCommand.h"
#include "Renderer2D.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Brigerad
{
Renderer::SceneData* Renderer::m_sceneData = new Renderer::SceneData;
Renderer*            Renderer::s_instance  = new Renderer;


void Renderer::Init()
{
    BR_PROFILE_FUNCTION();
    RenderCommand::Init();
    Renderer2D::Init();

    s_instance->m_shaderLibrary = CreateScope<ShaderLibrary>();

    s_instance->m_shaderLibrary->Load("assets/shaders/mesh_static.glsl");
    s_instance->m_shaderLibrary->Load("assets/shaders/mesh_anim.glsl");
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::BeginScene(OrthographicCamera& camera)
{
    m_sceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    m_sceneData->FrameCount++;
}

void Renderer::EndScene()
{
}

long long Renderer::GetFrameCount()
{
    return m_sceneData->FrameCount;
}

void Renderer::Submit(const Ref<Shader>&      shader,
                      const Ref<VertexArray>& vertexArray,
                      const glm::mat4&        transform)
{
    shader->Bind();
    std::dynamic_pointer_cast<OpenGLShader>(shader)->SetMat4("u_vp",
                                                             m_sceneData->ViewProjectionMatrix);
    std::dynamic_pointer_cast<OpenGLShader>(shader)->SetMat4("u_transform", transform);

    vertexArray->Bind();
    RenderCommand::DrawIndexed(vertexArray);
}

}    // namespace Brigerad

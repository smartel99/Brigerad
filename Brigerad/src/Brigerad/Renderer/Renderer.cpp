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
#include "Platform/OpenGL/OpenGLShader.h"

namespace Brigerad
{
Renderer::SceneData *Renderer::m_sceneData = new Renderer::SceneData;

void Renderer::Init()
{
    RenderCommand::Init();
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::SetViewport(0, 0, width, height);
}

void Renderer::BeginScene(OrthographicCamera &camera)
{
    m_sceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene()
{
}

void Renderer::Submit(const Ref<Shader> &shader,
                      const Ref<VertexArray> &vertexArray,
                      const glm::mat4 &transform)
{
    shader->Bind();
    std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_vp", m_sceneData->ViewProjectionMatrix);
    std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_transform", transform);

    vertexArray->Bind();
    RenderCommand::DrawIndexed(vertexArray);
}

} // namespace Brigerad

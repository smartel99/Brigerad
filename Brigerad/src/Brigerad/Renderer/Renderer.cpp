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

namespace Brigerad
{
Renderer::SceneData* Renderer::m_sceneData = new Renderer::SceneData;

void Renderer::BeginScene(OrthographicCamera& camera)
{
    m_sceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}


void Renderer::EndScene()
{

}


void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
{
    shader->Bind();
    shader->UploadUniformMat4("u_vp", m_sceneData->ViewProjectionMatrix);

    vertexArray->Bind();
    RenderCommand::DrawIndexed(vertexArray);
}


}


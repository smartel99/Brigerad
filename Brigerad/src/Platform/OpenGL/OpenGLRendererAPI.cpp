/**
 * @file   E:\dev\Brigerad\Brigerad\src\Platform\OpenGL\OpenGLRendererAPI.cpp
 * @author Samuel Martel
 * @date   2020/03/31
 *
 * @brief  Source for the OpenGLRendererAPI module.
 */
#include "brpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Brigerad
{
static void OpenGLLogMessage(GLenum        source,
                             GLenum        type,
                             GLuint        id,
                             GLenum        severity,
                             GLsizei       len,
                             const GLchar* msg,
                             const void*   usrParam)
{
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
    {
        BR_CORE_ERROR("{}", msg);
        // BR_CORE_ASSERT(false, "");
    }
    else
    {
        // BR_CORE_TRACE("{}", msg);
    }
}

void OpenGLRendererAPI::Init()
{
    BR_PROFILE_FUNCTION();
    glDebugMessageCallback(OpenGLLogMessage, nullptr);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    // Enable alpha blending.
    glEnable(GL_BLEND);
    // Set the way the blending is done.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth testing, which tells OpenGL to check if the pixel to be
    // drawn is in front or behind the others.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glFrontFace(GL_CCW);

    auto& caps = RendererAPI::GetCapabilities();

    caps.vendor   = (const char*)glGetString(GL_VENDOR);
    caps.renderer = (const char*)glGetString(GL_RENDERER);
    caps.version  = (const char*)glGetString(GL_VERSION);

    glGetIntegerv(GL_MAX_SAMPLES, &caps.maxSamples);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &caps.maxAnisotropy);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);    // Disable byte-alignment restriction.

    GLenum error = glGetError();
    while (error != GL_NO_ERROR)
    {
        BR_CORE_ERROR("OpenGL Error: {}", error);
        error = glGetError();
    }
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
{
    uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffers()->GetCount() : indexCount;
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}
}    // namespace Brigerad

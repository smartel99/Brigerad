/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\Renderer\Shader.cpp
 * @author Samuel Martel
 * @date   2020/03/07
 *
 * @brief  Source for the Shader module.
 */

#include "brpch.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Brigerad
{
Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    // Create an empty vertex shader handle.
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL.
    const GLchar* source = (const GLchar*)vertexSrc.c_str();
    glShaderSource(vertexShader, 1, &source, nullptr);

    // Compile the shader.
    glCompileShader(vertexShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL terminator.
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need that shader anymore.
        glDeleteShader(vertexShader);

        BR_CORE_ERROR("{0}", infoLog.data());
        BR_CORE_ASSERT(false, "Unable to compile vertex shader");
    }

    // Create an empty fragment shader handle.
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the vertex shader source code to GL.
    source = (const GLchar*)fragmentSrc.c_str();
    glShaderSource(fragmentShader, 1, &source, nullptr);

    // Compile the shader.
    glCompileShader(fragmentShader);

    isCompiled = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL terminator.
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need that shader anymore.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        BR_CORE_ERROR("{0}", infoLog.data());
        BR_CORE_ASSERT(false, "Unable to compile fragment shader");

        return;
    }

    // Vertex and Fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    m_rendererID = glCreateProgram();

    // Attach our shaders to our program
    glAttachShader(m_rendererID, vertexShader);
    glAttachShader(m_rendererID, fragmentShader);

    // Link our program.
    glLinkProgram(m_rendererID);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(m_rendererID, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(m_rendererID, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL terminator.
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(m_rendererID, maxLength, &maxLength, &infoLog[0]);

        // Delete the program.
        glDeleteProgram(m_rendererID);

        // We don't need the shaders anymore.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);


        BR_CORE_ERROR("{0}", infoLog.data());
        BR_CORE_ASSERT(false, "Unable to link shader");

        return;
    }

    // Detach shaders after successful link.
    glDetachShader(m_rendererID, vertexShader);
    glDetachShader(m_rendererID, fragmentShader);
}


Shader::~Shader()
{
    glDeleteProgram(m_rendererID);
}


void Shader::Bind() const
{
    glUseProgram(m_rendererID);
}


void Shader::Unbind() const
{
    glUseProgram(0);
}

}


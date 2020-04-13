/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\Renderer\Shader.cpp
 * @author Samuel Martel
 * @date   2020/03/07
 *
 * @brief  Source for the Shader module.
 */

#include "brpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Brigerad
{
OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
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


OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_rendererID);
}


void OpenGLShader::Bind() const
{
    glUseProgram(m_rendererID);
}


void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}


void OpenGLShader::UploadUniformInt(const std::string& name, int value)
{
    GLint location = glGetUniformLocation(m_rendererID, name.c_str());
    glUniform1i(location, value);
}


void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
{
    GLint location = glGetUniformLocation(m_rendererID, name.c_str());
    glUniform1f(location, value);
}


void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
{
    GLint location = glGetUniformLocation(m_rendererID, name.c_str());
    glUniform2f(location, values.x, values.y);
}


void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
{
    GLint location = glGetUniformLocation(m_rendererID, name.c_str());
    glUniform3f(location, values.x, values.y, values.z);
}


void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
{
    GLint location = glGetUniformLocation(m_rendererID, name.c_str());
    glUniform4f(location, values.x, values.y, values.z, values.w);
}


void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
    GLint location = glGetUniformLocation(m_rendererID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}


void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    GLint location = glGetUniformLocation(m_rendererID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}


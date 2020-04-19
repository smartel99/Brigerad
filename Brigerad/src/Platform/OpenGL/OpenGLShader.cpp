/**
 * @file   E:\dev\Brigerad\Brigerad\src\Brigerad\Renderer\Shader.cpp
 * @author Samuel Martel
 * @date   2020/03/07
 *
 * @brief  Source for the Shader module.
 */

#include "brpch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Brigerad
{
static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
    {
        return GL_VERTEX_SHADER;
    }
    else if (type == "fragment" || type == "pixel")
    {
        return GL_FRAGMENT_SHADER;
    }
    else
    {
        BR_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }
}


OpenGLShader::OpenGLShader(const std::string& filePath)
{
    std::string src = ReadFile(filePath);
    auto shaderSources = PreProcess(src);
    Compile(shaderSources);

    // Extract name from path.
    size_t lastSlash = filePath.find_last_of(R"(/\)");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    size_t lastDot = filePath.rfind('.');

    size_t count = lastDot == std::string::npos ?
        filePath.size() - lastSlash
        : lastDot - lastSlash;
    m_name = filePath.substr(lastSlash, count);
}



OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    :m_rendererID(0), m_name(name)
{
    std::unordered_map<GLenum, std::string> srcs;
    srcs[GL_VERTEX_SHADER] = vertexSrc;
    srcs[GL_FRAGMENT_SHADER] = fragmentSrc;
    Compile(srcs);
}


OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_rendererID);
}

std::string OpenGLShader::ReadFile(const std::string& filePath)
{
    std::string result = "";
    std::ifstream in = std::ifstream(filePath, std::ios::in | std::ios::binary);
    if (in)
    {
        // Move to the very end of the file.
        in.seekg(0, std::ios::end);
        // Use the position of the file pointer as the size of our string.
        result.resize(in.tellg());

        // Go back to the start.
        in.seekg(0, std::ios::beg);
        // Read the entire file.
        in.read(&result[0], result.size());
        // Close the file.
        in.close();
    }
    else
    {
        BR_CORE_ERROR("Could not open file '{0}'", filePath);
    }

    return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
{
    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLen = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of("\r\n", pos);
        BR_CORE_ASSERT(eol != std::string::npos, "Syntax error");
        size_t begin = pos + typeTokenLen + 1;
        std::string type = source.substr(begin, eol - begin);
        BR_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        shaderSources[ShaderTypeFromString(type)] =
            source.substr(nextLinePos, pos - (nextLinePos == std::string::npos
                                              ? source.size() - 1 : nextLinePos));
    }

    return shaderSources;
}

void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSrcs)
{
    // Get a program object.
    GLuint program = glCreateProgram();
    BR_CORE_ASSERT(shaderSrcs.size() <= 2, "Maximum 2 shaders per file");
    std::array<GLuint, 2> shaderIDs;
    int shaderIdIdx = 0;

    for (auto& kv : shaderSrcs)
    {
        GLenum type = kv.first;
        const std::string& source = kv.second;

        // Create an empty shader handle.
        GLuint shader = glCreateShader(type);

        // Send the shader source code to GL.
        const GLchar* sourceCStr = (const GLchar*)source.c_str();
        glShaderSource(shader, 1, &sourceCStr, nullptr);

        // Compile the shader.
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL terminator.
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need that shader anymore.
            glDeleteShader(shader);

            BR_CORE_ERROR("{0}", infoLog.data());
            BR_CORE_ASSERT(false, "Unable to compile vertex shader");
        }

        // Attach our shaders to our program
        glAttachShader(program, shader);
        shaderIDs[shaderIdIdx++] = shader;
    }

    // Vertex and Fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Link our program.
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL terminator.
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // Delete the program.
        glDeleteProgram(program);

        // We don't need the shaders anymore.
        for (auto id : shaderIDs)
        {
            glDeleteShader(id);
        }

        BR_CORE_ERROR("{0}", infoLog.data());
        BR_CORE_ASSERT(false, "Unable to link shader");

        return;
    }

    // Detach shaders after successful link.
    for (auto id : shaderIDs)
    {
        glDetachShader(program, id);
    }
    m_rendererID = program;
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


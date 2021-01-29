/**
 * @file   E:\dev\Brigerad\Brigerad\src\Platform\OpenGL\OpenGLTexture.cpp
 * @author Samuel Martel
 * @date   2020/04/11
 *
 * @brief  Source for the OpenGLTexture module.
 */
#include "brpch.h"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include "Brigerad/Renderer/RendererAPI.h"

namespace Brigerad
{
OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, uint8_t channels)
: m_width(width), m_height(height)
{
    BR_PROFILE_FUNCTION();

    if (channels == 4)
    {
        m_internalFormat = GL_RGBA8;
        m_dataFormat     = GL_RGBA;
    }
    else if (channels == 3)
    {
        m_internalFormat = GL_RGB8;
        m_dataFormat     = GL_RGB;
    }
    else if (channels == 1)
    {
        // m_internalFormat = GL_ALPHA8;
        m_internalFormat = GL_R8;
        // m_dataFormat = GL_ALPHA;
        m_dataFormat = GL_R;
    }
    else
    {
        BR_ERROR("Invalid number of channels!");
        return;
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
    glTextureStorage2D(m_rendererID, 1, m_internalFormat, m_width, m_height);

    glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_path(path)
{
    BR_PROFILE_FUNCTION();

    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc* data = nullptr;
    {
        BR_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    }
    BR_CORE_ASSERT(data, "Failed to load image!");
    m_width  = width;
    m_height = height;

    GLenum internalFormat = 0, dataFormat = 0;
    if (channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat     = GL_RGBA;
    }
    else if (channels == 3)
    {
        internalFormat = GL_RGB8;
        dataFormat     = GL_RGB;
    }
    else if (channels == 2)
    {
        internalFormat = GL_RG8;
        dataFormat     = GL_RG;
    }

    m_internalFormat = internalFormat;
    m_dataFormat     = dataFormat;

    BR_CORE_ASSERT(internalFormat && dataFormat, "Format not supported");

    glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
    glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

    glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(
      m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}


OpenGLTexture2D::~OpenGLTexture2D()
{
    BR_PROFILE_FUNCTION();

    glDeleteTextures(1, &m_rendererID);
}

void OpenGLTexture2D::SetData(void* data, uint32_t size)
{
    BR_PROFILE_FUNCTION();

    // Make sure the data format is either GL_RGBA, GL_RGB or GL_R and that we have all the data we
    // need.
    BR_CORE_ASSERT(size == m_width * m_height *
                             (m_dataFormat == GL_RGBA ? 4 : (m_dataFormat == GL_RGB ? 3 : 1)),
                   "Data must be entire texture!");

    if (m_dataFormat == GL_RGBA || m_dataFormat == GL_RGB)
    {
        glTextureSubImage2D(
          m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
    }
    else if (m_dataFormat == GL_R)
    {
        glTextureSubImage2D(
          m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
        // static const uint8_t d[] = {255};
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE,
        // d);
    }
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
    BR_PROFILE_FUNCTION();

    glBindTextureUnit(slot, m_rendererID);
}


OpenGLTextureCube::OpenGLTextureCube(uint32_t width, uint32_t height, uint8_t channels)
: m_width(width), m_height(height)
{
    BR_PROFILE_FUNCTION();

    if (channels == 4)
    {
        m_internalFormat = GL_RGBA8;
        m_dataFormat     = GL_RGBA;
    }
    else if (channels == 3)
    {
        m_internalFormat = GL_RGB8;
        m_dataFormat     = GL_RGB;
    }
    else if (channels == 1)
    {
        // m_internalFormat = GL_ALPHA8;
        m_internalFormat = GL_R8;
        // m_dataFormat = GL_ALPHA;
        m_dataFormat = GL_R;
    }
    else
    {
        BR_ERROR("Invalid number of channels!");
        return;
    }


    uint8_t* data = new uint8_t[m_width * m_width * channels];

    memset(data, 0, (m_width * m_width * channels));
    CreateTexture(data);

    delete[] data;
}

OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
{
    int w, h, channels;
    stbi_set_flip_vertically_on_load(false);
    m_imageData = stbi_load(path.c_str(), &w, &h, &channels, 0);

    m_width               = w;
    m_height              = h;
    GLenum internalFormat = 0, dataFormat = 0;
    if (channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat     = GL_RGBA;
    }
    else if (channels == 3)
    {
        internalFormat = GL_RGB8;
        dataFormat     = GL_RGB;
    }

    m_internalFormat = internalFormat;
    m_dataFormat     = dataFormat;

    BR_CORE_ASSERT(internalFormat && dataFormat, "Format not supported");

    CreateTexture(m_imageData);

    stbi_image_free(m_imageData);
}

OpenGLTextureCube::~OpenGLTextureCube()
{
    glDeleteTextures(1, &m_rendererID);
}

void OpenGLTextureCube::Bind(uint32_t slot /*= 0*/) const
{
    glBindTextureUnit(slot, m_rendererID);
}

void OpenGLTextureCube::SetData(void* data, uint32_t size)
{
}

void OpenGLTextureCube::CreateTexture(uint8_t* data)
{
    uint32_t faceW = m_width / 4;
    uint32_t faceH = m_height / 3;
    BR_CORE_ASSERT(faceW == faceH, "Non-square faces!");

    std::array<uint8_t*, 6> faces;
    for (size_t i = 0; i < faces.size(); i++)
    {
        faces[i] = new uint8_t[faceW * faceH * 3];    // 3BPP.
    }

    int faceIndex = 0;

    for (size_t i = 0; i < 4; i++)
    {
        for (size_t y = 0; y < faceH; y++)
        {
            size_t yOffset = y + faceH;
            for (size_t x = 0; x < faceW; x++)
            {
                size_t xOffset = x + i * faceW;
                faces[faceIndex][(x + y * faceW) * 3 + 0] =
                  data[(xOffset + yOffset * m_width) * 3 + 0];
                faces[faceIndex][(x + y * faceW) * 3 + 1] =
                  data[(xOffset + yOffset * m_width) * 3 + 1];
                faces[faceIndex][(x + y * faceW) * 3 + 2] =
                  data[(xOffset + yOffset * m_width) * 3 + 2];
            }
        }
        faceIndex++;
    }

    for (size_t i = 0; i < 3; i++)
    {
        // Skip the middle one.
        if (i == 0)
        {
            continue;
        }

        for (size_t y = 0; y < faceH; y++)
        {
            size_t yOffset = y + i * faceH;
            for (size_t x = 0; x < faceW; x++)
            {
                size_t xOffset = x + faceW;
                faces[faceIndex][(x + y * faceW) * 3 + 0] =
                  data[(xOffset + yOffset * m_width) * 3 + 0];
                faces[faceIndex][(x + y * faceW) * 3 + 1] =
                  data[(xOffset + yOffset * m_width) * 3 + 1];
                faces[faceIndex][(x + y * faceW) * 3 + 2] =
                  data[(xOffset + yOffset * m_width) * 3 + 2];
            }
        }

        faceIndex++;
    }

    glGenTextures(1, &m_rendererID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameterf(
      m_rendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().maxAnisotropy);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                 0,
                 m_dataFormat,
                 faceW,
                 faceH,
                 0,
                 m_dataFormat,
                 GL_UNSIGNED_BYTE,
                 faces[2]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                 0,
                 m_dataFormat,
                 faceW,
                 faceH,
                 0,
                 m_dataFormat,
                 GL_UNSIGNED_BYTE,
                 faces[0]);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                 0,
                 m_dataFormat,
                 faceW,
                 faceH,
                 0,
                 m_dataFormat,
                 GL_UNSIGNED_BYTE,
                 faces[4]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                 0,
                 m_dataFormat,
                 faceW,
                 faceH,
                 0,
                 m_dataFormat,
                 GL_UNSIGNED_BYTE,
                 faces[5]);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                 0,
                 m_dataFormat,
                 faceW,
                 faceH,
                 0,
                 m_dataFormat,
                 GL_UNSIGNED_BYTE,
                 faces[1]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                 0,
                 m_dataFormat,
                 faceW,
                 faceH,
                 0,
                 m_dataFormat,
                 GL_UNSIGNED_BYTE,
                 faces[3]);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (size_t i = 0; i < faces.size(); i++)
    {
        delete[] faces[i];
    }
}

}    // namespace Brigerad

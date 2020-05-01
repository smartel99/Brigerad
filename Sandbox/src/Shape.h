#pragma once

#include "Brigerad/Renderer/Shader.h"
#include "Brigerad/Renderer/VertexArray.h"
#include "Brigerad/Core/Timestep.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include "Controller.h"

#include <glm/glm.hpp>

#include <memory>

class Shape : public Controller
{
public:
    Shape(float* vecs, size_t vecCnt,
          uint32_t* indices, size_t indicesCnt,
          const Brigerad::BufferLayout& layout,
          const std::string& vertexShader,
          const std::string& fragmentShader,
          const std::string& debugName);

    Shape(float* vecs, size_t vecCnt,
          uint32_t* indices, size_t indicesCnt,
          const Brigerad::BufferLayout& layout,
          const std::string& filePath,
          const std::string& debugName);

    Shape(float* vecs, size_t vecCnt,
          uint32_t* indices, size_t indicesCnd,
          const Brigerad::BufferLayout& layout,
          const Brigerad::Ref<Brigerad::Shader>& shader,
          const std::string& debugName);

    void Submit();
    virtual void OnUpdate(Brigerad::Timestep ts) override;

    const Brigerad::Ref<Brigerad::Shader> GetShader() const
    {
        return m_shader;
    }

    virtual void SetPosition(const glm::vec3& position) override
    {
        m_position = position;
        RecalculateTransform();
    }
    virtual void SetRotation(float rotation) override
    {
        m_rotation = rotation;
        RecalculateTransform();
    }

    const glm::mat4& GetTransform() const
    {
        return m_transform;
    }

    const glm::vec3& GetScale() const
    {
        return m_scaleVec;
    }
    void SetScale(const glm::vec3& scale)
    {
        m_scaleVec = scale;
        RecalculateScale();
    }
    void SetScale(float scale)
    {
        m_scaleVec = glm::vec3(scale);
        RecalculateScale();
    }

    const glm::vec4& GetColor() const
    {
        return m_color;
    }
    glm::vec4& GetColorRef()
    {
        return m_color;
    }
    void SetColor(const glm::vec4& color)
    {
        m_color = color;
        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_shader)
            ->UploadUniformFloat4("u_Color", color);
    }
    void UploadColor() const
    {
        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_shader)
            ->UploadUniformFloat4("u_Color", m_color);
    }

    static Brigerad::Ref<Shape> Create(float* vecs, size_t vecCnt,
                                       uint32_t* indices, size_t indicesCnd,
                                       const Brigerad::BufferLayout& layout,
                                       const std::string& vertexShader,
                                       const std::string& fragmentShader,
                                       const std::string& debugName);

    static Brigerad::Ref<Shape> Create(float* vecs, size_t vecCnt,
                                       uint32_t* indices, size_t indicesCnd,
                                       const Brigerad::BufferLayout& layout,
                                       const std::string& filePath,
                                       const std::string& debugName);

    static Brigerad::Ref<Shape> Create(float* vecs, size_t vecCnt,
                                       uint32_t* indices, size_t indicesCnd,
                                       const Brigerad::BufferLayout& layout,
                                       const Brigerad::Ref<Brigerad::Shader>& shader,
                                       const std::string& debugName);

private:
    Brigerad::Ref<Brigerad::Shader> m_shader;
    Brigerad::Ref<Brigerad::VertexArray> m_vertexArray;
    glm::mat4 m_transform;
    glm::mat4 m_scale;
    glm::mat4 m_ts;
    glm::vec3 m_scaleVec;

    glm::vec4 m_color;

    std::string m_name = "Default";

private:
    virtual void HandleKeys(Brigerad::Timestep ts) override;
    void RecalculateTransform();
    void RecalculateScale();
};
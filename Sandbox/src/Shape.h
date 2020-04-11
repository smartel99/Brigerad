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
          uint32_t* indices, size_t indicesCnd,
          const Brigerad::BufferLayout& layout,
          const std::string& vertexShader,
          const std::string& fragmentShader,
          const std::string& debugName = "Default");

    void Submit();
    virtual void OnUpdate(Brigerad::Timestep ts) override;

    const std::shared_ptr<Brigerad::Shader> GetShader() const
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

    const glm::vec3& GetColor() const
    {
        return m_color;
    }
    glm::vec3& GetColorRef()
    {
        return m_color;
    }
    void SetColor(const glm::vec3& color)
    {
        m_color = color;
        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_shader)
            ->UploadUniformFloat3("u_Color", color);
    }
    void UploadColor() const
    {
        std::dynamic_pointer_cast<Brigerad::OpenGLShader>(m_shader)
            ->UploadUniformFloat3("u_Color", m_color);
    }

    static Shape* Create(float* vecs, size_t vecCnt,
                         uint32_t* indices, size_t indicesCnd,
                         const Brigerad::BufferLayout& layout,
                         const std::string& vertexShader,
                         const std::string& fragmentShader,
                         const std::string& debugName = "Default");

private:
    std::shared_ptr<Brigerad::Shader> m_shader;
    std::shared_ptr<Brigerad::VertexArray> m_vertexArray;
    glm::mat4 m_transform;
    glm::mat4 m_scale;
    glm::mat4 m_ts;
    glm::vec3 m_scaleVec;

    glm::vec3 m_color;

    std::string m_name;

private:
    virtual void HandleKeys(Brigerad::Timestep ts) override;
    void RecalculateTransform();
    void RecalculateScale();
};
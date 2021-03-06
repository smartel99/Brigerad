/**
 * @file   E:\dev\Brigerad\Sandbox\src\Shape.cpp
 * @author Samuel Martel
 * @date   2020/04/05
 *
 * @brief  Source for the Shape module.
 */

#include "Shape.h"

#include "Brigerad/Renderer/Renderer.h"
#include "Brigerad/Core/Log.h"

#include <glm/gtc/matrix_transform.hpp>

Shape::Shape(float* vecs,
             size_t vecCnt,
             uint32_t* indices,
             size_t indicesCnt,
             const Brigerad::BufferLayout& layout,
             const std::string& vertexShader,
             const std::string& fragmentShader,
             const std::string& debugName)
: Controller(),
  m_name(debugName),
  m_transform(1.0f),
  m_scale(1.0f),
  m_scaleVec(1.0f),
  m_ts(1.0f),
  m_color(0.2f, 0.3f, 0.8f, 1.0f)
{
    m_vertexArray = Brigerad::VertexArray::Create();

    Brigerad::Ref<Brigerad::VertexBuffer> vertexBuffer;
    vertexBuffer = Brigerad::VertexBuffer::Create(vecs, uint32_t(vecCnt));

    vertexBuffer->SetLayout(layout);

    m_vertexArray->AddVertexBuffer(vertexBuffer);

    Brigerad::Ref<Brigerad::IndexBuffer> indexBuffer;
    indexBuffer = Brigerad::IndexBuffer::Create(indices, uint32_t(indicesCnt));
    m_vertexArray->SetIndexBuffer(indexBuffer);

    m_shader = Brigerad::Shader::Create(debugName, vertexShader, fragmentShader);
}

Shape::Shape(float* vecs,
             size_t vecCnt,
             uint32_t* indices,
             size_t indicesCnt,
             const Brigerad::BufferLayout& layout,
             const std::string& filePath,
             const std::string& debugName)
: Controller(),
  m_name(debugName),
  m_transform(1.0f),
  m_scale(1.0f),
  m_scaleVec(1.0f),
  m_ts(1.0f),
  m_color(0.2f, 0.3f, 0.8f, 1.0f)
{
    m_vertexArray = Brigerad::VertexArray::Create();

    Brigerad::Ref<Brigerad::VertexBuffer> vertexBuffer;
    vertexBuffer = Brigerad::VertexBuffer::Create(vecs, uint32_t(vecCnt));

    vertexBuffer->SetLayout(layout);

    m_vertexArray->AddVertexBuffer(vertexBuffer);

    Brigerad::Ref<Brigerad::IndexBuffer> indexBuffer;
    indexBuffer = Brigerad::IndexBuffer::Create(indices, uint32_t(indicesCnt));
    m_vertexArray->SetIndexBuffer(indexBuffer);

    m_shader = Brigerad::Shader::Create(filePath);
}

Shape::Shape(float* vecs,
             size_t vecCnt,
             uint32_t* indices,
             size_t indicesCnt,
             const Brigerad::BufferLayout& layout,
             const Brigerad::Ref<Brigerad::Shader>& shader,
             const std::string& debugName)
: Controller(),
  m_name(debugName),
  m_transform(1.0f),
  m_scale(1.0f),
  m_scaleVec(1.0f),
  m_ts(1.0f),
  m_color(0.2f, 0.3f, 0.8f, 1.0f),
  m_shader(shader)
{
    m_vertexArray = Brigerad::VertexArray::Create();

    Brigerad::Ref<Brigerad::VertexBuffer> vertexBuffer;
    vertexBuffer = Brigerad::VertexBuffer::Create(vecs, uint32_t(vecCnt));

    vertexBuffer->SetLayout(layout);

    m_vertexArray->AddVertexBuffer(vertexBuffer);

    Brigerad::Ref<Brigerad::IndexBuffer> indexBuffer;
    indexBuffer = Brigerad::IndexBuffer::Create(indices, uint32_t(indicesCnt));
    m_vertexArray->SetIndexBuffer(indexBuffer);
}

void Shape::Submit()
{
    Brigerad::Renderer::Submit(m_shader, m_vertexArray, m_ts);
}

void Shape::OnUpdate(Brigerad::Timestep ts)
{
    static float oldRot = m_rotation;
    HandleKeys(ts);
    glm::vec3 acc =
      glm::vec3(m_acceleration.x * ts, m_acceleration.y * ts, m_acceleration.z * ts);
    m_position += acc;
    // If position or rotation has changed since last frame:
    if (acc != glm::vec3(0.0f) || oldRot != m_rotation)
    {
        RecalculateTransform();
    }
}

Brigerad::Ref<Shape> Shape::Create(float* vecs,
                                   size_t vecCnt,
                                   uint32_t* indices,
                                   size_t indicesCnd,
                                   const Brigerad::BufferLayout& layout,
                                   const std::string& vertexShader,
                                   const std::string& fragmentShader,
                                   const std::string& debugName)
{
    return std::make_shared<Shape>(
      vecs, vecCnt, indices, indicesCnd, layout, vertexShader, fragmentShader, debugName);
}

Brigerad::Ref<Shape> Shape::Create(float* vecs,
                                   size_t vecCnt,
                                   uint32_t* indices,
                                   size_t indicesCnd,
                                   const Brigerad::BufferLayout& layout,
                                   const std::string& filePath,
                                   const std::string& debugName)
{
    return std::make_shared<Shape>(vecs, vecCnt, indices, indicesCnd, layout, filePath, debugName);
}

Brigerad::Ref<Shape> Shape::Create(float* vecs,
                                   size_t vecCnt,
                                   uint32_t* indices,
                                   size_t indicesCnd,
                                   const Brigerad::BufferLayout& layout,
                                   const Brigerad::Ref<Brigerad::Shader>& shader,
                                   const std::string& debugName)
{
    return std::make_shared<Shape>(vecs, vecCnt, indices, indicesCnd, layout, shader, debugName);
}

void Shape::HandleKeys(Brigerad::Timestep ts)
{
    for (Brigerad::KeyCode key : m_keys)
    {
        float ds = (MOVE_SPEED * ts);
        float dr = (ROTATION_SPEED * ts);
        // Down.
        if (key == BR_KEY_W)
        {
            // If not at max acceleration:
            if (m_acceleration.y <= MAX_POS_SPEED)
            {
                m_acceleration.y += ds;
            }
        }
        // Up.
        if (key == BR_KEY_S)
        {
            // If not at max acceleration:
            if (m_acceleration.y >= MAX_NEG_SPEED)
            {
                m_acceleration.y -= ds;
            }
        }

        // Right.
        if (key == BR_KEY_A)
        {
            // If not at max acceleration:
            if (m_acceleration.x >= MAX_NEG_SPEED)
            {
                m_acceleration.x -= ds;
            }
        }
        // Left.
        if (key == BR_KEY_D)
        {
            // If not at max acceleration:
            if (m_acceleration.x <= MAX_POS_SPEED)
            {
                m_acceleration.x += ds;
            }
        }

        // Tilt CCW.
        if (key == BR_KEY_Q)
        {
            // Tilt CCW by 5 degrees.
            m_rotation = ((m_rotation + dr) >= 360 ? 0 : m_rotation + dr);
        }
        // Tilt CW.
        if (key == BR_KEY_E)
        {
            // Tilt CW by 5 degrees.
            m_rotation = ((m_rotation - dr) <= 0 ? 360 : m_rotation - dr);
        }

        // Scale X axis +.
        if (key == BR_KEY_T)
        {
            m_scaleVec.x += ds;
            RecalculateScale();
        }
        // Scale X axis -.
        if (key == BR_KEY_G)
        {
            m_scaleVec.x -= ds;
            RecalculateScale();
        }
        // Scale Y axis +.
        if (key == BR_KEY_Y)
        {
            m_scaleVec.y += ds;
            RecalculateScale();
        }
        // Scale Y axis -.
        if (key == BR_KEY_H)
        {
            m_scaleVec.y -= ds;
            RecalculateScale();
        }
    }
}

void Shape::RecalculateTransform()
{
    m_transform =
      glm::translate(glm::mat4(1.0f), m_position) *
      glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));

    m_ts = m_transform * m_scale;
}

void Shape::RecalculateScale()
{
    m_scale = glm::scale(glm::mat4(1.0f), m_scaleVec);
    m_ts    = m_transform * m_scale;
}

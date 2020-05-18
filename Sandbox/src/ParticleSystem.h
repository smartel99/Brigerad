#pragma once

#include "Brigerad.h"

struct ParticleProps
{
    glm::vec2 position;
    glm::vec2 velocity, velocityVariation;
    glm::vec4 colorBegin, colorEnd;
    float sizeBegin, sizeEnd, sizeVariation;
    float lifeTime = 1.0f;
};

class ParticleSystem
{
    public:
    ParticleSystem();

    void OnUpdate(Brigerad::Timestep ts);
    void OnRender(Brigerad::OrthographicCamera& camera);

    void Emit(const ParticleProps& particleProps);

    private:
    struct Particle
    {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec4 colorBegin, colorEnd;
        float rotation = 0.0f;
        float sizeBegin, sizeEnd;

        float lifeTime      = 1.0f;
        float lifeRemaining = 0.0f;

        bool active = false;
    };

    std::vector<Particle> m_particlePool;
    uint32_t m_poolIndex = 9999;
};
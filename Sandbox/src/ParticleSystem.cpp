#include "ParticleSystem.h"


#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include <random>

class Random
{
    public:
    static void Init() { s_randomEngine.seed(std::random_device()()); }

    static float Float()
    {
        return (float)s_distribution(s_randomEngine) /
               (float)std::numeric_limits<long long>::max();
    }

    private:
    static std::mt19937 s_randomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> s_distribution;
};

std::mt19937 Random::s_randomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_distribution;

ParticleSystem::ParticleSystem(uint32_t maxParticles)
: m_poolIndex(maxParticles - 1)
{
    m_particlePool.resize(maxParticles);
}

void ParticleSystem::OnUpdate(Brigerad::Timestep ts)
{
    for (auto& particle : m_particlePool)
    {
        if (particle.active == false)
        {
            continue;
        }

        if (particle.lifeRemaining <= 0.0f)
        {
            particle.active = false;
            continue;
        }

        particle.lifeRemaining -= ts;
        particle.position += particle.velocity * (float)ts;
        particle.rotation += 0.01f * ts;
    }
}

void ParticleSystem::OnRender(Brigerad::OrthographicCamera& camera)
{
    Brigerad::Renderer2D::BeginScene(camera);
    for (auto& particle : m_particlePool)
    {
        if (particle.active == false)
        {
            continue;
        }

        // Fade away particles.
        float life      = particle.lifeRemaining / particle.lifeTime;
        glm::vec4 color = glm::lerp(particle.colorEnd, particle.colorBegin, life);

        float size = glm::lerp(particle.sizeEnd, particle.sizeBegin, life);

        glm::vec3 position = { particle.position.x, particle.position.y, 1.0f };
        Brigerad::Renderer2D::DrawRotatedQuad(position,
                                              { size, size },
                                              color,
                                              particle.rotation);
    }
    Brigerad::Renderer2D::EndScene();
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
    Particle& particle = m_particlePool[m_poolIndex];
    particle.active    = true;
    particle.position  = particleProps.position;
    particle.rotation  = Random::Float() * 2.0f * glm::pi<float>();

    // Velocity
    particle.velocity = particleProps.velocity;
    particle.velocity.x += particleProps.velocityVariation.x * (Random::Float() - 0.5f);
    particle.velocity.y += particleProps.velocityVariation.y * (Random::Float() - 0.5f);

    // Color
    particle.colorBegin = particleProps.colorBegin;
    particle.colorEnd   = particleProps.colorEnd;

    particle.lifeTime      = particleProps.lifeTime;
    particle.lifeRemaining = particleProps.lifeTime;
    particle.sizeBegin     = particleProps.sizeBegin +
                         particleProps.sizeVariation * (Random::Float() - 0.5f);
    particle.sizeEnd = particleProps.sizeEnd;

    m_poolIndex = --m_poolIndex % m_particlePool.size();
}
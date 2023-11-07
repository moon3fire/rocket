#include "ParticleSystem.h"
#include <random>

class Random
{
public:
	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}
private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

ParticleSystem::ParticleSystem()
{
	m_particlePool.resize(100000);
}

void ParticleSystem::emit(const ParticleProps& particleProps)
{
	Particle& particle = m_particlePool[m_poolIndex];
	particle.active = true;
	particle.position = particleProps.position;
	particle.rotation = Random::Float() * 2.0f * glm::pi<float>();

	// Velocity
	particle.velocity = particleProps.velocity;
	particle.velocity.x += particleProps.velocityVariation.x * (Random::Float() - 0.5f);
	particle.velocity.y += particleProps.velocityVariation.y * (Random::Float() - 0.5f);

	// Color
	particle.colorBegin = particleProps.colorBegin;
	particle.colorEnd = particleProps.colorEnd;

	// Size
	particle.sizeBegin = particleProps.sizeBegin + particleProps.sizeVariation * (Random::Float() - 0.5f);
	particle.sizeEnd = particleProps.sizeEnd;

	// Life
	particle.lifeTime = particleProps.lifeTime;
	particle.lifeRemaining = particleProps.lifeTime;

	m_poolIndex = --m_poolIndex % m_particlePool.size();
}

void ParticleSystem::onUpdate(Rocket::Timestep ts)
{
	for (auto& particle : m_particlePool)
	{
		if (!particle.active)
			continue;

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

void ParticleSystem::onRender(Rocket::OrthographicCamera2D& camera)
{
	Rocket::Renderer2D::beginScene(camera);
	for (auto& particle : m_particlePool)
	{
		if (!particle.active)
			continue;

		float life = particle.lifeRemaining / particle.lifeTime;
		glm::vec4 color = glm::lerp(particle.colorEnd, particle.colorBegin, life);
		//color.a = color.a * life;

		float size = glm::lerp(particle.sizeEnd, particle.sizeBegin, life);
		glm::vec3 position = { particle.position.x, particle.position.y, 0.2f };
		Rocket::Renderer2D::drawQuad3D(position, { size, size }, color, particle.rotation);
	}
	Rocket::Renderer2D::endScene();
}
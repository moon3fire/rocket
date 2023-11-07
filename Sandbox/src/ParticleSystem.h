#pragma once

#include <Rocket.h>

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

	void onUpdate(Rocket::Timestep ts);
	void onRender(Rocket::OrthographicCamera2D& camera);

	void emit(const ParticleProps& particleProps);

private:
	struct Particle
	{
		glm::vec2 position;
		glm::vec2 velocity;
		glm::vec4 colorBegin, colorEnd;
		float rotation = 0.0f;
		float sizeBegin, sizeEnd;

		float lifeTime = 1.0f;
		float lifeRemaining = 0.0f;

		bool active = false;
	};
	std::vector<Particle> m_particlePool;
	uint32_t m_poolIndex = 999;
};

#pragma once

#include "Rocket.h"

class Sandbox2D : public Rocket::Layer {
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	virtual void onUpdate(Rocket::Timestep ts) override;
	virtual void onImGuiRender() override;
	void onEvent(Rocket::Event& event) override;

private:

	struct SubtextureData {
		glm::vec2 coords;
		glm::vec2 scale;
		char tileSymbol;
	};

	Rocket::OrthographicCameraController m_cameraController;

	Rocket::Ref<Rocket::Framebuffer> m_framebuffer;
	Rocket::Ref<Rocket::Texture2D> m_spriteSheet;

	SubtextureData m_subtextureData[50];

	//scene 1
	glm::vec4 m_backgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec2 m_backgroundPosition = { 0.0f, 0.0f };
	glm::vec2 m_backgroundScale = { 10.0f, 10.0f };

	glm::vec4 m_squareColor = { 0.1f, 0.3f, 0.8f, 1.0f };
	glm::vec2 m_squarePosition = { 0.0f, 0.0f };
	glm::vec2 m_squareScale = { 1.0f, 1.0f };
	float m_squareRotation = 0.0f;

	uint32_t m_mapWidth, m_mapHeight;
	std::unordered_map<char, Rocket::Ref<Rocket::SubTexture2D>> m_textureMap;

	glm::vec4 m_particleStartColor = glm::vec4(1.0f);
	glm::vec4 m_particleEndColor = glm::vec4(1.0f);
};
#pragma once

#include "Rocket/Core/Core.h"
#include "Shader.h"

#include <glm/glm.hpp>

//std
#include <vector>

namespace Rocket {

	struct BloomMip {
		glm::vec2 size;
		glm::ivec2 intSize;
		unsigned int texture;
	};

	class BloomFramebuffer {
	public:
		BloomFramebuffer();
		~BloomFramebuffer();
		bool init(uint32_t viewportWidth, uint32_t viewportHeight, unsigned int mipChainLength);
		void destroy();

		void bind();
		void unbind();

		const std::vector<BloomMip>& getMipChain() const { return m_mipChain; }

	private:
		bool m_isActive = false;
		unsigned int m_rendererID;
		std::vector<BloomMip> m_mipChain;
	};

	class CombineFramebuffer {
	public:
		CombineFramebuffer();
		~CombineFramebuffer();

		bool init(uint32_t viewportWidth, uint32_t viewportHeight);
		void destroy();

		void bind();
		void unbind();

		const uint32_t getTexture() const { return m_colorTexture; }

	private:
		bool m_isActive = false;
		unsigned int m_rendererID;
		uint32_t m_colorTexture; // attachment 0
	};

	class BloomRenderer {
	public:
		BloomRenderer();
		~BloomRenderer();
		bool init(uint32_t viewportWidth, uint32_t viewportHeight);
		void destroy();
		void renderBloomTexture(uint32_t srcTexture, float filterRadius);
		uint32_t getBloomTexture() const { return m_bloomFramebuffer.getMipChain()[0].texture; }

		uint32_t combineTextures(uint32_t srcTexture, uint32_t bloomTexture);

		void onResize(uint32_t width, uint32_t height);

	private:
		void renderDownsamples(uint32_t srcTexture);
		void renderUpsamples(float filterRadius);

		bool m_isActive = false;
		BloomFramebuffer m_bloomFramebuffer;
		glm::ivec2 m_srcViewportSize;
		glm::vec2 m_srcViewportSizeFloat;

		Ref<Shader> m_downsampleShader;
		Ref<Shader> m_upsampleShader;

		Ref<Shader> m_combineShader;
		CombineFramebuffer m_combineFramebuffer;

		// fullscreen quad
		unsigned int quadVBO, quadVAO;
		void configureQuad();

	};

} // namespace Rocket

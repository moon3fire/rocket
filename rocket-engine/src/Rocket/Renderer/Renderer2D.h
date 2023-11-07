#pragma once

#include "OrthographicCamera.h"

#include "Texture.h"
#include "SubTexture2D.h"

namespace Rocket {

	class Renderer2D {
	public:
		static void init();
		static void shutdown();

		static void beginScene(const OrthographicCamera2D& camera);
		static void endScene();
		static void flush();

		static void drawQuad2D(const glm::vec2& position = { 0.0f, 0.0f },
							 const glm::vec2& size = { 1.0f, 1.0f },
							 const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
							 float rotation = 0.0f);
		static void drawQuad3D(const glm::vec3& position = { 0.0f, 0.0f, 0.0f },
							 const glm::vec2& size = { 1.0f, 1.0f },
							 const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
							 float rotation = 0.0f);
		static void drawQuad2DWithTexture(const glm::vec2& position = { 0.0f, 0.0f },
			const glm::vec2& size = { 1.0f, 1.0f },
			const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Ref<Texture2D>& texture = nullptr,
			float rotation = 0.0f);
		static void drawQuad3DWithTexture(const glm::vec3& position = { 0.0f, 0.0f, 0.0f },
			const glm::vec2& size = { 1.0f, 1.0f },
			const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Ref<Texture2D>& texture = nullptr,
			float rotation = 0.0f);
		static void drawQuad2DWithSubTexture(const glm::vec2& position = { 0.0f, 0.0f },
			const glm::vec2& size = { 1.0f, 1.0f },
			const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Ref<SubTexture2D>& subtexture = nullptr, // can't be nullptr
			float rotation = 0.0f);
		static void drawQuad3DWithSubTexture(const glm::vec3& position = { 0.0f, 0.0f, 0.0f },
			const glm::vec2& size = { 1.0f, 1.0f },
			const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
			const Ref<SubTexture2D>& subtexture = nullptr,
			float rotation = 0.0f);

		//Stats

		struct Statistics {
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			uint32_t getTotalVertexCount() { return quadCount * 4; }
			uint32_t getTotalIndexCount() { return quadCount * 6; }
		};

		static Statistics getStats();
		static void resetStats();
		
	private:
		static void flushAndReset();
	};

} // namespace Rocket
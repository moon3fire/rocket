#pragma once

#include "OrthographicCamera.h"
#include "Camera.h"

#include "Texture.h"
#include "SubTexture2D.h"
#include "EditorCamera.h"

#include "Rocket/Scene/Components.h"
#include "Framebuffer.h"

namespace Rocket {

	class Renderer2D {
	public:
		static void init();
		static void reset();
		static void shutdown();

		static void initQuadVB();
		static void initCircleVB();
		static void initLineVB();
		static void initIB();

		static void beginScene(const OrthographicCamera2D& camera); // TODO: Remove
		static void beginScene(const Camera& camera, const glm::mat4& transform);
		static void beginScene(const EditorCamera& camera);
		
		// lighting, additional stuff
		static void setEntityID(uint32_t id);
		static void uploadView(const glm::vec3& position); // this one will most likely stay, UPD im not sure
		static void applyDirectionalLights(const std::vector<DirectionalLightComponent>& dirLights, const glm::vec3& viewPosition);
		static void applyPointLights(const std::vector<PointLightComponent>& pointLights);
		static void applySpotLights(const std::vector<SpotLightComponent>& spotLights);
		static void prepareSkybox(/*const std::vector<std::string>& faces* @brief --- will implement later */);
		static void setSkybox(const std::vector<std::string>& faces);
		static void changeSkybox();
		static void applySkybox(const EditorCamera& camera);

		static void enableHDR(bool enabled);
		static void setExposure(float exposure);

		static void enableReflection(bool enabled);
		static void enableRefraction(bool enabled);

		static void endScene();
		static void flush();

		static void drawQuad2D(const glm::vec2& position = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 1.0f }, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float rotation = 0.0f);
		static void drawQuad3D(const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 1.0f }, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, float rotation = 0.0f);
		static void drawQuad2DWithTexture(const glm::vec2& position = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 1.0f }, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, const Ref<Texture2D>& texture = nullptr, float rotation = 0.0f);
		static void drawQuad3DWithTexture(const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 1.0f }, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, const Ref<Texture2D>& texture = nullptr, float rotation = 0.0f);
		static void drawQuad2DWithSubTexture(const glm::vec2& position = { 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 1.0f }, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, const Ref<SubTexture2D>& subtexture = nullptr /* can't be nullptr */, float rotation = 0.0f);
		static void drawQuad3DWithSubTexture(const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec2& size = { 1.0f, 1.0f }, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f }, const Ref<SubTexture2D>& subtexture = nullptr, float rotation = 0.0f);

		static void drawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID = -1);
		static void drawCircle(const glm::mat4& transform, CircleRendererComponent& crc, int entityID = -1);
		
		static void drawQuadShape(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void drawQuadShape(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static void drawLine(const glm::vec3& lineStartPoint, const glm::vec3& likeEndPoint, const glm::vec4& color, int entityID = -1);
		
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
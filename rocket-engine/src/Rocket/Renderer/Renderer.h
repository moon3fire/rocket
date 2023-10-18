#pragma once

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "Shader.h"

namespace Rocket {

	class Renderer {

	public:
		static void beginScene(OrthographicCamera2D& camera); // TODO: scene parameters
		static void endScene();

		static void sumbit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1));

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }

	private:
		struct SceneData {
			glm::mat4 viewProjectionMatrix;
		};

		static SceneData* m_sceneData;
	};

} // namespace Rocket
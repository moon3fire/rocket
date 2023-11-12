#pragma once

#include "RenderCommand.h"
#include "OrthographicCamera.h"
#include "OrthographicCameraController.h"
#include "Shader.h"

namespace Rocket {

	class Renderer {

	public:

		static void init();

		static void onWindowResize();

		static void beginScene(OrthographicCamera2D& camera); // TODO: scene parameters
		static void endScene();

		static void submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform = glm::mat4(1));

		static void setCameraController(const OrthographicCameraController& controller) { m_cameraController = createRef<OrthographicCameraController>(controller); }
		static Ref<OrthographicCameraController> getCameraController() { return m_cameraController; }

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }

	private:
		struct SceneData {
			glm::mat4 viewProjectionMatrix;
		};

		static Scope<SceneData> m_sceneData;
		static Ref<OrthographicCameraController> m_cameraController;
	};

} // namespace Rocket
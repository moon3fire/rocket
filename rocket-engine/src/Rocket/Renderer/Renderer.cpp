#include "rcktpch.h"
#include "Renderer.h"

namespace Rocket {

	Renderer::SceneData* Renderer::m_sceneData = new Renderer::SceneData;

	void Renderer::beginScene(OrthographicCamera2D& camera) {
		m_sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene() {

	}

	void Renderer::sumbit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader) {
		vertexArray->bind();
		shader->uploadUniformMat4("u_ViewProjection", m_sceneData->viewProjectionMatrix);
		RenderCommand::drawIndexed(vertexArray);
	}

} // namespace Rocket
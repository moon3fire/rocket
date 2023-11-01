#include "rcktpch.h"
#include "Renderer.h"
#include "Renderer2D.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Rocket {

	Scope<Renderer::SceneData> Renderer::m_sceneData =  createScope<Renderer::SceneData>();

	void Renderer::init() {
		RCKT_PROFILE_FUNCTION();

		RenderCommand::init();
		Renderer2D::init();
	}

	void Renderer::onWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::setViewport(0, 0, width, height);
	}

	void Renderer::beginScene(OrthographicCamera2D& camera) {
		m_sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene() {

	}

	void Renderer::submit(const Ref<VertexArray>& vertexArray, const Ref<Shader>& shader, const glm::mat4& transform) {
		vertexArray->bind();
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_ViewProjection", m_sceneData->viewProjectionMatrix);
		std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_modelMatrix", transform);
		RenderCommand::drawIndexed(vertexArray);
	}

} // namespace Rocket
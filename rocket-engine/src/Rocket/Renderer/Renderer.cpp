#include "rcktpch.h"
#include "Renderer.h"

namespace Rocket {

	void Renderer::beginScene() {

	}

	void Renderer::endScene() {

	}

	void Renderer::sumbit(const std::shared_ptr<VertexArray>& vertexArray) {
		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}

} // namespace Rocket
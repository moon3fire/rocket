#pragma once

#include "RenderCommand.h"

namespace Rocket {

	class Renderer {

	public:
		static void beginScene(); // TODO: scene parameters
		static void endScene();

		static void sumbit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	};

} // namespace Rocket
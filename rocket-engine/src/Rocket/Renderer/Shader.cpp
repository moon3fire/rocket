#include "rcktpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Rocket {

	Shader* Shader::create(const std::string& vertexSource, const std::string& fragmentSource) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:		RCKT_CORE_ASSERT(false, "RendererAPI not found: None");
			case RendererAPI::API::OpenGL:		return new OpenGLShader(vertexSource, fragmentSource);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

} // namespace Rocket
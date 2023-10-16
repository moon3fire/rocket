#include "rcktpch.h"
#include "VertexArray.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Renderer.h"

namespace Rocket {

	VertexArray* VertexArray::create() {
		switch (Renderer::getAPI()) {

		case RendererAPI::API::None:		RCKT_CORE_ASSERT(false, "RendererAPI not found: None");
		case RendererAPI::API::OpenGL:		return new OpenGLVertexArray();
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

} // namespace Rocket
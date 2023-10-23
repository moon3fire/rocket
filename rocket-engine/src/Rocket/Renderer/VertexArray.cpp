#include "rcktpch.h"
#include "VertexArray.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Renderer.h"

namespace Rocket {

	Ref<VertexArray> VertexArray::create() {
		switch (Renderer::getAPI()) {

		case RendererAPI::API::None:		RCKT_CORE_ASSERT(false, "RendererAPI not found: None");
		case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLVertexArray>();
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

} // namespace Rocket
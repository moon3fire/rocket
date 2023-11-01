#include "rcktpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Rocket {
	
	Ref<VertexBuffer> VertexBuffer::create(uint32_t size) {
		switch (Renderer::getAPI()) {

			case RendererAPI::API::None:    RCKT_CORE_ASSERT(false, "Renderer API is None");
			case RendererAPI::API::OpenGL:  return createRef<OpenGLVertexBuffer>(size);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::create(float* vertices, uint32_t size) {
		switch (Renderer::getAPI()) {
	
			case RendererAPI::API::None:    RCKT_CORE_ASSERT(false, "Renderer API is None");
			case RendererAPI::API::OpenGL:  return createRef<OpenGLVertexBuffer>(vertices, size);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}


	Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t count) {
		switch (Renderer::getAPI()) {

			case RendererAPI::API::None:    RCKT_CORE_ASSERT(false, "Renderer API is None");
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLIndexBuffer>(indices, count);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}


} // namespace Rocket
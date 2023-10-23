#include "rcktpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Rocket {

	Ref<Texture2D> Texture2D::create(const std::string& path) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:		RCKT_CORE_ASSERT(false, "RendererAPI not found: None");
			case RendererAPI::API::OpenGL:		return createRef<OpenGLTexture2D>(path);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::create(int width, int height) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:		RCKT_CORE_ASSERT(false, "RendererAPI not found: None");
			case RendererAPI::API::OpenGL:		return createRef<OpenGLTexture2D>(width, height);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

} // namespace Rocket
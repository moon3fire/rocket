#include "rcktpch.h"

#include "Skybox.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLSkybox.h"

namespace Rocket {

	Ref<Skybox> Skybox::create(const std::vector<std::string>& faces)
	{
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None:		RCKT_CORE_ASSERT(false, "RendererAPI not found: None");
			case RendererAPI::API::OpenGL:		return std::make_shared<OpenGLSkybox>(faces);
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

} // namespace Rocket
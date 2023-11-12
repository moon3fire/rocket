#include "rcktpch.h"
#include "Framebuffer.h"

#include "Rocket/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Rocket {

	Ref<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec)
	{
		switch (Renderer::getAPI()) {
		
			case RendererAPI::API::None: RCKT_CORE_ASSERT(false, "Renderer API is not chosen!"); return nullptr;
			case RendererAPI::API::OpenGL: return createRef<OpenGLFramebuffer>(spec);
		
		}

		RCKT_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

} // namespace Rocket
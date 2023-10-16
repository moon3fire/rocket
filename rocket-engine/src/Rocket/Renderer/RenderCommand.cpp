#include "rcktpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPi.h"

namespace Rocket {

	RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI;

} // namespace Rocket
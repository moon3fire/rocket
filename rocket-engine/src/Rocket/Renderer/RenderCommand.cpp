#include "rcktpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPi.h"

namespace Rocket {

	Scope<RendererAPI> RenderCommand::s_rendererAPI = createScope<OpenGLRendererAPI>();

} // namespace Rocket
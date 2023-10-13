#pragma once

#include "Rocket/Renderer/GraphicContext.h"
#include "GLFW/glfw3.h"

class GLFWwindow;

namespace Rocket {
	class OpenGLContext: public GraphicContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_windowHandle;
	};
} // namespace Rocket

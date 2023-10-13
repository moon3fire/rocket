#pragma once

#include "Rocket/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"

class GLFWwindow;

namespace Rocket {
	
	class OpenGLContext: public GraphicsContext {

	public:
		
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void init() override;
		virtual void swapBuffers() override;
	
	private:
		
		GLFWwindow* m_windowHandle;
	};

} // namespace Rocket

#include "rcktpch.h"
#include "OpenGLContext.h"
#include "glad/glad.h"

namespace Rocket {
	
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_windowHandle(windowHandle) {
		RCKT_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::init() {
		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RCKT_CORE_ASSERT(status, "Failed to Initialize Glad");
	}

	void OpenGLContext::swapBuffers() {
		glfwSwapBuffers(m_windowHandle);
	}

} // namespace Rocket
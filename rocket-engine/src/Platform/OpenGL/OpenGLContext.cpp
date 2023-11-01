#include "rcktpch.h"
#include "OpenGLContext.h"
#include "glad/glad.h"

namespace Rocket {
	
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_windowHandle(windowHandle) {
		RCKT_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::init() {
		RCKT_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RCKT_CORE_ASSERT(status, "Failed to Initialize Glad");

		RCKT_CORE_INFO("Rocket info:");
		RCKT_CORE_INFO("   Vendor:  {0}", glGetString(GL_VENDOR));
		RCKT_CORE_INFO("   Renderer:  {0}", glGetString(GL_RENDERER));
		RCKT_CORE_INFO("   Version:  {0}", glGetString(GL_VERSION));

#ifdef RCKT_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		RCKT_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Requires at least OpenGL version 4.5!");
#endif

	}

	void OpenGLContext::swapBuffers() {
		RCKT_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandle);
	}

} // namespace Rocket
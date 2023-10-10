#include "rcktpch.h"
#include "Application.h"

#include "Rocket/Events/ApplicationEvent.h"
#include "Rocket/Log.h"

#include "GLFW/glfw3.h"

namespace Rocket {

	Application::Application() :m_running(false) {
		m_window = std::unique_ptr<Window>(Window::create());
	}

	Application::~Application() {}

	void Application::run() {
		m_running = true;

		while (m_running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_window->onUpdate();
		}
	}

} // namespace Rocket
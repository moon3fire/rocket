#include "Application.h"

#include "Rocket/Events/ApplicationEvent.h"
#include "Rocket/Log.h"

namespace Rocket {

	Application::Application() {}

	Application::~Application() {}

	void Application::run() {
		WindowResizeEvent event(1280, 720);
		RCKT_INFO(event);
	}

} // namespace Rocket
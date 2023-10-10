#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace Rocket {

	class ROCKET_API Application {
	public:
		Application();
		virtual ~Application();
		
		void run();



	private:
		std::unique_ptr<Window> m_window;
		bool m_running;
	};

	//to be defined in client
	Application* createApplication();

} // namespace Rocket

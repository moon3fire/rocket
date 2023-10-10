#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Rocket {

	class ROCKET_API Application {
	public:
		Application();
		virtual ~Application();
		
		void run();


		//to be defined in client
	};

	Application* createApplication();

} // namespace Rocket

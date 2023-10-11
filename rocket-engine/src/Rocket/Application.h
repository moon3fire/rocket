#pragma once

#include "Core.h"

#include "Events/Event.h"
#include "Window.h"

#include "Rocket/Events/ApplicationEvent.h"
#include "Rocket/LayerStack.h"

namespace Rocket {

	class ROCKET_API Application {
	public:
		Application();
		virtual ~Application();
		
		void run();
		void onEvent(Event& event);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		inline static Application& get() { return *s_instance; }

		inline Window& getWindow() { return *m_window; }
	private:
		bool onWindowClose(WindowCloseEvent& event);

		std::unique_ptr<Window> m_window;
		bool m_running;
		LayerStack m_layerStack;

		static Application* s_instance;
	};

	//to be defined in client
	Application* createApplication();

} // namespace Rocket

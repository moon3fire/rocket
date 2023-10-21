#pragma once

#include "Core.h"

#include "Rocket/Events/Event.h"
#include "Window.h"

#include "Rocket/Events/ApplicationEvent.h"
#include "Rocket/Core/LayerStack.h"
#include "Rocket/ImGui/ImGuiLayer.h"

#include "Rocket/Core/Timestep.h"


#include "Rocket/Renderer/Buffer.h"
#include "Rocket/Renderer/VertexArray.h"
#include "Rocket/Renderer/Shader.h"

#include "Rocket/Renderer/OrthographicCamera.h"

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
		bool onWindowResize(WindowResizeEvent& event);

	private:
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		bool m_running = true;
		bool m_minimized = false;
		LayerStack m_layerStack;
		float m_LastFrameTime = 0.0f;
		static Application* s_instance;
	};

	//to be defined in client
	Application* createApplication();

} // namespace Rocket

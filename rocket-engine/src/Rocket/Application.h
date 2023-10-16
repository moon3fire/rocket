#pragma once

#include "Core.h"

#include "Events/Event.h"
#include "Window.h"

#include "Rocket/Events/ApplicationEvent.h"
#include "Rocket/LayerStack.h"
#include "Rocket/ImGui/ImGuiLayer.h"

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Rocket/Renderer/Shader.h"

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
		ImGuiLayer* m_imguiLayer;
		bool m_running;
		LayerStack m_layerStack;

		//temp
		std::shared_ptr<VertexArray> m_triangleVA;
		std::shared_ptr<Shader> m_triangleShader;
		
		std::shared_ptr<Shader> m_squareShader;
		std::shared_ptr<VertexArray> m_squareVA;

		static Application* s_instance;
	};

	//to be defined in client
	Application* createApplication();

} // namespace Rocket

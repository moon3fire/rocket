#include "rcktpch.h"
#include "Application.h"

#include "Rocket/Core/Log.h"

#include "Input.h"
#include "Rocket/Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Rocket {

#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application() :m_running(false) {

		RCKT_PROFILE_FUNCTION();

		RCKT_CORE_ASSERT(!s_instance, "Application already exist!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FUNC(Application::onEvent));

		Renderer::init();

		m_imguiLayer = new ImGuiLayer();
		pushOverlay(m_imguiLayer);
	}

	Application::~Application() {}

	void Application::onEvent(Event& event) {
		RCKT_PROFILE_FUNCTION();

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(Application::onWindowResize));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			if (event.handled)
				break;
			(*--it)->onEvent(event);
		}
	}

	void Application::pushLayer(Layer* layer) {
		RCKT_PROFILE_FUNCTION();

		m_layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* overlay) {
		RCKT_PROFILE_FUNCTION();

		m_layerStack.pushOverlay(overlay);
		overlay->onAttach();
	}

	void Application::run() {
		RCKT_PROFILE_FUNCTION();

		m_running = true;

		while (m_running) {

			RCKT_PROFILE_SCOPE("Run loop");

			float time = (float)glfwGetTime(); // pLatform dependent get_time()
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_minimized) {
				{
					RCKT_PROFILE_SCOPE("LayerStack onUpdate:");
					for (Layer* layer : m_layerStack) {
						layer->onUpdate(timestep);
					}
				}
				m_imguiLayer->begin();
				{
					RCKT_PROFILE_SCOPE("ImguiLayerStack onImguiRender:");
					for (Layer* layer : m_layerStack) {
						layer->onImGuiRender();
					}
					m_imguiLayer->end();
				}
			}

			
			
			m_window->onUpdate();
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& event) {
		m_running = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& event) {
		RCKT_PROFILE_FUNCTION();

		if (event.getWidth() == 0 || event.getHeight() == 0) {
			m_minimized = true;
			return false;
		}
		else 
			m_minimized = false;
		Renderer::onWindowResize(event.getWidth(), event.getHeight());
		return false;
	}


} // namespace Rocket
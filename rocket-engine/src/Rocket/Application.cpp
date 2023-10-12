#include "rcktpch.h"
#include "Application.h"

#include "Rocket/Log.h"

#include "glad/glad.h"

#include "Input.h"

namespace Rocket {

#define BIND_EVENT_FUNC(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application() :m_running(false) {
		RCKT_CORE_ASSERT(!s_instance, "Application already exist!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FUNC(Application::onEvent));

		m_imguiLayer = new ImGuiLayer();
		pushOverlay(m_imguiLayer);
	}

	Application::~Application() {}

	void Application::onEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::onWindowClose));

		//RCKT_CORE_TRACE("{0} ", event);

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(event);
			if (event.m_handled)
				break;
		}
	}

	void Application::pushLayer(Layer* layer) {
		m_layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* overlay) {
		m_layerStack.pushOverlay(overlay);
		overlay->onAttach();
	}

	void Application::run() {
		m_running = true;

		while (m_running) {
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);


			for (Layer* layer : m_layerStack) {
				layer->onUpdate();
			}
			m_imguiLayer->begin();
			for (Layer* layer : m_layerStack) {
				layer->onImGuiRender();
			}
			m_imguiLayer->end();

			m_window->onUpdate();
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& event) {
		m_running = false;
		return true;
	}

} // namespace Rocket
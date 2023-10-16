#include "rcktpch.h"
#include "Application.h"

#include "Rocket/Log.h"

#include "Input.h"
#include "Renderer/Renderer.h"

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

		{
			m_triangleVA.reset(VertexArray::create());

			float triangleVertices[7 * 3] = {
				-0.5f, -0.5f, 0.0f, 0.2f, 0.1f, 0.9f, 1.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
				 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 0.9f, 1.0f
			};

			std::shared_ptr<VertexBuffer> triangleVertexBuffer;
			triangleVertexBuffer.reset(VertexBuffer::create(triangleVertices, sizeof(triangleVertices)));
			/*
			BufferLayout triangleLayout = {
				{ ShaderDataType::Float3, "a_position" },
				{ ShaderDataType::Float4, "a_color" }
			};
			*/
			triangleVertexBuffer->setLayout(
			{
				{ ShaderDataType::Float3, "a_position" },
				{ ShaderDataType::Float4, "a_color" } 
			});

			m_triangleVA->addVertexBuffer(triangleVertexBuffer);
			uint32_t triangleIndices[3] = { 0, 1, 2 };
			
			std::shared_ptr<IndexBuffer> triangleIndexBuffer;
			triangleIndexBuffer.reset(IndexBuffer::create(triangleIndices, sizeof(triangleIndexBuffer) / sizeof(uint32_t)));

			m_triangleVA->setIndexBuffer(triangleIndexBuffer);
		}

		{
			m_squareVA.reset(VertexArray::create());

			float squareVertices[7 * 4] = {
					-0.8f, -0.8f, 0.0f, 0.8f, 0.3f, 0.6f, 1.0f,
					 0.8f, -0.8f, 0.0f, 0.4f, 0.9f, 0.1f, 1.0f,
					 0.8f,  0.8f, 0.0f, 0.7f, 0.1f, 0.2f, 1.0f,
					-0.8f,  0.8f, 0.0f, 0.3f, 0.4f, 0.5f, 1.0f
			};

			std::shared_ptr<VertexBuffer> squareVertexBuffer;
			squareVertexBuffer.reset(VertexBuffer::create(squareVertices, sizeof(squareVertices)));

			squareVertexBuffer->setLayout(
			{
				{ ShaderDataType::Float3, "a_position" },
				{ ShaderDataType::Float4, "a_color" }
			});

			m_squareVA->addVertexBuffer(squareVertexBuffer);
			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

			std::shared_ptr<IndexBuffer> squareIndexBuffer;
			squareIndexBuffer.reset(IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

			m_squareVA->setIndexBuffer(squareIndexBuffer);
		}

		
		std::string triangleVertexShaderSource = R"(
			#version 330 core
			
			layout (location = 0) in vec3 a_position;
			layout (location = 1) in vec4 a_color;
			out vec4 frag_color;

			void main()
			{
				frag_color = a_color;
				gl_Position = vec4(a_position, 1.0);		
			}
		)";

		std::string triangleFragmentShaderSource = R"(
			#version 330 core

			in vec4 frag_color;			
			layout (location = 0) out vec4 color;	
		
			void main()
			{
				color = frag_color;
			}
		)";

		std::string squareVertexShaderSource = R"(
			#version 330 core
			
			layout (location = 0) in vec3 a_position;
			layout (location = 1) in vec4 a_color;
			out vec4 frag_color;

			void main()
			{
				frag_color = a_color;
				gl_Position = vec4(a_position, 1.0);		
			}
		)";

		std::string squareFragmentShaderSource = R"(
			#version 330 core

			in vec4 frag_color;			
			layout (location = 0) out vec4 color;	
		
			void main()
			{
				color = frag_color;
			}
		)";

		m_triangleShader.reset(new Shader(triangleVertexShaderSource, triangleFragmentShaderSource));
		m_squareShader.reset(new Shader(squareVertexShaderSource, squareFragmentShaderSource));
	
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
			
			RenderCommand::setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
			RenderCommand::clear();

			Renderer::beginScene();
			{
				m_squareShader->bind();
				Renderer::sumbit(m_squareVA);

				m_triangleShader->bind();
				Renderer::sumbit(m_triangleVA);
			}
			Renderer::endScene();


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
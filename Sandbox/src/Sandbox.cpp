#include "rcktpch.h"
#include "Sandbox2D.h"

class ExampleLayer : public Rocket::Layer {
public:
	ExampleLayer() :Layer("example"), m_cameraController(1280.0f / 720.0f, true) {
		{
			m_triangleVA = Rocket::VertexArray::create();

			float triangleVertices[7 * 3] = {
				-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
			};

			Rocket::Ref<Rocket::VertexBuffer> triangleVertexBuffer;
			triangleVertexBuffer = Rocket::VertexBuffer::create(triangleVertices, sizeof(triangleVertices));
			/*
			BufferLayout triangleLayout = {
				{ ShaderDataType::Float3, "a_position" },
				{ ShaderDataType::Float4, "a_color" }
			};
			*/
			triangleVertexBuffer->setLayout(
				{
					{ Rocket::ShaderDataType::Float3, "a_position" },
					{ Rocket::ShaderDataType::Float4, "a_color" }
				});

			m_triangleVA->addVertexBuffer(triangleVertexBuffer);
			uint32_t triangleIndices[3] = { 0, 1, 2 };

			Rocket::Ref<Rocket::IndexBuffer> triangleIndexBuffer;
			triangleIndexBuffer = Rocket::IndexBuffer::create(triangleIndices, sizeof(triangleIndexBuffer) / sizeof(uint32_t));

			m_triangleVA->setIndexBuffer(triangleIndexBuffer);
		}

		{
			m_squareVA = Rocket::VertexArray::create();

			float squareVertices[5 * 4] = {
					-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
					 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
					 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
					-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
			};

			Rocket::Ref<Rocket::VertexBuffer> squareVertexBuffer;
			squareVertexBuffer = Rocket::VertexBuffer::create(squareVertices, sizeof(squareVertices));

			squareVertexBuffer->setLayout(
				{
					{ Rocket::ShaderDataType::Float3, "a_position" },
					{ Rocket::ShaderDataType::Float2, "a_textureCoord" }
				});

			m_squareVA->addVertexBuffer(squareVertexBuffer);
			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

			Rocket::Ref<Rocket::IndexBuffer> squareIndexBuffer;
			squareIndexBuffer = Rocket::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

			m_squareVA->setIndexBuffer(squareIndexBuffer);
		}

		m_triangleShader = Rocket::Shader::create("assets/shaders/Triangle.glsl");
		m_squareShader = Rocket::Shader::create("assets/shaders/Square.glsl");
		auto textureShader = m_shaderLibrary.load("assets/shaders/Texture.glsl");
		auto triangleShader = m_shaderLibrary.load("assets/shaders/Triangle.glsl");
		RCKT_INFO(textureShader.get()->getName());
		RCKT_INFO(m_triangleShader.get()->getName());
		RCKT_INFO(m_squareShader.get()->getName());

		m_texture = Rocket::Texture2D::create("assets/textures/wood.png");
		std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->bind();
		std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->uploadUniformInt("u_texture", 0);
		
		m_squareColor = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	void onUpdate(Rocket::Timestep ts) override {

		//update
		//RCKT_TRACE("Delta time {0}s ({1} ms)", ts.GetSeconds(), ts.GetMiliseconds());
		m_cameraController.onUpdate(ts);

		//render
		Rocket::RenderCommand::setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		Rocket::RenderCommand::clear();

		Rocket::Renderer::beginScene(m_cameraController.getCamera());
			m_texture->bind(0);

			
			glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->bind();
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->uploadUniformFloat3("u_squareColor", m_squareColor);

			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 myTransform = glm::translate(glm::mat4(1.0), pos) * scale;
					std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->uploadUniformMat4("u_viewProjection", m_cameraController.getCamera().getViewProjectionMatrix());
					
					std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->bind();
					Rocket::Renderer::submit(m_squareVA, m_squareShader, myTransform);
					
				}
			}

			auto textureShader = m_shaderLibrary.get("Texture");
			
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->bind();
			Rocket::Renderer::submit(m_squareVA, textureShader, glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 0.f))));
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->uploadUniformMat4("u_viewProjection", m_cameraController.getCamera().getViewProjectionMatrix());
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->unbind();

			/*
			auto triangleShader = m_shaderLibrary.get("Triangle");
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(triangleShader)->bind();
			Rocket::Renderer::submit(m_triangleVA, triangleShader, glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(1.f, 1.f, 1.f))));
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(triangleShader)->uploadUniformMat4("u_viewProjection", m_cameraController.getCamera().getViewProjectionMatrix());
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->unbind();
			*/

			//m_triangleShader->bind();
			//std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_triangleShader)->uploadUniformMat4("u_viewProjection", m_camera.getViewProjectionMatrix());
			//Rocket::Renderer::sumbit(m_triangleVA, m_triangleShader);

		Rocket::Renderer::endScene();
	}

	virtual void onImGuiRender() override {
		ImGui::Begin("Square settings");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::ColorEdit3("Square color", glm::value_ptr(m_squareColor));
		ImGui::End();
	}

	void onEvent(Rocket::Event& event) override {
		m_cameraController.onEvent(event);
	}

private:
	Rocket::ShaderLibrary m_shaderLibrary;

	//temp
	Rocket::Ref<Rocket::VertexArray> m_triangleVA;
	Rocket::Ref<Rocket::Shader> m_triangleShader;

	Rocket::Ref<Rocket::Shader> m_squareShader;
	Rocket::Ref<Rocket::VertexArray> m_squareVA;
	glm::vec3 m_squareColor{};

	Rocket::OrthographicCameraController m_cameraController;

	Rocket::Ref<Rocket::Texture2D> m_texture;
};

class Sandbox : public Rocket::Application {
public:
	Sandbox() {
		pushLayer(new Sandbox2D());
	}

	~Sandbox() {

	}
};

Rocket::Application* Rocket::createApplication() {
	return new Sandbox();
}
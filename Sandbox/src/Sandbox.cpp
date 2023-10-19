#include "rcktpch.h"
#include <Rocket.h>

#include "../vendors/imgui/imgui.h"

class ExampleLayer : public Rocket::Layer {
public:
	ExampleLayer() :Layer("example"), m_camera(-1.6f, 1.6f, -.9f, .9f) {
		{
			m_triangleVA.reset(Rocket::VertexArray::create());

			float triangleVertices[7 * 3] = {
				-0.5f, -0.5f, 0.0f, 0.2f, 0.1f, 0.9f, 1.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
				 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 0.9f, 1.0f
			};

			std::shared_ptr<Rocket::VertexBuffer> triangleVertexBuffer;
			triangleVertexBuffer.reset(Rocket::VertexBuffer::create(triangleVertices, sizeof(triangleVertices)));
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

			std::shared_ptr<Rocket::IndexBuffer> triangleIndexBuffer;
			triangleIndexBuffer.reset(Rocket::IndexBuffer::create(triangleIndices, sizeof(triangleIndexBuffer) / sizeof(uint32_t)));

			m_triangleVA->setIndexBuffer(triangleIndexBuffer);
		}

		{
			m_squareVA.reset(Rocket::VertexArray::create());

			float squareVertices[7 * 4] = {
					-0.8f, -0.8f, 0.0f, 0.8f, 0.3f, 0.6f, 1.0f,
					 0.8f, -0.8f, 0.0f, 0.4f, 0.9f, 0.1f, 1.0f,
					 0.8f,  0.8f, 0.0f, 0.7f, 0.1f, 0.2f, 1.0f,
					-0.8f,  0.8f, 0.0f, 0.3f, 0.4f, 0.5f, 1.0f
			};

			std::shared_ptr<Rocket::VertexBuffer> squareVertexBuffer;
			squareVertexBuffer.reset(Rocket::VertexBuffer::create(squareVertices, sizeof(squareVertices)));

			squareVertexBuffer->setLayout(
				{
					{ Rocket::ShaderDataType::Float3, "a_position" },
					{ Rocket::ShaderDataType::Float4, "a_color" }
				});

			m_squareVA->addVertexBuffer(squareVertexBuffer);
			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

			std::shared_ptr<Rocket::IndexBuffer> squareIndexBuffer;
			squareIndexBuffer.reset(Rocket::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

			m_squareVA->setIndexBuffer(squareIndexBuffer);
		}


		std::string triangleVertexShaderSource = R"(
			#version 330 core
			
			layout (location = 0) in vec3 a_position;
			layout (location = 1) in vec4 a_color;

			uniform mat4 u_viewProjection;

			out vec4 frag_color;

			void main()
			{
				frag_color = a_color;
				gl_Position = u_viewProjection * vec4(a_position, 1.0);		
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

			uniform mat4 u_viewProjection;

			out vec4 frag_color;

			void main()
			{
				frag_color = a_color;
				gl_Position = u_viewProjection * vec4(a_position, 1.0);		
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

		m_triangleShader.reset(new Rocket::Shader(triangleVertexShaderSource, triangleFragmentShaderSource));
		m_squareShader.reset(new Rocket::Shader(squareVertexShaderSource, squareFragmentShaderSource));

	}

	void onUpdate(Rocket::Timestep ts) override {

		RCKT_TRACE("Delta time {0}s ({1} ms)", ts.GetSeconds(), ts.GetMiliseconds());

		if (Rocket::Input::isKeyPressed(RCKT_KEY_LEFT)) {
			m_camera.setPosition(glm::vec3(m_camera.getPosition().x + m_cameraSpeed * ts, m_camera.getPosition().y, m_camera.getPosition().z));
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_RIGHT)) {
			m_camera.setPosition(glm::vec3(m_camera.getPosition().x - m_cameraSpeed * ts, m_camera.getPosition().y, m_camera.getPosition().z));
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_DOWN)) {
			m_camera.setPosition(glm::vec3(m_camera.getPosition().x, m_camera.getPosition().y + m_cameraSpeed * ts, m_camera.getPosition().z));
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_UP)) {
			m_camera.setPosition(glm::vec3(m_camera.getPosition().x, m_camera.getPosition().y - m_cameraSpeed * ts, m_camera.getPosition().z));
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_A)) {
			m_rotation -= m_rotationSpeed * ts;
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_D)) {
			m_rotation += m_rotationSpeed * ts;
		}


		Rocket::RenderCommand::setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		Rocket::RenderCommand::clear();

		//m_camera.setPosition({ 0.0f, 0.0f, 0.0f });
		m_camera.setRotation(m_rotation);
		//rotationPerFrame += 0.01;

		Rocket::Renderer::beginScene(m_camera);
		{
			m_squareShader->bind();
			m_squareShader->uploadUniformMat4("u_viewProjection", m_camera.getViewProjectionMatrix());
			Rocket::Renderer::sumbit(m_squareVA, m_squareShader);

			m_triangleShader->bind();
			m_triangleShader->uploadUniformMat4("u_viewProjection", m_camera.getViewProjectionMatrix());
			Rocket::Renderer::sumbit(m_triangleVA, m_triangleShader);
		}
		Rocket::Renderer::endScene();
	}

	virtual void onImGuiRender() override {

	}

	void onEvent(Rocket::Event& event) override {
	
	}

private:

	//temp
	std::shared_ptr<Rocket::VertexArray> m_triangleVA;
	std::shared_ptr<Rocket::Shader> m_triangleShader;

	std::shared_ptr<Rocket::Shader> m_squareShader;
	std::shared_ptr<Rocket::VertexArray> m_squareVA;

	Rocket::OrthographicCamera2D m_camera;

	float m_rotationSpeed = 10.0f;
	float m_rotation = 0.0f;
	float m_cameraSpeed = 1.0f;

};

class Sandbox : public Rocket::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};

Rocket::Application* Rocket::createApplication() {
	return new Sandbox();
}
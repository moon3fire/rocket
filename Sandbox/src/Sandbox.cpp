#include "rcktpch.h"
#include <Rocket.h>

#include "Platform/OpenGL/OpenGLShader.h"

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

			Rocket::Ref<Rocket::VertexBuffer> triangleVertexBuffer;
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

			Rocket::Ref<Rocket::IndexBuffer> triangleIndexBuffer;
			triangleIndexBuffer.reset(Rocket::IndexBuffer::create(triangleIndices, sizeof(triangleIndexBuffer) / sizeof(uint32_t)));

			m_triangleVA->setIndexBuffer(triangleIndexBuffer);
		}

		{
			m_squareVA.reset(Rocket::VertexArray::create());

			float squareVertices[5 * 4] = {
					-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
					 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
					 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
					-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
			};

			Rocket::Ref<Rocket::VertexBuffer> squareVertexBuffer;
			squareVertexBuffer.reset(Rocket::VertexBuffer::create(squareVertices, sizeof(squareVertices)));

			squareVertexBuffer->setLayout(
				{
					{ Rocket::ShaderDataType::Float3, "a_position" },
					{ Rocket::ShaderDataType::Float2, "a_textureCoord" }
				});

			m_squareVA->addVertexBuffer(squareVertexBuffer);
			uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

			Rocket::Ref<Rocket::IndexBuffer> squareIndexBuffer;
			squareIndexBuffer.reset(Rocket::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

			m_squareVA->setIndexBuffer(squareIndexBuffer);
		}

		m_triangleShader = Rocket::Shader::create("assets/shaders/Triangle.glsl");
		m_squareShader = Rocket::Shader::create("assets/shaders/Square.glsl");
		auto textureShader = m_shaderLibrary.load("assets/shaders/Texture.glsl");
		RCKT_INFO(textureShader.get()->getName());
		RCKT_INFO(m_triangleShader.get()->getName());
		RCKT_INFO(m_squareShader.get()->getName());

		m_texture = Rocket::Texture2D::create("assets/textures/wood.png");
		std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->bind();
		std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->uploadUniformInt("u_texture", 0);
		
		m_squareColor = glm::vec3(1.0f, 1.0f, 1.0f);
	}

	void onUpdate() override {

		if (Rocket::Input::isKeyPressed(RCKT_KEY_LEFT)) {
			m_camera.setPosition(glm::vec3(m_camera.getPosition().x - m_cameraSpeed, m_camera.getPosition().y, m_camera.getPosition().z));
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_RIGHT)) {
			m_camera.setPosition(glm::vec3(m_camera.getPosition().x + m_cameraSpeed, m_camera.getPosition().y, m_camera.getPosition().z));
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_DOWN)) {
			m_camera.setPosition(glm::vec3(m_camera.getPosition().x, m_camera.getPosition().y - m_cameraSpeed, m_camera.getPosition().z));
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_UP)) {
			m_camera.setPosition(glm::vec3(m_camera.getPosition().x, m_camera.getPosition().y + m_cameraSpeed, m_camera.getPosition().z));
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_A)) {
			m_rotation += m_rotationSpeed;
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_D)) {
			m_rotation -= m_rotationSpeed;
		}


		//_______________________________________________________________________________________________________________
		if (Rocket::Input::isKeyPressed(RCKT_KEY_I)) {
			m_squarePosition.y += m_squareMoveSpeed;
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_J)) {
			m_squarePosition.x -= m_squareMoveSpeed;
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_K)) {
			m_squarePosition.y -= m_squareMoveSpeed;
		}
		if (Rocket::Input::isKeyPressed(RCKT_KEY_L)) {
			m_squarePosition.x += m_squareMoveSpeed;
		}

		glm::mat4 squareTransform = glm::translate(glm::mat4(1.0f), m_squarePosition);



		Rocket::RenderCommand::setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		Rocket::RenderCommand::clear();

		//m_camera.setPosition({ 0.0f, 0.0f, 0.0f });
		m_camera.setRotation(m_rotation);
		//rotationPerFrame += 0.01;

		Rocket::Renderer::beginScene(m_camera);
			m_texture->bind(0);

			
			glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->bind();
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->uploadUniformFloat3("u_squareColor", m_squareColor);

			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 myTransform = glm::translate(glm::mat4(1.0), pos) * scale;
					std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->uploadUniformMat4("u_viewProjection", m_camera.getViewProjectionMatrix());
					
					std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->bind();
					Rocket::Renderer::submit(m_squareVA, m_squareShader, myTransform);
					
				}
			}

			auto textureShader = m_shaderLibrary.get("Texture");
			
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->bind();
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->uploadUniformMat4("u_viewProjection", m_camera.getViewProjectionMatrix());
			Rocket::Renderer::submit(m_squareVA, textureShader, squareTransform);
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(textureShader)->unbind();

			//m_triangleShader->bind();
			//std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_triangleShader)->uploadUniformMat4("u_viewProjection", m_camera.getViewProjectionMatrix());
			//Rocket::Renderer::sumbit(m_triangleVA, m_triangleShader);

		Rocket::Renderer::endScene();
	}

	virtual void onImGuiRender() override {
		ImGui::Begin("Square settings");
		ImGui::ColorEdit3("Square color", glm::value_ptr(m_squareColor));
		ImGui::End();
	}

	void onEvent(Rocket::Event& event) override {
	
	}

private:
	Rocket::ShaderLibrary m_shaderLibrary;

	//temp
	Rocket::Ref<Rocket::VertexArray> m_triangleVA;
	Rocket::Ref<Rocket::Shader> m_triangleShader;

	Rocket::Ref<Rocket::Shader> m_squareShader;
	Rocket::Ref<Rocket::VertexArray> m_squareVA;
	glm::vec3 m_squareColor{};

	Rocket::OrthographicCamera2D m_camera;

	glm::vec3 m_squarePosition{ 0 };
	float m_squareMoveSpeed = 0.008f;

	float m_rotationSpeed = 0.003f;
	float m_rotation = 0.0f;
	float m_cameraSpeed = 0.0088f;


	Rocket::Ref<Rocket::Texture2D> m_texture;
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
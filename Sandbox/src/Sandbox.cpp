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


		std::string triangleVertexShaderSource = R"(
			#version 330 core
			
			layout (location = 0) in vec3 a_position;
			layout (location = 1) in vec4 a_color;

			uniform mat4 u_viewProjection;
			uniform mat4 u_modelMatrix;

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

			uniform mat4 u_viewProjection;
			uniform mat4 u_modelMatrix;

			void main()
			{
				gl_Position = u_viewProjection * u_modelMatrix * vec4(a_position, 1.0);		
			}
		)";

		std::string squareFragmentShaderSource = R"(
			#version 330 core

			layout (location = 0) out vec4 color;
			uniform vec3 u_squareColor;
		
			void main()
			{
				color = vec4(u_squareColor, 1.0);
			}
		)";

		std::string textureVertexShaderSource = R"(
			#version 330 core
			
			layout (location = 0) in vec3 a_position;
			layout (location = 1) in vec2 a_textureCoord;

			uniform mat4 u_viewProjection;
			uniform mat4 u_modelMatrix;

			out vec2 v_textureCoord;	
	
			void main()
			{
				v_textureCoord = a_textureCoord;
				gl_Position = u_viewProjection * u_modelMatrix * vec4(a_position, 1.0);		
			}
		)";

		std::string textureFragmentShaderSource = R"(
			#version 330 core

			layout (location = 0) out vec4 color;
		
			in vec2 v_textureCoord;

			uniform sampler2D u_texture;

			void main()
			{
				color = texture(u_texture, v_textureCoord);
			}
		)";

		m_triangleShader.reset(Rocket::Shader::create(triangleVertexShaderSource, triangleFragmentShaderSource));
		m_squareShader.reset(Rocket::Shader::create(squareVertexShaderSource, squareFragmentShaderSource));
		m_textureShader.reset(Rocket::Shader::create(textureVertexShaderSource, textureFragmentShaderSource));


		m_texture = Rocket::Texture2D::create("assets/textures/wood.png");
		std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_textureShader)->bind();
		std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_textureShader)->uploadUniformInt("u_texture", 0);
		
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

			
			glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->bind();
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->uploadUniformFloat3("u_squareColor", m_squareColor);

			for (int y = 0; y < 20; y++) {
				for (int x = 0; x < 20; x++) {
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 myTransform = glm::translate(glm::mat4(1.0), pos) * scale;
					std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_squareShader)->uploadUniformMat4("u_viewProjection", m_camera.getViewProjectionMatrix());
					Rocket::Renderer::sumbit(m_squareVA, m_squareShader, myTransform);
				}
			}
			
			m_texture->bind(0);
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_textureShader)->bind();
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_textureShader)->uploadUniformMat4("u_viewProjection", m_camera.getViewProjectionMatrix());
			Rocket::Renderer::sumbit(m_squareVA, m_textureShader, squareTransform);
			std::dynamic_pointer_cast<Rocket::OpenGLShader>(m_textureShader)->unbind();

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

	//temp
	Rocket::Ref<Rocket::VertexArray> m_triangleVA;
	Rocket::Ref<Rocket::Shader> m_triangleShader;

	Rocket::Ref<Rocket::Shader> m_squareShader, m_textureShader;
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
#include "rcktpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

namespace Rocket {

	struct Renderer2DStorage {
		Ref<VertexArray> squareVA;
		
		Ref<Shader> squareFlatColorShader;

		Ref<Shader> squareTextureShader;
		Ref<Texture2D> defaultTexture;

		glm::mat4 squareTransform;
	};

	static Renderer2DStorage* s_data;

	void Renderer2D::init() {
		s_data = new Renderer2DStorage();
		s_data->squareVA = VertexArray::create();

		float squareVertices[5 * 4] = {
				-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
				 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
				-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Rocket::Ref<VertexBuffer> squareVertexBuffer;
		squareVertexBuffer = VertexBuffer::create(squareVertices, sizeof(squareVertices));

		squareVertexBuffer->setLayout(
			{
				{ ShaderDataType::Float3, "a_position" },
				{ ShaderDataType::Float2, "a_textureCoord"}
			});

		s_data->squareVA->addVertexBuffer(squareVertexBuffer);
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		Ref<Rocket::IndexBuffer> squareIndexBuffer;
		squareIndexBuffer = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

		s_data->squareVA->setIndexBuffer(squareIndexBuffer);

		s_data->squareFlatColorShader = Shader::create("assets/shaders/Square.glsl");
		s_data->squareTextureShader = Shader::create("assets/shaders/Texture.glsl");
		s_data->squareTextureShader->bind();
		s_data->defaultTexture = Texture2D::create("assets/textures/default.png");
		s_data->squareTextureShader->setInt("u_texture", 0);
	}

	void Renderer2D::shutdown() {
		delete(s_data);
	}

	void Renderer2D::beginScene(const OrthographicCamera2D& camera) {
		s_data->squareFlatColorShader->bind();
		s_data->squareFlatColorShader->setMat4("u_viewProjection", camera.getViewProjectionMatrix());

		s_data->squareTextureShader->bind();
		s_data->squareTextureShader->setMat4("u_viewProjection", camera.getViewProjectionMatrix());
	}

	void Renderer2D::endScene() {

	}

	void Renderer2D::drawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation) {
		drawQuad3D({ position.x, position.y, 0.0f }, size, color, rotation);
	}

	void Renderer2D::drawQuad3D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation) {
		s_data->squareFlatColorShader->bind();
		s_data->squareFlatColorShader->setFloat4("u_color", color);
		s_data->squareTransform = glm::translate(glm::mat4(1.0f), position) *
								  glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f }) *
								  glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1));
		s_data->squareFlatColorShader->setMat4("u_modelMatrix", s_data->squareTransform);
		s_data->squareVA->bind();
		RenderCommand::drawIndexed(s_data->squareVA);
	}

	void Renderer2D::drawQuad2DWithTexture( const glm::vec2& position,
											const glm::vec2& size,
											const glm::vec4& color,
											const Ref<Texture2D>& texture,
											float rotation) {
		drawQuad3DWithTexture({ position.x, position.y, 0.0f }, size, color, texture, rotation);
	}

	void Renderer2D::drawQuad3DWithTexture( const glm::vec3& position,
											const glm::vec2& size,
											const glm::vec4& color,
											const Ref<Texture2D>& texture,
											float rotation) {
		if (texture == nullptr) {
			s_data->defaultTexture->bind(0);
		}
		else {
			texture->bind(0);
		}

		s_data->squareTextureShader->bind();

		s_data->squareTransform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f }) *
			glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1));
		s_data->squareTextureShader->setMat4("u_modelMatrix", s_data->squareTransform);
		s_data->squareTextureShader->setFloat4("u_color", color);
		
		s_data->squareVA->bind();
		RenderCommand::drawIndexed(s_data->squareVA);
	}


} // namespace Rocket
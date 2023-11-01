#include "rcktpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

namespace Rocket {

	struct QuadVertex {
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 texCoord = { 0.0f, 0.0f };
		glm::vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct Renderer2DStorage {
		const uint32_t maxQuads = 10000;
		const uint32_t maxVertices = maxQuads * 4;
		const uint32_t maxIndices = maxQuads * 6;

		Ref<VertexArray> quadVA;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> quadShader;
		Ref<Texture2D> defaultTexture;
		glm::vec2 defaultTextureScale;
		glm::mat4 quadTransform;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;
	};

	static Renderer2DStorage s_data;

	void Renderer2D::init() {
		RCKT_PROFILE_FUNCTION();

		s_data.quadVA = VertexArray::create();

		s_data.quadVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(QuadVertex));

		s_data.quadVertexBuffer->setLayout(
			{
				{ ShaderDataType::Float3, "a_position" },
				{ ShaderDataType::Float2, "a_textureCoord"},
				{ ShaderDataType::Float4, "a_color"}
			});

		s_data.quadVA->addVertexBuffer(s_data.quadVertexBuffer);

		s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertices];

		uint32_t* quadIndices = new uint32_t[s_data.maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_data.maxIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}


		Ref<Rocket::IndexBuffer> quadIB;
		quadIB = IndexBuffer::create(quadIndices, s_data.maxIndices);

		s_data.quadVA->setIndexBuffer(quadIB);

		delete[] quadIndices;

		s_data.quadShader = Shader::create("assets/shaders/Quad.glsl");
		s_data.quadShader->bind();
		s_data.defaultTexture = Texture2D::create("assets/textures/default.png");
		//s_data.quadShader->setInt("u_texture", 0);
	}

	void Renderer2D::shutdown() {
		RCKT_PROFILE_FUNCTION();
	}

	void Renderer2D::beginScene(const OrthographicCamera2D& camera) {
		RCKT_PROFILE_FUNCTION();

		s_data.quadShader->bind();
		s_data.quadShader->setMat4("u_viewProjection", camera.getViewProjectionMatrix());

		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
	}

	void Renderer2D::endScene() {
		RCKT_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase;
		s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, dataSize);

		flush();
	}

	void Renderer2D::flush() {
		RCKT_PROFILE_FUNCTION();

		RenderCommand::drawIndexed(s_data.quadVA, s_data.quadIndexCount);
	}

	void Renderer2D::drawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation) {
		drawQuad3D({ position.x, position.y, 0.0f }, size, color, rotation);
	}

	void Renderer2D::drawQuad3D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation) {
		RCKT_PROFILE_FUNCTION();

		s_data.quadShader->bind();
		s_data.quadShader->setFloat4("u_color", color);
		s_data.quadTransform = glm::translate(glm::mat4(1.0f), position) *
								  glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f }) *
								  glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1));
		s_data.quadShader->setMat4("u_modelMatrix", s_data.quadTransform);
		s_data.quadVA->bind();
		RenderCommand::drawIndexed(s_data.quadVA);
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
		RCKT_PROFILE_FUNCTION();

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = position;
		s_data.quadVertexBufferPtr->texCoord = { 0.0f, 0.0f };
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = { position.x + size.x, position.y, 0.0f };
		s_data.quadVertexBufferPtr->texCoord = { 1.0f, 0.0f };
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = { position.x + size.x, position.y + size.y, 0.0f };
		s_data.quadVertexBufferPtr->texCoord = { 1.0f, 1.0f };
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = { position.x, position.y + size.y, 0.0f };
		s_data.quadVertexBufferPtr->texCoord = { 0.0f, 1.0f };
		s_data.quadVertexBufferPtr++;

		s_data.quadIndexCount += 6;
		/*
		if (texture == nullptr) {
			s_data.defaultTexture->bind(0);
			s_data.squareTextureShader->setFloat("u_textureCoordMultiplier", 10.0f);
		}
		else {
			s_data.squareTextureShader->setFloat("u_textureCoordMultiplier", 1.0f);
			texture->bind(0);
		}

		s_data.squareTransform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f }) *
			glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 0, 1));

		s_data.squareTextureShader->bind();

		s_data.squareTextureShader->setMat4("u_modelMatrix", s_data.squareTransform);
		
		s_data.squareVA->bind();
		RenderCommand::drawIndexed(s_data.squareVA);
		*/
	}


} // namespace Rocket
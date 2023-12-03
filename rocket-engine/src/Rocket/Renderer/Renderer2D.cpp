#include "rcktpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <glad.h>
#include <stb_image.h>

namespace Rocket {

	//represents each vertex of the quad
	struct QuadVertex {
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 texCoord = { 0.0f, 0.0f };
		glm::vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 normal = { 0.0f, 0.0f, 0.0f };
		float texIndex;
		float tilingFactor;

		//Editor only
		int entityID;
	};

	//entire renderer2D data storage
	struct Renderer2DStorage {
		static const uint32_t maxQuads = 1000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxTextureSlots = 32; // TODO: RenderCaps

		Ref<VertexArray> quadVA;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> quadShader;
		//Ref<Shader> lightingShader;
		Ref<Texture2D> defaultTexture;
		glm::vec2 defaultTextureScale;
		glm::mat4 quadTransform;

		//Ref<VertexArray> skyboxVA;
		//Ref<VertexBuffer> skyboxVB;
		Ref<Shader> skyboxShader;
		unsigned int skyboxVAO, skyboxVBO, skyboxTexture;

		//temp ends

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr; // TODO: Make an array of this to support multiple textures at once

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; // 0 default texture
		
		glm::vec4 quadVertexPositions[4];

		Renderer2D::Statistics stats;
	};

	static Renderer2DStorage s_data;

	//TODO: remove this one, temporary stuff
	void Renderer2D::setEntityID(uint32_t id) {
		s_data.quadShader->setInt("currentEntityID", id);
	}

	void Renderer2D::uploadDiffuseLight(const glm::vec3& color, const glm::vec3& pos) {
		//s_data.diffuseLightColor = color;
		//s_data.diffuseLightPosition = pos;

		s_data.quadShader->setFloat3("u_diffusePosition", pos);
		s_data.quadShader->setFloat3("u_diffuseColor", color);
	}
	/*
	void Renderer2D::uploadModelMatrix(const glm::mat4& modelMat) {
		//s_data.quadShader->setMat4("u_model", modelMat);
	}
	*/
	void Renderer2D::uploadSpecularViewerPosition(const glm::vec3& position) {
		s_data.quadShader->setFloat3("u_viewPosition", position);
	}

	void Renderer2D::applyDirectionalLights(const std::vector<DirectionalLightComponent>& dirLights, const glm::vec3& viewPosition) {
		uploadSpecularViewerPosition(viewPosition);
		s_data.quadShader->setDirectionalLights(dirLights);
	}

	void Renderer2D::applyPointLights(const std::vector<PointLightComponent>& pointLights) {
		s_data.quadShader->setPointLights(pointLights);
	}

	void Renderer2D::applySpotLights(const std::vector<SpotLightComponent>& spotLights) {
		s_data.quadShader->setSpotLights(spotLights);
	}

	unsigned int Renderer2D::loadCubemap(std::vector<std::string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			if (i != 0)
				stbi_set_flip_vertically_on_load(false);
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	void Renderer2D::prepareSkybox() {
		float cubeVertices[] = {
			// positions          // texture Coords
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		glGenVertexArrays(1, &s_data.skyboxVAO);
		glGenBuffers(1, &s_data.skyboxVBO);
		glBindVertexArray(s_data.skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, s_data.skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		std::vector<std::string> faces
		{
			"C:/dev/rocket-engine/rocket-editor/assets/skybox/right.jpg",
			"C:/dev/rocket-engine/rocket-editor/assets/skybox/left.jpg",
			"C:/dev/rocket-engine/rocket-editor/assets/skybox/top.jpg",
			"C:/dev/rocket-engine/rocket-editor/assets/skybox/bottom.jpg",
			"C:/dev/rocket-engine/rocket-editor/assets/skybox/front.jpg",
			"C:/dev/rocket-engine/rocket-editor/assets/skybox/back.jpg"
		};

		s_data.skyboxTexture = loadCubemap(faces);

	}


	void Renderer2D::applySkybox(const EditorCamera& camera, const glm::vec2& viewportSize) {
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

		s_data.skyboxShader->bind();
		s_data.skyboxShader->setInt("skybox", 0);
		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix())); // remove translation from the view matrix
		//glm::mat4 projection = glm::perspective(glm::radians(camera.getZoomLevel()), (float)viewportSize.x / (float)viewportSize.y, 0.1f, 100.0f);
		//s_data.skyboxShader->setMat4("u_model", model);
		s_data.skyboxShader->setMat4("u_view", view);
		s_data.skyboxShader->setMat4("u_projection", camera.getProjection());

		// skybox cube
		glBindVertexArray(s_data.skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, s_data.skyboxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
	}


	//temp ends here


	void Renderer2D::init() {
		RCKT_PROFILE_FUNCTION();
		//VA stuff
		s_data.quadVA = VertexArray::create();
		s_data.quadVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(QuadVertex));
		s_data.quadVertexBuffer->setLayout(
			{
				{ ShaderDataType::Float3, "a_position"     },
				{ ShaderDataType::Float2, "a_textureCoord" },
				{ ShaderDataType::Float4, "a_color"        },
				{ ShaderDataType::Float3, "a_normal"       },
				{ ShaderDataType::Float,  "a_texIndex"     },
				{ ShaderDataType::Float,  "a_tilingFactor" },
				{ ShaderDataType::Int,    "a_entityID"     }, // color attachment 2
			});
		s_data.quadVA->addVertexBuffer(s_data.quadVertexBuffer);
		
		//batching part
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

		//uploading to GPU
		s_data.quadVA->setIndexBuffer(quadIB);

		//uploaded, no more need
		delete[] quadIndices;

		s_data.quadShader = Shader::create("assets/shaders/Quad.glsl");
		s_data.skyboxShader = Shader::create("assets/shaders/Skybox.glsl");
		//s_data.lightingShader = Shader::create("assets/shaders/Lighting.glsl");
		s_data.quadShader->bind();
		s_data.defaultTexture = Texture2D::create("assets/textures/default.png");
		
		int samplers[s_data.maxTextureSlots];
		for (int i = 0; i < s_data.maxTextureSlots; i++)
			samplers[i] = i;

		s_data.quadShader->setIntArray("u_textures", samplers, s_data.maxTextureSlots);
		
		//binding default texture to set 0
		s_data.textureSlots[0] = s_data.defaultTexture;

		s_data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		prepareSkybox();
	}

	void Renderer2D::shutdown() {
		RCKT_PROFILE_FUNCTION();

		delete[] s_data.quadVertexBufferBase;
	}

	void Renderer2D::beginScene(const OrthographicCamera2D& camera) {
		RCKT_PROFILE_FUNCTION();

		s_data.quadShader->bind();
		s_data.quadShader->setMat4("u_viewProjection", camera.getViewProjectionMatrix());

		//starting from 0 bytes in each frame
		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		//starting from slot 1 in each frame
		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform) {
		RCKT_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.getProjection() * glm::inverse(transform);

		s_data.quadShader->bind();
		s_data.quadShader->setMat4("u_viewProjection", viewProj);

		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		//starting from slot 1 in each frame
		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::beginScene(const EditorCamera& camera) {
		RCKT_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.getViewProjection();

		s_data.quadVA->bind();
		s_data.quadShader->bind();
		s_data.quadShader->setMat4("u_viewProjection", viewProj);
	
		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;
		
		//starting from slot 1 in each frame
		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::endScene() {
		RCKT_PROFILE_FUNCTION();

		//calculating data size to be rendered in bytes
		uint32_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase;
		s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, dataSize);

		flush();
	}

	void Renderer2D::flush() {
		RCKT_PROFILE_FUNCTION();

		// nothing to render
		if (s_data.quadIndexCount == 0)
			return;

		//bind textures
		for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
			s_data.textureSlots[i]->bind();
		}

		RenderCommand::drawIndexed(s_data.quadVA, s_data.quadIndexCount);
		s_data.stats.drawCalls++;
		s_data.quadShader->unbind();
	}

	void Renderer2D::drawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation) {
		drawQuad3D({ position.x, position.y, 0.0f }, size, color, rotation);
	}

	void Renderer2D::drawQuad3D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation) {
		RCKT_PROFILE_FUNCTION();

		if (s_data.quadIndexCount >= Renderer2DStorage::maxIndices) {
			flushAndReset();
		}

		const float textureIndex = -1.0f; // no texture for simple quads
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[0];
		s_data.quadVertexBufferPtr->texCoord = { 0.0f, 0.0f };
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[1];
		s_data.quadVertexBufferPtr->texCoord = { 1.0f, 0.0f };
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[2];
		s_data.quadVertexBufferPtr->texCoord = { 1.0f, 1.0f };
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[3];
		s_data.quadVertexBufferPtr->texCoord = { 0.0f, 1.0f };
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadIndexCount += 6;
		s_data.stats.quadCount++;
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

		if (s_data.quadIndexCount >= Renderer2DStorage::maxIndices) {
			flushAndReset();
		}

		float textureIndex = 0.0f;
		float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		if (texture != nullptr) {
			for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
				if (*s_data.textureSlots[i].get() == *texture.get()) {
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f) {
				textureIndex = (float)s_data.textureSlotIndex;
				s_data.textureSlots[s_data.textureSlotIndex] = texture;
				textureIndex = s_data.textureSlotIndex;
				s_data.textureSlotIndex++;
			}
		}
		else {
			tilingFactor = 10.0f;
		}
		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[0];
		s_data.quadVertexBufferPtr->texCoord = { 0.0f, 0.0f };
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[1];
		s_data.quadVertexBufferPtr->texCoord = { 1.0f, 0.0f };
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[2];;
		s_data.quadVertexBufferPtr->texCoord = { 1.0f, 1.0f };
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[3];;
		s_data.quadVertexBufferPtr->texCoord = { 0.0f, 1.0f };
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadIndexCount += 6;
		s_data.stats.quadCount++;
	}

	void Renderer2D::drawQuad2DWithSubTexture(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const Ref<SubTexture2D>& subtexture, float rotation) {
		drawQuad3DWithSubTexture({ position.x, position.y, 0.0f }, size, color, subtexture, rotation);
	}

	void Renderer2D::drawQuad3DWithSubTexture(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const Ref<SubTexture2D>& subtexture, float rotation) {
		RCKT_PROFILE_FUNCTION();

		if (s_data.quadIndexCount >= Renderer2DStorage::maxIndices) {
			flushAndReset();
		}

		float textureIndex = 0.0f;
		float tilingFactor = 1.0f;
		const glm::vec2* texCoords = subtexture->getTexCoords();
		const Ref<Texture2D> texture = subtexture->getTexture();

		if (texture != nullptr) {
			for (uint32_t i = 1; i < s_data.textureSlotIndex; i++) {
				if (*s_data.textureSlots[i].get() == *texture.get()) {
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f) {
				textureIndex = (float)s_data.textureSlotIndex;
				s_data.textureSlots[s_data.textureSlotIndex] = texture;
				textureIndex = s_data.textureSlotIndex;
				s_data.textureSlotIndex++;
			}
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[0];
		s_data.quadVertexBufferPtr->texCoord = texCoords[0];
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[1];
		s_data.quadVertexBufferPtr->texCoord = texCoords[1];
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[2];;
		s_data.quadVertexBufferPtr->texCoord = texCoords[2];
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadVertexBufferPtr->color = color;
		s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[3];;
		s_data.quadVertexBufferPtr->texCoord = texCoords[3];
		s_data.quadVertexBufferPtr->texIndex = textureIndex;
		s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
		s_data.quadVertexBufferPtr++;

		s_data.quadIndexCount += 6;
		s_data.stats.quadCount++;

	}

	void Renderer2D::drawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID) {
		drawQuadWithViewMat(transform, src.color, entityID);
	}

	void Renderer2D::drawQuadWithViewMat(const glm::mat4& transform, const glm::vec4& color, int entityID) {
		RCKT_PROFILE_FUNCTION();

		if (s_data.quadIndexCount >= Renderer2DStorage::maxIndices) {
			flushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float textureIndex = -1.0f; // no texture for simple quads
		const float tilingFactor = 1.0f;

		glm::vec3 edge1 = transform * (s_data.quadVertexPositions[2] - s_data.quadVertexPositions[1]);
		glm::vec3 edge2 = transform * (s_data.quadVertexPositions[0] - s_data.quadVertexPositions[1]);

		glm::vec3 normal = glm::cross(edge1, edge2);

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[i];
			s_data.quadVertexBufferPtr->color = color;
			s_data.quadVertexBufferPtr->normal = normal;
			s_data.quadVertexBufferPtr->texCoord = textureCoords[i];
			s_data.quadVertexBufferPtr->texIndex = textureIndex;
			s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			s_data.quadVertexBufferPtr->entityID = entityID;
			s_data.quadVertexBufferPtr++;

		}

		s_data.quadIndexCount += 6;
		s_data.stats.quadCount++;
	}

	Renderer2D::Statistics Renderer2D::getStats() {
		return s_data.stats;
	}

	void Renderer2D::resetStats() {
		memset(&s_data.stats, 0, sizeof(Renderer2D::Statistics));
	}

	void Renderer2D::flushAndReset() {
		endScene();
		
		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		s_data.textureSlotIndex = 1;
	}


} // namespace Rocket
#include "rcktpch.h"
#include "Renderer2D.h"

#include "VertexArray.h"
#include "Shader.h"
#include "Skybox.h"
#include "RenderCommand.h"
#include "Rocket/Utils/Utils.h"

#include <glad.h>
#include <stb_image.h>


namespace Rocket {

	//represents each vertex of the quad
	struct QuadVertex {
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec2 texCoord = { 0.0f, 0.0f };
		glm::vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
	//	glm::vec3 normal = { 0.0f, 0.0f, 0.0f };
		float texIndex;
		float tilingFactor;

		//Editor only
		int entityID;
	};

	struct CircleVertex {
		glm::vec3 worldPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 localPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
		float thickness;
		float fade;

		//Editor only
		int entityID;
	};

	//entire renderer2D data storage
	struct Renderer2DStorage {
		static const uint32_t maxQuads = 1000;
		static const uint32_t maxQuadVertices = maxQuads * 4;
		static const uint32_t maxQuadIndices = maxQuads * 6;
		static const uint32_t maxCircles = 1000;
		static const uint32_t maxCircleVertices = maxCircles * 4;
		static const uint32_t maxCircleIndices = maxCircles * 6;
		static const uint32_t maxTextureSlots = 32; // TODO: RenderCaps



		Ref<VertexArray> quadVA;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> quadShader;

		Ref<VertexArray> circleVA;
		Ref<VertexBuffer> circleVertexBuffer;
		Ref<Shader> circleShader;

		//Ref<VertexArray> skyboxVA;
		//Ref<VertexBuffer> skyboxVB;
		Ref<Shader> reflectionShader, refractionShader;
		bool reflectionEnabled = false, refractionEnabled = false;

		Ref<Shader> skyboxShader;
		Ref<Skybox> skybox;
		std::string skyboxPath = "../rocket-editor/assets/skybox/";
		int skyboxIndex = 1;
		std::vector<std::string> skyboxFaces;

		//Ref<VertexArray> skyboxVertexArray;
		//Ref<VertexBuffer> skyboxVertexBuffer;
		//unsigned int skyboxVAO, skyboxVBO, skyboxTexture;


		//temp ends

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr; // TODO: Make an array of this to support multiple textures at once

		uint32_t circleIndexCount = 0;
		CircleVertex* circleVertexBufferBase = nullptr;
		CircleVertex* circleVertexBufferPtr = nullptr;

		// TODO: move these ones to vector, 

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 0; // no default texture
		
		glm::vec4 quadVertexPositions[4];

		Renderer2D::Statistics stats;
	};

	static Renderer2DStorage s_data;


	void Renderer2D::init() {
		RCKT_PROFILE_FUNCTION();
		s_data.quadVA = VertexArray::create();
		s_data.circleVA = VertexArray::create();
		initQuadVB();
		initCircleVB();
		initIB();
		s_data.quadShader = Shader::create("assets/shaders/Quad.glsl");
		s_data.circleShader = Shader::create("assets/shaders/Circle.glsl");
		s_data.skyboxShader = Shader::create("assets/shaders/Skybox.glsl");
		s_data.reflectionShader = Shader::create("assets/shaders/Reflection.glsl");
		s_data.refractionShader = Shader::create("assets/shaders/Refraction.glsl");
		s_data.quadShader->bind();

		s_data.quadVertexBufferBase = new QuadVertex[s_data.maxQuadVertices];
		s_data.circleVertexBufferBase = new CircleVertex[s_data.maxCircleVertices];

		int samplers[s_data.maxTextureSlots];
		for (int i = 0; i < s_data.maxTextureSlots; i++)
			samplers[i] = i;
		s_data.quadShader->setIntArray("u_textures", samplers, s_data.maxTextureSlots);

		s_data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		prepareSkybox();
	}

	void Renderer2D::reset() {
		RCKT_PROFILE_FUNCTION();
		s_data.quadVA->unbind();
		RCKT_CORE_INFO("Before: {0}", s_data.quadVA.use_count());
		s_data.quadVA.reset();
		s_data.quadVA = VertexArray::create();
		RCKT_CORE_INFO("Then: {0}", s_data.quadVA.use_count());
		delete[] s_data.quadVertexBufferBase;
		s_data.quadVertexBufferBase = new QuadVertex[s_data.maxQuadVertices];
		s_data.quadIndexCount = 0;
		initQuadVB();
		initIB();
		s_data.quadVA->bind();
	}

	void Renderer2D::shutdown() {
		RCKT_PROFILE_FUNCTION();

		delete[] s_data.quadVertexBufferBase;
		delete[] s_data.circleVertexBufferBase;
	}

	void Renderer2D::initQuadVB() {
		s_data.quadVertexBuffer.reset();
		s_data.quadVertexBuffer = VertexBuffer::create(s_data.maxQuadVertices * sizeof(QuadVertex));
		s_data.quadVertexBuffer->setLayout(
			{
				{ ShaderDataType::Float3, "a_position"     },
				{ ShaderDataType::Float2, "a_texCoord"	   },
				{ ShaderDataType::Float4, "a_color"        },
				{ ShaderDataType::Float,  "a_texIndex"     },
				{ ShaderDataType::Float,  "a_tilingFactor" },
				{ ShaderDataType::Int,    "a_entityID"     }, // color attachment 2
			});
		s_data.quadVA->addVertexBuffer(s_data.quadVertexBuffer);
	}

	void Renderer2D::initCircleVB() {
		s_data.circleVertexBuffer.reset();
		s_data.circleVertexBuffer = VertexBuffer::create(s_data.maxCircleVertices * sizeof(CircleVertex));
		s_data.circleVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "a_worldPosition" },
			{ ShaderDataType::Float3, "a_localPosition" },
			{ ShaderDataType::Float4, "a_color"         },
			{ ShaderDataType::Float,  "a_thickness"		},
			{ ShaderDataType::Float,  "a_fade"			},
			{ ShaderDataType::Float,  "a_entityID"		},
		});
		s_data.circleVA->addVertexBuffer(s_data.circleVertexBuffer);
	}

	void Renderer2D::initIB() {
		s_data.quadVertexBufferBase = new QuadVertex[s_data.maxQuadVertices];
		uint32_t* quadIndices = new uint32_t[s_data.maxQuadIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_data.maxQuadIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		Ref<Rocket::IndexBuffer> quadIB;
		quadIB = IndexBuffer::create(quadIndices, s_data.maxQuadIndices);
		s_data.quadVA->setIndexBuffer(quadIB);
		s_data.circleVA->setIndexBuffer(quadIB);
		delete[] quadIndices;
	}
	
	void Renderer2D::beginScene(const OrthographicCamera2D& camera) {
		RCKT_PROFILE_FUNCTION();

		s_data.quadShader->bind();
		s_data.quadShader->setMat4("u_viewProjection", camera.getViewProjectionMatrix());
		s_data.circleShader->bind();
		s_data.circleShader->setMat4("u_viewProjection", camera.getViewProjectionMatrix());

		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		s_data.circleIndexCount = 0;
		s_data.circleVertexBufferPtr = s_data.circleVertexBufferBase;
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform) {
		RCKT_PROFILE_FUNCTION();

		glm::mat4 viewProjection = camera.getProjection() * glm::inverse(transform);

		s_data.quadShader->bind();
		s_data.quadShader->setMat4("u_viewProjection", viewProjection);
		s_data.circleShader->bind();
		s_data.circleShader->setMat4("u_viewProjection", viewProjection);

		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		s_data.circleIndexCount = 0;
		s_data.circleVertexBufferPtr = s_data.circleVertexBufferBase;
	}

	void Renderer2D::beginScene(const EditorCamera& camera) {
		RCKT_PROFILE_FUNCTION();

		glm::mat4 viewProjection = camera.getViewProjection();
		s_data.quadShader->bind();
		s_data.quadShader->setMat4("u_viewProjection", viewProjection);
		s_data.circleShader->bind();
		s_data.circleShader->setMat4("u_viewProjection", viewProjection);

		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		s_data.circleIndexCount = 0;
		s_data.circleVertexBufferPtr = s_data.circleVertexBufferBase;

		/*
		if (s_data.reflectionEnabled) {
			s_data.reflectionShader->bind();
			s_data.reflectionShader->setMat4("u_viewProjection", viewProj);
			s_data.reflectionShader->setMat4("u_model", glm::mat4(1.0f));
			s_data.reflectionShader->setFloat3("u_viewPosition", camera.getPosition());

		}
		else if (s_data.refractionEnabled) {
			s_data.refractionShader->bind();
			s_data.refractionShader->setMat4("u_viewProjection", viewProj);
			s_data.refractionShader->setMat4("u_model", glm::mat4(1.0f));
			s_data.refractionShader->setFloat3("u_viewPosition", camera.getPosition());
		}
		*/ // TODO: Move into components, don't forget about gamma correction
	}


	//TODO: remove this one, temporary stuff
	void Renderer2D::setEntityID(uint32_t id) {
		s_data.quadShader->setInt("currentEntityID", id);
	}

	void Renderer2D::uploadView(const glm::vec3& position) {
		s_data.quadShader->setFloat3("u_viewPosition", position);
	}

	void Renderer2D::applyDirectionalLights(const std::vector<DirectionalLightComponent>& dirLights, const glm::vec3& viewPosition) {
		uploadView(viewPosition);
		s_data.quadShader->setDirectionalLights(dirLights);
	}
	
	void Renderer2D::applyPointLights(const std::vector<PointLightComponent>& pointLights) {
		s_data.quadShader->setPointLights(pointLights);
	}
	
	void Renderer2D::applySpotLights(const std::vector<SpotLightComponent>& spotLights) {
		s_data.quadShader->setSpotLights(spotLights);
	}
	
	void Renderer2D::prepareSkybox() {
		s_data.skyboxFaces = {
		s_data.skyboxPath + std::to_string(s_data.skyboxIndex) + "/right.png",
		s_data.skyboxPath + std::to_string(s_data.skyboxIndex) + "/left.png",
		s_data.skyboxPath + std::to_string(s_data.skyboxIndex) + "/top.png",
		s_data.skyboxPath + std::to_string(s_data.skyboxIndex) + "/bottom.png",
		s_data.skyboxPath + std::to_string(s_data.skyboxIndex) + "/front.png",
		s_data.skyboxPath + std::to_string(s_data.skyboxIndex) + "/back.png"
		};

		s_data.skybox = Skybox::create(s_data.skyboxFaces);
	}

	void Renderer2D::setSkybox(const std::vector<std::string>& faces) {
		s_data.skybox->resetSkybox(faces);
	}

	void Renderer2D::changeSkybox() {
		s_data.skyboxIndex++;
		if (s_data.skyboxIndex > 4)
			s_data.skyboxIndex = 1;
		prepareSkybox();
	}

	void Renderer2D::applySkybox(const EditorCamera& camera) {
		s_data.skyboxShader->bind();
		s_data.skyboxShader->setInt("skybox", 0);
		glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix())); // remove translation from the view matrix
		s_data.skyboxShader->setMat4("u_view", view);
		s_data.skyboxShader->setMat4("u_projection", camera.getProjection());
		s_data.skybox->drawSkybox();
		s_data.skyboxShader->unbind();
	}

	void Renderer2D::enableReflection(bool enabled) {
		s_data.reflectionEnabled = enabled;
	}

	void Renderer2D::enableRefraction(bool enabled) {
		s_data.refractionEnabled = enabled;
	}


	void Renderer2D::endScene() {
		RCKT_PROFILE_FUNCTION();

		flush();
	}

	void Renderer2D::flush() {
		RCKT_PROFILE_FUNCTION();

		// nothing to render
		if (s_data.quadIndexCount > 0) {
			uint32_t dataSize = (uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase;
			s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, dataSize);

			//bind textures
			for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
				s_data.textureSlots[i]->bind(i);
			}

			s_data.quadShader->bind();
			RenderCommand::drawIndexed(s_data.quadVA, s_data.quadIndexCount);
			s_data.stats.drawCalls++;
		}

		if (s_data.circleIndexCount > 0) {

			uint32_t dataSize = (uint8_t*)s_data.circleVertexBufferPtr - (uint8_t*)s_data.circleVertexBufferBase;
			s_data.circleVertexBuffer->setData(s_data.circleVertexBufferBase, dataSize);

			s_data.circleShader->bind();
			RenderCommand::drawIndexed(s_data.circleVA, s_data.circleIndexCount);
			s_data.stats.drawCalls++;
		}

	}

	void Renderer2D::drawQuad2D(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation) {
		drawQuad3D({ position.x, position.y, 0.0f }, size, color, rotation);
	}

	void Renderer2D::drawQuad3D(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, float rotation) {
		RCKT_PROFILE_FUNCTION();

		if (s_data.quadIndexCount >= Renderer2DStorage::maxQuadIndices) {
			flushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float textureIndex = -1.0f; // no texture for simple quads
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		glm::vec3 edge1 = transform * (s_data.quadVertexPositions[2] - s_data.quadVertexPositions[1]);
		glm::vec3 edge2 = transform * (s_data.quadVertexPositions[0] - s_data.quadVertexPositions[1]);
		glm::vec3 normal = glm::cross(edge1, edge2);

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[i];
			s_data.quadVertexBufferPtr->color = color;
			//s_data.quadVertexBufferPtr->normal = normal;
			s_data.quadVertexBufferPtr->texCoord = textureCoords[i];
			s_data.quadVertexBufferPtr->texIndex = textureIndex;
			s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			s_data.quadVertexBufferPtr->entityID = -1; // (TODO: add or remove this function either entity ID)
			s_data.quadVertexBufferPtr++;

		}

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

		if (s_data.quadIndexCount >= Renderer2DStorage::maxQuadIndices) {
			flushAndReset();
		}

		float textureIndex = 0.0f;
		float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		if (texture != nullptr) {
			for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
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

		if (s_data.quadIndexCount >= Renderer2DStorage::maxQuadIndices) {
			flushAndReset();
		}

		float textureIndex = 0.0f;
		float tilingFactor = 1.0f;
		const glm::vec2* texCoords = subtexture->getTexCoords();
		const Ref<Texture2D> texture = subtexture->getTexture();

		if (texture != nullptr) {
			for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
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
		RCKT_PROFILE_FUNCTION();

		if (s_data.quadIndexCount >= Renderer2DStorage::maxQuadIndices) {
			flushAndReset();
		}

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		float textureIndex = -1.0f; // being set if has a texture

		if (src.texture != nullptr) {
			for (uint32_t i = 0; i < s_data.textureSlotIndex; i++) {
				if (*s_data.textureSlots[i].get() == *src.texture.get()) {
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == -1.0f) {
				textureIndex = (float)s_data.textureSlotIndex;
				s_data.textureSlots[s_data.textureSlotIndex] = src.texture;
				s_data.textureSlotIndex++;
			}
		}

		for (size_t i = 0; i < quadVertexCount; i++) {
			s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[i];
			s_data.quadVertexBufferPtr->texCoord = textureCoords[i];
			s_data.quadVertexBufferPtr->color = src.color;
			s_data.quadVertexBufferPtr->texIndex = textureIndex;
			s_data.quadVertexBufferPtr->tilingFactor = src.tilingFactor;
			s_data.quadVertexBufferPtr->entityID = entityID;
			s_data.quadVertexBufferPtr++;
		}
	
		s_data.quadIndexCount += 6;
		s_data.stats.quadCount++;
	}

	void Renderer2D::drawCircle(const glm::mat4& transform, CircleRendererComponent& crc, int entityID) {
		
		RCKT_PROFILE_FUNCTION();
		
		//implement for circle flushing and reset

		for (size_t i = 0; i < 4; i++) {
			s_data.circleVertexBufferPtr->worldPosition = transform * s_data.quadVertexPositions[i];
			s_data.circleVertexBufferPtr->localPosition = 2.0f * s_data.quadVertexPositions[i];
			s_data.circleVertexBufferPtr->color = crc.color;
			s_data.circleVertexBufferPtr->thickness = crc.thickness;
			s_data.circleVertexBufferPtr->fade = crc.fade;
			s_data.circleVertexBufferPtr->entityID = entityID;
			s_data.circleVertexBufferPtr++;
		}

		s_data.circleIndexCount += 6;
		//s_data.circleCount++;
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

		s_data.textureSlotIndex = 0; // ?
	}


} // namespace Rocket
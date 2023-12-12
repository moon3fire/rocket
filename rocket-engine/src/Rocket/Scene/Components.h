#pragma once

#define SCENE_MAX_DIRECTIONAL_LIGHTS_COUNT 10
#define SCENE_MAX_POINT_LIGHTS_COUNT 100
#define SCENE_MAX_SPOT_LIGHTS_COUNT 100

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.h"
#include "Rocket/Core/UUID.h"
#include "Rocket/Renderer/Texture.h"

namespace Rocket {

	struct TagComponent {
		std::string tag;
		UUID id;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag_) :tag(tag_) {}
		TagComponent(const std::string& tag_, const UUID& uuid) :tag(tag_), id(uuid) {}
	};

	struct TransformComponent {
		
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation_) :position(translation_) {}
		TransformComponent(const glm::vec3& translation_, const glm::vec3& scale_) :position(translation_), scale(scale_) {}
		TransformComponent(const glm::vec3& translation_, const glm::vec3& scale_, const glm::vec3& rotation_)
			:position(translation_), scale(scale_),  rotation(rotation_) {}

		glm::mat4 getTransform() const {
			glm::mat4 rotateMat = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), position) * rotateMat * glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct SpriteRendererComponent {
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> texture = nullptr;
		float tilingFactor = 1.0f;
		bool isReflected = false;

		uint32_t getTextureID() const { 
			if (texture == nullptr)
				return -1;
			return texture->getRendererID();
		}

		std::string getTexturePath() const {
			if (texture == nullptr)
				return "";
			return texture->getTexturePath();
		}

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color_) :color(color_) {}
		SpriteRendererComponent(const glm::vec4& color_, const Ref<Texture2D>& texture_) :color(color_), texture(texture_) {}
	};

	struct CircleRendererComponent {
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		//float radius = 0.5f;
		float thickness = 1.0f;
		float fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct DirectionalLightComponent {
		// NOTE: order matters, UPD, maybe not
		glm::vec3 direction = { 0.0f, 0.0f, 0.0f };
		glm::vec3 ambient = { 1.0f, 1.0f, 1.0f };
		glm::vec3 diffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 specular = { 1.0f, 1.0f, 1.0f };
		float ambientStrenght = 0.0f;

		DirectionalLightComponent() :direction(glm::vec3(0.0f)), ambient(glm::vec3(1.0f)), diffuse(glm::vec3(1.0f)), specular(glm::vec3(1.0f)), ambientStrenght(0.0f) {}
		DirectionalLightComponent(const DirectionalLightComponent& other) :direction(other.direction), ambient(other.ambient), diffuse(other.diffuse), specular(other.specular), ambientStrenght(other.ambientStrenght) {}
		DirectionalLightComponent(const glm::vec3& direction_, const glm::vec3& ambient_, const glm::vec3& diffuse_, const glm::vec3& specular_, float strenght)
			: direction(direction_), ambient(ambient_), diffuse(diffuse_), specular(specular_), ambientStrenght(strenght) {}
	};

	struct PointLightComponent {
		glm::vec3* position;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float constant = 1.0f;
		float linear = 0.1f;
		float quadratic = 0.01f;

		float ambientStrenght = 0.0f;

		PointLightComponent() :position(nullptr), ambient(glm::vec3(1.0f)), diffuse(glm::vec3(1.0f)), specular(glm::vec3(1.0f)), constant(1.0f), linear(0.1f), quadratic(0.01f) {}
		PointLightComponent(const PointLightComponent& other) :position(other.position), ambient(other.ambient), diffuse(other.diffuse), specular(other.specular), constant(other.constant), linear(other.linear), quadratic(other.quadratic) {}
		PointLightComponent(TransformComponent& tc, const glm::vec3& ambient_, const glm::vec3& diffuse_, const glm::vec3& specular_, float constant_, float linear_, float quadratic_)
			: position(&(tc.position)), ambient(ambient_), diffuse(diffuse_), specular(specular_), constant(constant_), linear(linear_), quadratic(quadratic_) {}
		
	};

	struct SpotLightComponent {
		glm::vec3* position;
		glm::vec3 direction;
		float cutOff;
		float outerCutOff;

		float constant;
		float linear;
		float quadratic;
		
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		SpotLightComponent() :position(nullptr), ambient(glm::vec3(0.0f)), diffuse(glm::vec3(0.0f)), specular(glm::vec3(1.0f)), constant(1.0f), linear(0.1f), quadratic(0.01f), direction(glm::vec3(0.0f)), cutOff(1.0f), outerCutOff(1.0f) {}
		SpotLightComponent(const SpotLightComponent&) = default; // TODO: make sure if works change to this
		SpotLightComponent(TransformComponent& tc) :position(&(tc.position)), ambient(glm::vec3(1.0f)), diffuse(glm::vec3(1.0f)), specular(glm::vec3(1.0f)), constant(1.0f), linear(0.1f), quadratic(0.01f), direction(glm::vec3(0.0f)), cutOff(1.0f), outerCutOff(1.0f) {}
	};

	struct CameraComponent {
		SceneCamera camera;
		bool primary = true; // TODO: move into scene
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	// forward declaration to avoid recursive include
	class ScriptableEntity;
	struct NativeScriptComponent {
		ScriptableEntity* m_instance = nullptr;

		ScriptableEntity* (*instantiateScript)();
		void (*destroyScript)(NativeScriptComponent*);

		template <typename T>
		void bind() {

			instantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			destroyScript = [](NativeScriptComponent* nsc) { delete nsc->m_instance; nsc->m_instance = nullptr; };
		}
	};

	//Physics

	struct RigidBody2DComponent {
		enum class BodyType { Static = 0, Dynamic = 1, Kinematic = 2 };
		BodyType type = BodyType::Static;
		bool fixedRotation = false;

		void* runtimeBody = nullptr;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;
		~RigidBody2DComponent() { }
	};

	struct BoxCollider2DComponent {
		glm::vec2 offset = { 0.0f, 0.0f };
		glm::vec2 size = { 0.5f, 0.5f };

		float density = 1.0f;
		float friction = 0.5f;
		float restitution = 0.0f;
		float restitutionThreshold = 0.5f;

		void* runtimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
		~BoxCollider2DComponent() { }
	};

} // namespace Rocket


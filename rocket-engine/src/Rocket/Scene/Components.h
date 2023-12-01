#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"

namespace Rocket {

	struct TagComponent {
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag_) :tag(tag_) {}
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

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color_) :color(color_) {}
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

	struct CameraComponent {
		SceneCamera camera;
		bool primary = true; // TODO: move into scene
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

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

} // namespace Rocket


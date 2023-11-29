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
		// NOTE: order matters
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float ambientStrenght = 0.0f;

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent& other) :direction(other.direction), ambient(other.ambient), diffuse(other.diffuse), specular(other.specular), ambientStrenght(other.ambientStrenght) {}
		DirectionalLightComponent(const glm::vec3& direction_ = { 0.0f, 0.0f, 0.0f }, const glm::vec3& ambient_ = { 0.0f, 0.0f, 0.0f }, const glm::vec3& diffuse_ = { 0.0f, 0.0f, 0.0f }, const glm::vec3& specular_ = { 0.0f, 0.0f, 0.0f })
			: direction(direction_), ambient(ambient_), diffuse(diffuse_), specular(specular_), ambientStrenght(0.2f) {}
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


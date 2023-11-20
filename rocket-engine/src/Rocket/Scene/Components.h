#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
			glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), rotation.x, { 1, 0, 0 }) *
								  glm::rotate(glm::mat4(1.0f), rotation.y, { 0, 1, 0 }) *
								  glm::rotate(glm::mat4(1.0f), rotation.z, { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), position) * rotateMat * glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct SpriteRendererComponent {
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color_) :color(color_) {}
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


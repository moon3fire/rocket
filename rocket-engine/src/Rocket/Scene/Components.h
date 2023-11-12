#pragma once

#include <glm/glm.hpp>

#include "Rocket/Renderer/Camera.h"

namespace Rocket {

	struct TagComponent {
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag_) :tag(tag_) {}
	};

	struct TransformComponent {
		glm::mat4 transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform_) :transform(transform_) {}

		operator glm::mat4& () { return transform; }
		operator const glm::mat4& () const { return transform; }
	};

	struct SpriteRendererComponent {
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color_) :color(color_) {}
	};

	struct CameraComponent {
		Camera camera;
		bool primary = true; // TODO: move into scene

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const glm::mat4& projection) :camera(projection) {}
	};

} // namespace Rocket


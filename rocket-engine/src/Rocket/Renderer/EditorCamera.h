#pragma once

#include "Camera.h"
#include "Rocket/Core/Timestep.h"
#include "Rocket/Events/Event.h"
#include "Rocket/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Rocket {

	class EditorCamera : public Camera {
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void onUpdate(Timestep ts);
		void onEvent(Event& event);

		inline float getDistance() const { return m_distance; }
		inline void setDistance(float distance) { m_distance = distance; }

		inline void setViewportSize(float width, float height) { m_viewportWidth = width; m_viewportHeight = height; updateProjection(); }

		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
		glm::mat4 getViewProjection() const { return m_projection * m_viewMatrix; }

		glm::vec3 getUpDirection() const;
		glm::vec3 getRightDirection() const;
		glm::vec3 getForwardDirection() const;
		const glm::vec3& getPosition() const { return m_position; }
		glm::quat getOrientation() const;

		float getZoomLevel() const { return m_distance / std::tan(glm::radians(0.5f * m_FOV)); }
		float getPitch() const { return m_pitch; }
		float getYaw() const { return m_yaw; }

	private:
		void updateProjection();
		void updateView();

		bool onMouseScroll(MouseScrolledEvent& event);

		void mousePan(const glm::vec2& delta);
		void mouseRotate(const glm::vec2& delta);
		void mouseZoom(float delta);

		glm::vec3 calculatePosition() const;

		std::pair<float, float> panSpeed() const;
		float rotationSpeed() const;
		float zoomSpeed() const;

	private:
		float m_FOV = 45.0f, m_aspectRatio = 1.778f, m_nearClip = 0.1f, m_farClip = 1000.0f;

		glm::mat4 m_viewMatrix;
		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_focalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_initialMousePosition;

		float m_distance = 10.0f;
		float m_pitch = 0.0f, m_yaw = 0.0f;

		float m_viewportWidth = 1280.0f, m_viewportHeight = 720.0f;
	};

} // namespace Rocket
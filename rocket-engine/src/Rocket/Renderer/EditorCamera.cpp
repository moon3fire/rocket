#include "rcktpch.h"
#include "EditorCamera.h"

#include "Rocket/Core/Input.h"
#include "Rocket/Core/KeyCodes.h"
#include "Rocket/Core/MouseButtonCodes.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Rocket {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
	: m_FOV(fov), m_aspectRatio(aspectRatio), m_nearClip(nearClip), m_farClip(farClip)
	, Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip)) {	updateView(); }

	void EditorCamera::updateProjection() {
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
		m_projection = glm::perspective(glm::radians(m_FOV), m_aspectRatio, m_nearClip, m_farClip);
	}

	void EditorCamera::updateView() {
		// m_yaw = m_pitch = 0.0f;
		m_position = calculatePosition();

		glm::quat orientation = getOrientation();
		m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
		m_viewMatrix = glm::inverse(m_viewMatrix);
	}

	std::pair<float, float> EditorCamera::panSpeed() const {
		float x = std::min(m_viewportWidth / 1000.0f, 2.4f); // max is 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_viewportHeight / 1000.0f, 2.4f); // max is 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::rotationSpeed() const {
		return 0.8f;
	}

	float EditorCamera::zoomSpeed() const {
		float distance = m_distance * 0.2f;
		distance = std::max(distance, 0.0f); // min distance is 0
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed is 100
		return speed;
	}

	void EditorCamera::onUpdate(Timestep ts) {
		if (Input::isKeyPressed(RCKT_KEY_LEFT_ALT)) {
			const glm::vec2& mouse{ Input::getMouseX(), Input::getMouseY() };
			glm::vec2 delta = (mouse - m_initialMousePosition) * 0.003f;
			m_initialMousePosition = mouse;

			if (Input::isMouseButtonPressed(RCKT_MOUSE_BUTTON_MIDDLE))
				mousePan(delta);
			else if (Input::isMouseButtonPressed(RCKT_MOUSE_BUTTON_LEFT))
				mouseRotate(delta);
			else if (Input::isMouseButtonPressed(RCKT_MOUSE_BUTTON_RIGHT))
				mouseZoom(delta.y);
		}

		updateView();
	}

	void EditorCamera::onEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MouseScrolledEvent>(RCKT_BIND_EVENT_FUNC(EditorCamera::onMouseScroll));
	}

	bool EditorCamera::onMouseScroll(MouseScrolledEvent& event) {
		float delta = event.getOffsetY() * 0.1f;
		mouseZoom(delta);
		updateView();
		return false;
	}

	void EditorCamera::mousePan(const glm::vec2& delta) {
		auto [xSpeed, ySpeed] = panSpeed();
		m_focalPoint += -getRightDirection() * delta.x * xSpeed * m_distance;
		m_focalPoint += getUpDirection() * delta.y * ySpeed * m_distance;
	}

	void EditorCamera::mouseRotate(const glm::vec2& delta) {
		float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		m_yaw += yawSign * delta.x * rotationSpeed();
		m_pitch += delta.y * rotationSpeed();
	}

	void EditorCamera::mouseZoom(float delta) {
		m_distance -= delta * zoomSpeed();
		if (m_distance < 1.0f) {
			m_focalPoint += getForwardDirection();
			m_distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::getUpDirection() const { return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f)); }
	glm::vec3 EditorCamera::getRightDirection() const { return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f)); }
	glm::vec3 EditorCamera::getForwardDirection() const { return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f)); }

	glm::vec3 EditorCamera::calculatePosition() const { return m_focalPoint - getForwardDirection() * m_distance; }
	glm::quat EditorCamera::getOrientation() const { return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f)); }

} // namespace Rocket
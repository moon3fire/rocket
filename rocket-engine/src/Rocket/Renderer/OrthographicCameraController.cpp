#include "rcktpch.h"
#include "OrthographicCameraController.h"

#include "Rocket/Core/Input.h"
#include "Rocket/Core/KeyCodes.h"

namespace Rocket {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
	:m_aspectRatio(aspectRatio)
	,m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_rotation(rotation) {

	}

	void OrthographicCameraController::onUpdate(Timestep ts) {
		RCKT_PROFILE_FUNCTION();

		// TODO: move translations into functions to be dependent at zoom level
		if (Input::isKeyPressed(RCKT_KEY_A)) {
			m_cameraPosition.x -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}
		else if (Input::isKeyPressed(RCKT_KEY_D)) {
			m_cameraPosition.x += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}
		if (Input::isKeyPressed(RCKT_KEY_S)) {
			m_cameraPosition.x -= -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}
		else if (Input::isKeyPressed(RCKT_KEY_W)) {
			m_cameraPosition.x += -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		if (m_rotation) {
			if (Input::isKeyPressed(RCKT_KEY_Q))
				m_cameraRotation += m_cameraRotationSpeed * ts;
			if (Input::isKeyPressed(RCKT_KEY_E))
				m_cameraRotation -= m_cameraRotationSpeed * ts;

			if (m_cameraRotation > 180.0f)
				m_cameraRotation -= 360.0f;
			else if (m_cameraRotation <= -180.0f)
				m_cameraRotation += 360.0f;

			m_camera.setRotation(m_cameraRotation);
		}

		m_camera.setPosition(m_cameraPosition);
	}

	void OrthographicCameraController::onEvent(Event& event) {
		RCKT_PROFILE_FUNCTION();

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MouseScrolledEvent>(RCKT_BIND_EVENT_FUNC(OrthographicCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(RCKT_BIND_EVENT_FUNC(OrthographicCameraController::onWindowResized));
	}

	bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& event) {
		RCKT_PROFILE_FUNCTION();

		m_zoomLevel -= event.getOffsetY() * 0.45f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

	bool OrthographicCameraController::onWindowResized(WindowResizeEvent& event) {
		RCKT_PROFILE_FUNCTION();

		m_aspectRatio = (float)event.getWidth() / (float)event.getHeight();
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

} // namespace Rocket
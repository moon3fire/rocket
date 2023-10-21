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
		// TODO: move translations into functions to be dependent at zoom level
		if (Input::isKeyPressed(RCKT_KEY_A)) {
			m_cameraPosition.x -= m_cameraTranslationSpeed * ts;
		}
		if (Input::isKeyPressed(RCKT_KEY_D)) {
			m_cameraPosition.x += m_cameraTranslationSpeed * ts;
		}
		if (Input::isKeyPressed(RCKT_KEY_S)) {
			m_cameraPosition.y -= m_cameraTranslationSpeed * ts;
		}
		if (Input::isKeyPressed(RCKT_KEY_W)) {
			m_cameraPosition.y += m_cameraTranslationSpeed * ts;
		}

		if (m_rotation) {
			if (Input::isKeyPressed(RCKT_KEY_Q)) {
				m_cameraRotation += m_cameraRotationSpeed * ts;
			}
			if (Input::isKeyPressed(RCKT_KEY_E)) {
				m_cameraRotation -= m_cameraRotationSpeed * ts;
			}
			m_camera.setRotation(m_cameraRotation);
		}

		m_camera.setPosition(m_cameraPosition);
	}

	void OrthographicCameraController::onEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MouseScrolledEvent>(RCKT_BIND_EVENT_FUNC(OrthographicCameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(RCKT_BIND_EVENT_FUNC(OrthographicCameraController::onWindowResized));
	}

	bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& event) {
		m_zoomLevel -= event.getOffsetY() * 0.25f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

	bool OrthographicCameraController::onWindowResized(WindowResizeEvent& event) {
		m_aspectRatio = (float)event.getWidth() / (float)event.getHeight();
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

} // namespace Rocket
#pragma once

#include "Rocket/Renderer/OrthographicCamera.h"
#include "Rocket/Core/Timestep.h"

#include "Rocket/Events/ApplicationEvent.h"
#include "Rocket/Events/MouseEvent.h"

namespace Rocket {

	class OrthographicCameraController {

	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep ts);
		void onEvent(Event& event);

		OrthographicCamera2D& getCamera() { return m_camera; }
		const OrthographicCamera2D& getCamera() const { return m_camera; }

		float getZoomLevel() const { return m_zoomLevel; }
		void setZoomLevel(float zoomLevel) { m_zoomLevel = zoomLevel; }
	private:
		bool onMouseScrolled(MouseScrolledEvent& event);
		bool onWindowResized(WindowResizeEvent& event);

	private:
		float m_aspectRatio;
		float m_zoomLevel = 1.0f;
		OrthographicCamera2D m_camera;

		bool m_rotation;

		glm::vec3 m_cameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_cameraRotation = 0.0f;
		float m_cameraTranslationSpeed = 3.5f, m_cameraRotationSpeed = 2.0f;

	};



} // namespace Rocket

#pragma once

#include "Rocket/Renderer/Camera.h"

namespace Rocket {

	class SceneCamera : public Camera {
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void setOrthographic(float size, float nearClip, float farClip);
		void setPerspective(float verticalFOV, float nearClip, float farClip);

		//perspective
		float getPerspectiveVerticalFOV() const { return m_perspectiveFOV; }
		void setPerspectiveVerticalFOV(float verticalFOV) { m_perspectiveFOV = verticalFOV; recalculateProjection(); }
		float getPerspectiveNearClip() const { return m_perspectiveNear; }
		void setPerspectiveNearClip(float nearClip) { m_perspectiveNear = nearClip; recalculateProjection(); }
		float getPerspectiveFarClip() const { return m_perspectiveFar; }
		void setPerspectiveFarClip(float farClip) { m_perspectiveFar = farClip; recalculateProjection(); }

		//orthographic
		float getOrthographicSize() const { return m_orthographicSize; }
		void setOrthographicSize(float size) { m_orthographicSize = size; recalculateProjection(); }
		float getOrthographicNearClip() const { return m_orthographicNear; }
		float getOrthographicFarClip() const { return m_orthographicFar; }
		void setOrthographicNearClip(float nearClip) { m_orthographicNear = nearClip; recalculateProjection(); }
		void setOrthographicFarClip(float farClip) { m_orthographicFar = farClip; recalculateProjection(); }

		void setViewportSize(uint32_t width, uint32_t height);
	
		ProjectionType getProjectionType() const { return m_projectionType; }
		void setProjectionType(ProjectionType type) { m_projectionType = type; recalculateProjection(); }
	
	private:
		void recalculateProjection();
	private:
		ProjectionType m_projectionType = ProjectionType::Orthographic;

		float m_perspectiveFOV = glm::radians(45.0f);
		float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;

		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;


		float m_aspectRatio = 0.0f;
	};

} // namespace Rocket
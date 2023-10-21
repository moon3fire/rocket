#include "rcktpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Rocket {

	OrthographicCamera2D::OrthographicCamera2D(float left, float right, float bottom, float top)
	:m_projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_viewMatrix(1.0f)
	,m_position(glm::vec3(0)), m_rotation(0.f) {
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthographicCamera2D::setProjection(float left, float right, float bottom, float top) {
		m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void OrthographicCamera2D::recalculateViewMatrix() {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
			glm::rotate(glm::mat4(1.0f), m_rotation, glm::vec3(0, 0, 1));
		
		m_viewMatrix = glm::inverse(transform);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}


} // namespace Rocket
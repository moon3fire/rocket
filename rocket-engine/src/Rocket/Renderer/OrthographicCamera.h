#pragma once

#include <glm/glm.hpp>

namespace Rocket {

	class OrthographicCamera2D {

	public:
		OrthographicCamera2D(float left, float right, float bottom, float top);

		void setProjection(float left, float right, float bottom, float top);

		const glm::vec3& getPosition() const { return m_position; }
		void setPosition(const glm::vec3& position) { m_position = position; recalculateViewMatrix(); }

		float getRotation() const { return m_rotation; }
		void setRotation(float rotation) { m_rotation = rotation; recalculateViewMatrix(); }


		const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return m_viewProjectionMatrix; }

	private:
		void recalculateViewMatrix();

		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjectionMatrix;

		glm::vec3 m_position;
		float m_rotation;
	};

} // namespace Rocket
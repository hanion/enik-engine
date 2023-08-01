#pragma once

#include <glm/glm.hpp>

namespace Enik {

class OrthographicCamera {
public:
	OrthographicCamera(float left, float right, float bottom, float top);


	const glm::vec3& GetPosition() const { return m_Position; }
	void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

	const glm::vec3& GetRotation() const { return m_Rotation; }
	void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

	void SetProjection(float left, float right, float bottom, float top);

	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
private:
	void RecalculateViewMatrix();

private:
	glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewProjectionMatrix;


	glm::vec3 m_Position = glm::vec3(0.0f);
	glm::vec3 m_Rotation = glm::vec3(0.0f);
};

}
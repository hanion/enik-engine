#include "orthographic_camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <base.h>

namespace Enik {
OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)) {
	EN_PROFILE_SCOPE;

	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::SetProjection(float left, float right, float bottom, float top) {
	EN_PROFILE_SCOPE;

	m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::RecalculateViewMatrix() {
	EN_PROFILE_SCOPE;

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);
	transform = glm::rotate(transform, glm::radians(m_Rotation.x), glm::vec3(1, 0, 0));
	transform = glm::rotate(transform, glm::radians(m_Rotation.y), glm::vec3(0, 1, 0));
	transform = glm::rotate(transform, glm::radians(m_Rotation.z), glm::vec3(0, 0, 1));

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
}
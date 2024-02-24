#include <pch.h>
#include "scene_camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Enik {

SceneCamera::SceneCamera() {
	RecalculateProjection();
}

SceneCamera::~SceneCamera() {
}

void SceneCamera::SetOrthographic(float size, float near, float far) {
	m_OrthographicSize = size;
	m_OrthographicNear = near;
	m_OrthographicFar  = far;
	RecalculateProjection();
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height) {
	m_AspectRatio  = (float)width / (float)height;
	m_ScreenWidth  = width;
	m_ScreenHeight = height;
	RecalculateProjection();
}

void SceneCamera::SetViewportSize(glm::vec2 position, uint32_t width, uint32_t height) {
	m_ScreenPosition = position;
	SetViewportSize(width, height);
}

glm::vec2 SceneCamera::GetWorldPosition(const float x, const float y) const {
	float ortho_left   = -m_OrthographicSize * m_AspectRatio * 0.5f;
	float ortho_right  =  m_OrthographicSize * m_AspectRatio * 0.5f;
	float ortho_bottom = -m_OrthographicSize * 0.5f;
	float ortho_top    =  m_OrthographicSize * 0.5f;

	float world_x = ortho_left   +         (x - m_ScreenPosition.x) / m_ScreenWidth   * (ortho_right - ortho_left);
	float world_y = ortho_bottom + (1.0f - (y - m_ScreenPosition.y) / m_ScreenHeight) * (ortho_top - ortho_bottom);
	return { world_x, world_y };
}

void SceneCamera::RecalculateProjection() {
	float ortho_left   = -m_OrthographicSize * m_AspectRatio * 0.5f;
	float ortho_right  =  m_OrthographicSize * m_AspectRatio * 0.5f;
	float ortho_bottom = -m_OrthographicSize * 0.5f;
	float ortho_top    =  m_OrthographicSize * 0.5f;

	m_Projection = glm::ortho(ortho_left, ortho_right, ortho_bottom, ortho_top, m_OrthographicNear, m_OrthographicFar);
}

}
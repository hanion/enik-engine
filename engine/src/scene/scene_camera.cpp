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
	m_AspectRatio     = (float)width / (float)height;
	RecalculateProjection();
}

void SceneCamera::RecalculateProjection() {
	float orthoLeft   = -m_OrthographicSize * m_AspectRatio * 0.5f;
	float orthoRight  =  m_OrthographicSize * m_AspectRatio * 0.5f;
	float orthoBottom = -m_OrthographicSize * 0.5f;
	float orthoTop    =  m_OrthographicSize * 0.5f;

	m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
}

}
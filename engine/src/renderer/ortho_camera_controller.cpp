#include "ortho_camera_controller.h"

#include <pch.h>

#include "core/input.h"
#include "events/key_codes.h"

namespace Enik {

OrthographicCameraController::OrthographicCameraController(float aspect_ratio, bool rotation)
	: m_AspectRatio(aspect_ratio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation) {
}

OrthographicCameraController::OrthographicCameraController(float left, float right, float bottom, float top, bool rotation)
	: m_AspectRatio((right-left)/(bottom-top)), m_Camera(left, right, bottom, top), m_Rotation(rotation) {
}

void OrthographicCameraController::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	Moving();
}
void OrthographicCameraController::OnEvent(Event& e) {
	EventDispatcher dispatcher = EventDispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>(EN_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
	dispatcher.Dispatch<WindowResizeEvent> (EN_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));

	dispatcher.Dispatch<MouseButtonPressedEvent> (EN_BIND_EVENT_FN(OrthographicCameraController::OnMouseButtonPressed));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(EN_BIND_EVENT_FN(OrthographicCameraController::OnMouseButtonReleased));

	dispatcher.Dispatch<MouseMovedEvent>(EN_BIND_EVENT_FN(OrthographicCameraController::OnMouseMoved));
}

void OrthographicCameraController::OnResize(float width, float height) {
	m_AspectRatio = width / height;
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e) {
	m_ZoomLevel -= e.GetYOffset() * 0.1f;
	m_ZoomLevel = glm::clamp(m_ZoomLevel, 0.05f, 100.0f);

	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);

	m_CameraMoveSpeed = m_ZoomLevel;
	return false;
}
bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e) {
	OnResize((float)e.GetWidth(), (float)e.GetHeight());
	return false;
}

bool OrthographicCameraController::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
	if (e.GetMouseButton() == 1) {
		m_StartMoving = true;
		m_Moving = true;
		e.Handled = true;
	}
	return false;
}

bool OrthographicCameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
	if (e.GetMouseButton() == 1) {
		m_Moving = false;
		m_StartMoving = false;
		e.Handled = true;
	}
	return false;
}

bool OrthographicCameraController::OnMouseMoved(MouseMovedEvent& e) {
	if (m_StartMoving) {
		m_MouseStartPos = glm::vec2(e.GetX(), e.GetY());
		m_StartMoving = false;
	}
	return false;
}
void OrthographicCameraController::Moving() {
	if (not m_Moving or m_StartMoving) {
		return;
	}

	auto [x, y] =Input::GetMousePosition();
	glm::vec2 current_pos = glm::vec2(x, y);
	glm::vec2 diff = current_pos - m_MouseStartPos;
	m_MouseStartPos = current_pos;

	diff *= m_ZoomLevel * 0.0035f;

	m_CameraPosition.x -= diff.x;
	m_CameraPosition.y += diff.y;
	m_Camera.SetPosition(m_CameraPosition);

}
}
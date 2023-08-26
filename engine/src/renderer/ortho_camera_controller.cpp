#include "ortho_camera_controller.h"

#include <pch.h>

#include "core/input.h"
#include "events/key_codes.h"

namespace Enik {

OrthographicCameraController::OrthographicCameraController(float aspect_ratio, bool rotation)
	: m_AspectRatio(aspect_ratio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation) {
}

void OrthographicCameraController::OnUpdate(Timestep timestep) {
	EN_PROFILE_SCOPE;

	float deltaTime = timestep.GetSeconds();

	if (Input::IsKeyPressed(Key::A)) {
		m_CameraPosition -= glm::vec3(1, 0, 0) * m_CameraMoveSpeed * deltaTime;
		m_Camera.SetPosition(m_CameraPosition);
	}
	else if (Input::IsKeyPressed(Key::D)) {
		m_CameraPosition += glm::vec3(1, 0, 0) * m_CameraMoveSpeed * deltaTime;
		m_Camera.SetPosition(m_CameraPosition);
	}

	if (Input::IsKeyPressed(Key::W)) {
		m_CameraPosition += glm::vec3(0, 1, 0) * m_CameraMoveSpeed * deltaTime;
		m_Camera.SetPosition(m_CameraPosition);
	}
	else if (Input::IsKeyPressed(Key::S)) {
		m_CameraPosition -= glm::vec3(0, 1, 0) * m_CameraMoveSpeed * deltaTime;
		m_Camera.SetPosition(m_CameraPosition);
	}

	if (m_Rotation) {
		if (Input::IsKeyPressed(Key::Q)) {
			m_CameraRotation += glm::vec3(0, 0, 1) * m_CameraRotationSpeed * deltaTime;
			m_Camera.SetRotation(m_CameraRotation);
		}
		else if (Input::IsKeyPressed(Key::E)) {
			m_CameraRotation -= glm::vec3(0, 0, 1) * m_CameraRotationSpeed * deltaTime;
			m_Camera.SetRotation(m_CameraRotation);
		}
	}
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
	m_ZoomLevel = glm::clamp(m_ZoomLevel, 0.05f, 10.0f);

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
		m_IsMoving = true;
	}
	return false;
}

bool OrthographicCameraController::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
	if (e.GetMouseButton() == 1) {
		m_IsMoving = false;
		m_StartedMoving = false;
	}
	return false;
}

bool OrthographicCameraController::OnMouseMoved(MouseMovedEvent& e) {
	if (m_IsMoving && !m_StartedMoving) {
		m_MouseStartPos = glm::vec2(e.GetX(), e.GetY());
		m_StartedMoving = true;
	}
	else if (m_IsMoving && m_StartedMoving) {
		// this is arbitrary, switch to calculating camera with pixels
		glm::vec3 diff = glm::vec3(m_MouseStartPos.x - e.GetX(), e.GetY() - m_MouseStartPos.y, 0.0f);
		m_CameraPosition += diff / 170.0f * m_ZoomLevel;
		m_Camera.SetPosition(m_CameraPosition);
		m_StartedMoving = false;
	}

	return false;
}
}
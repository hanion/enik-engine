#include <pch.h>
#include "ortho_camera_controller.h"

#include "input.h"
#include "events/key_codes.h"

namespace Enik {

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
	: m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation) {
	
}

void OrthographicCameraController::OnUpdate(Timestep timestep) {
	float deltaTime = timestep.GetSeconds();

	if(Input::IsKeyPressed(Key::A)){
		m_CameraPosition -= glm::vec3(1, 0, 0) * m_CameraMoveSpeed * deltaTime;
	}
	else if (Input::IsKeyPressed(Key::D)) {
		m_CameraPosition += glm::vec3(1, 0, 0) * m_CameraMoveSpeed * deltaTime;
	}

	if(Input::IsKeyPressed(Key::W)){
		m_CameraPosition += glm::vec3(0, 1, 0) * m_CameraMoveSpeed * deltaTime;
	}
	else if (Input::IsKeyPressed(Key::S)) {
		m_CameraPosition -= glm::vec3(0, 1, 0) * m_CameraMoveSpeed * deltaTime;
	}

	m_Camera.SetPosition(m_CameraPosition);
	
	if (m_Rotation) {
		if(Input::IsKeyPressed(Key::Q)){
			m_CameraRotation += glm::vec3(0, 0, 1) * m_CameraRotationSpeed * deltaTime;
		}
		else if (Input::IsKeyPressed(Key::E)) {
			m_CameraRotation -= glm::vec3(0, 0, 1) * m_CameraRotationSpeed * deltaTime;
		}
		m_Camera.SetRotation(m_CameraRotation);
	}

}
void OrthographicCameraController::OnEvent(Event& e) {
	EventDispatcher dispatcher = EventDispatcher(e);
	dispatcher.Dispatch<MouseScrolledEvent>(EN_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
	dispatcher.Dispatch<WindowResizeEvent> (EN_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));

}
bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e) {
	m_ZoomLevel -= e.GetYOffset() * 0.1f;
	m_ZoomLevel = glm::clamp(m_ZoomLevel, 0.05f, 10.0f);
	
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	
	m_CameraMoveSpeed = m_ZoomLevel;
    return false;
}
bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e) {
    m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	return false;
}

}
#pragma once
#include "renderer/orthographic_camera.h"
#include "core/timestep.h"

#include "events/application_event.h"
#include "events/mouse_event.h"

namespace Enik {

class OrthographicCameraController {
public:
	OrthographicCameraController(float aspect_ratio, bool rotation = false);

	void OnUpdate(Timestep timestep);
	void OnEvent(Event& e, bool is_viewport_hovered);

	void OnResize(float width, float height);

	OrthographicCamera& GetCamera() { return m_Camera; }
	const OrthographicCamera& GetCamera() const { return m_Camera; }

private:
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnWindowResized(WindowResizeEvent& e);
	bool OnMouseButtonPressed (MouseButtonPressedEvent& e);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	bool OnMouseMoved(MouseMovedEvent& e);

private:
	float m_AspectRatio = 1.6f/0.9f;
	float m_ZoomLevel = 10.0f;

	OrthographicCamera m_Camera;

	bool m_Rotation;
	glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_CameraRotation = glm::vec3(0.0f, 0.0f, 0.0f);

	float m_CameraMoveSpeed = 1.0f;
	float m_CameraRotationSpeed = 50.0f;

	bool m_IsMoving = false;
	bool m_StartedMoving = false;
	glm::vec2 m_MouseStartPos;

};

}
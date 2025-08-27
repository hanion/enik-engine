#include "button_container.h"

namespace Enik {

void ButtonContainer::OnUpdate(Timestep ts) {
	Entity cam = FindEntityByUUID(m_CameraUUID);

	if (not cam) {
		CONSOLE_DEBUG_ERROR("ButtonContainer couldn't find Camera!");
		return;
	}

	glm::vec3 mouse = glm::vec3(cam.Get<Component::Camera>().Cam.GetWorldPosition(Input::GetMousePosition()), 0);
	mouse += cam.Get<Component::Transform>().LocalPosition;

	Entity* hovering = nullptr;
	for (Entity& child : GetChildren()) {

		// if it has sprite, it is considered a button
		if(not child.Has<Component::SpriteRenderer>()) {
			continue;
		}


		auto& tr = child.Get<Component::Transform>();
		float left   = tr.LocalPosition.x - (tr.LocalScale.x * 0.5f);
		float right  = tr.LocalPosition.x + (tr.LocalScale.x * 0.5f);
		float bottom = tr.LocalPosition.y - (tr.LocalScale.y * 0.5f);
		float top    = tr.LocalPosition.y + (tr.LocalScale.y * 0.5f);
		if (mouse.x > left and mouse.x < right and
			mouse.y > bottom and mouse.y < top) {
			hovering = &child;
		} else {
			child.Get<Component::SpriteRenderer>().Color.a = 1.0f;
		}

	}

	m_HoveredButton = hovering;

	if (m_HoveredButton) {
		m_HoveredButton->Get<Component::SpriteRenderer>().Color.a = 0.8f;

		if (m_OldHoveredButton != m_HoveredButton) {
			m_OldHoveredButton = m_HoveredButton;
			if (Has<Component::AudioSources>()) {
				Get<Component::AudioSources>().Play("hover");
			}
		}

	} else {
		m_OldHoveredButton = nullptr;
	}

	if (m_ClickedButton) {
		m_ClickedButton->Get<Component::SpriteRenderer>().Color.a = 0.5f;
		if (m_OldClickedButton != m_ClickedButton) {
			m_OldClickedButton = m_ClickedButton;
			if (Has<Component::AudioSources>()) {
				Get<Component::AudioSources>().Play("click");
			}
		}
	} else {
		m_OldClickedButton = nullptr;
	}

	if (m_SelectedButton) {
		m_SelectedButton->Get<Component::SpriteRenderer>().Color.a = 0.1f;
		m_SelectedButton = nullptr;
		if (Has<Component::AudioSources>()) {
			Get<Component::AudioSources>().Play("select");
		}
	}

}


void ButtonContainer::OnMouseButtonPressed(const MouseButtonPressedEvent& event) {
	if (not m_HoveredButton) {
		m_ClickedButton = nullptr;
		return;
	}

	if (event.GetMouseButton() == Mouse::Button0) {
		m_ClickedButton = m_HoveredButton;
	}

}

void ButtonContainer::OnMouseButtonReleased(const MouseButtonReleasedEvent& event) {
	if (not m_HoveredButton or not m_ClickedButton) {
		m_HoveredButton = nullptr;
		m_ClickedButton = nullptr;
		return;
	}


	if (event.GetMouseButton() == Mouse::Button0) {
		if (m_HoveredButton == m_ClickedButton) {
			// click confirmed !!!
			m_SelectedButton = m_ClickedButton;
			// TODO do something with this info now, the button is pressed.
			OnPressed(*m_SelectedButton);
		}
		m_HoveredButton = nullptr;
		m_ClickedButton = nullptr;
	}


}


}
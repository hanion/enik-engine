#pragma once

#include <base.h>
#include "../layer.h"

#include "../../events/event.h"
#include "../../events/key_event.h"
#include "../../events/mouse_event.h"
#include "../../events/application_event.h"


namespace Enik {

class ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	void OnAttach();
	void OnDetach();
	void OnUpdate();
	void OnEvent(Event& event);

private:
	bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
	bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
	bool OnMouseMovedEvent(MouseMovedEvent& e);
	bool OnMouseScrolledEvent(MouseScrolledEvent& e);
	
	bool OnKeyPressedEvent(KeyPressedEvent& e);
	bool OnKeyReleasedEvent(KeyReleasedEvent& e);
	bool OnKeyTypedEvent(KeyTypedEvent& e);
	bool OnWindowResizeEvent(WindowResizeEvent& e);


private:
	float m_Time = 0.0f;

};



}
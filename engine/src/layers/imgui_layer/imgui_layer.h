#pragma once

#include <base.h>
#include "layers/layer.h"

#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "events/application_event.h"


namespace Enik {

class ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	void OnAttach();
	void OnDetach();
	void OnImGuiRender();

	void Begin();
	void End();
	void ShowTestingWindow();

private:
	float m_Time = 0.0f;

};



}
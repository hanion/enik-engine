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
	virtual void OnEvent(Event& e) override final;

	void Begin();
	void End();
	void ShowTestingWindow();

	void BlockEvents(bool block) { m_BlockEvents = block; }

private:
	bool m_BlockEvents = true;

};



}
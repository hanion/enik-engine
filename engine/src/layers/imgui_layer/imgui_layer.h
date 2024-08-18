#pragma once

#include <base.h>

#include "events/application_event.h"
#include "events/event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "layers/layer.h"

namespace Enik {

class ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	void OnAttach() override;
	void OnDetach() override;
	virtual void OnEvent(Event& e) override final;

	void Begin();
	void End();
	void ShowTestingWindow();

	void BlockEvents(bool block) { m_BlockEvents = block; }

	uint32_t GetActiveWidgetID();

private:
	bool m_BlockEvents = true;
};

}
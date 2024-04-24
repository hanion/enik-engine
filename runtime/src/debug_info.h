#pragma once
#include <base.h>
#include <chrono>
#include "core/timestep.h"
#include "events/key_event.h"


namespace Enik {

class DebugInfoPanel {
public:
	void ShowDebugInfoPanel(Timestep timestep);

	bool OnKeyReleased(KeyReleasedEvent& event);

private:
	int m_ShowDebugInfoPanel = 0;

	std::chrono::high_resolution_clock::time_point m_StartTime = std::chrono::high_resolution_clock::now();
};

}
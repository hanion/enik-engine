#pragma once
#include <base.h>
#include "events/key_event.h"
#include "core/timestep.h"


namespace Enik {

class DebugInfoPanel {
public:
	void BeginMenu();

	void ShowDebugInfoPanel(Timestep timestep);

	bool OnKeyReleased(KeyReleasedEvent& event);

private:
	bool m_ShowPerformance   = false;
	bool m_ShowRendererStats = false;
	bool m_ShowRenderer      = false;
	bool m_ShowProject       = false;
	bool m_ShowWindow        = false;
	bool m_ShowProgram       = false;

	std::chrono::high_resolution_clock::time_point m_StartTime = std::chrono::high_resolution_clock::now();

};

}
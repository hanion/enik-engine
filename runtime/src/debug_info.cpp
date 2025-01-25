#include "debug_info.h"
#include <imgui/imgui.h>
#include "renderer/renderer2D.h"
#include "project/project.h"
#include "core/application.h"
#include "core/input.h"


namespace Enik {


void DebugInfoPanel::ShowDebugInfoPanel(Timestep timestep) {
	if (not m_ShowDebugInfoPanel) {
		return;
	}

	constexpr ImGuiWindowFlags debug_info_window_flags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoInputs
	;
	constexpr ImVec4 color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);


	ImGui::Begin("DEBUG_INFO", nullptr, debug_info_window_flags);

	ImGui::TextColored(color, "Performance");
	ImGui::Text("	Frame Time = %.2fms", timestep.GetMilliseconds());
	ImGui::Text("	FPS = %.0f", (1.0f/timestep.GetSeconds()));

	if (m_ShowDebugInfoPanel > 1) {
		ImGui::Spacing();
		ImGui::Spacing();

		auto stats = Renderer2D::GetStats();
		ImGui::TextColored(color,"Renderer Stats");
		ImGui::Text("	Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("	Quad Count: %d", stats.QuadCount);
		ImGui::Text("	Total Vertex Count: %d", stats.GetTotalVertexCount());
		ImGui::Text("	Total Index  Count: %d", stats.GetTotalIndexCount());
	}

	if (m_ShowDebugInfoPanel > 2) {
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::TextColored(color, "Renderer");
		ImGui::Text("	Vendor: %s",   (const char*)glGetString(GL_VENDOR));
		ImGui::Text("	Renderer: %s", (const char*)glGetString(GL_RENDERER));
		ImGui::Text("	Version: %s",  (const char*)glGetString(GL_VERSION));
	}

	if (m_ShowDebugInfoPanel > 3) {
		ImGui::Spacing();
		ImGui::Spacing();

		ProjectConfig& config = Project::GetActive()->GetConfig();

		ImGui::TextColored(color, "Project");
		ImGui::Text("	Name: %s",  config.project_name.c_str());
		ImGui::Text("	Path: %s",  Project::GetAbsolutePath(".").string().c_str());
		ImGui::Text("	Start Scene: %s",  config.start_scene.string().c_str());
		ImGui::Text("	Script Module: %s",  config.script_module_path.string().c_str());
		ImGui::Text("	Asset Registry: %s",  config.asset_registry_path.string().c_str());
	}

	if (m_ShowDebugInfoPanel > 4) {
		ImGui::Spacing();
		ImGui::Spacing();

		Window& window = Application::Get().GetWindow();

		ImGui::TextColored(color, "Window");
		ImGui::Text("	Size: %dx%d", window.GetWidth(), window.GetHeight());

		if (window.IsVSync()) {
			ImGui::Text("	VSync: On");
		} else {
			ImGui::Text("	VSync: Off");
		}
	}

	if (m_ShowDebugInfoPanel > 5) {
		ImGui::Spacing();
		ImGui::Spacing();

		auto current_time = std::chrono::high_resolution_clock::now();
		uint64_t duration = std::chrono::duration_cast<std::chrono::seconds>(current_time - m_StartTime).count();

		int minutes = duration / 60;
		int seconds = duration % 60;

		ImGui::TextColored(color, "Program");
		ImGui::Text("	Time: %d minutes %d seconds", minutes, seconds);
	}

	ImGui::End();
}

// set this manually
constexpr int DEBUG_INFO_PANEL_COUNT = 6;

bool DebugInfoPanel::OnKeyReleased(KeyReleasedEvent& event) {
	if (Input::IsKeyPressed(Key::LeftControl)) {
		if (event.GetKeyCode() == Key::I) {
			m_ShowDebugInfoPanel++;
			if (m_ShowDebugInfoPanel > DEBUG_INFO_PANEL_COUNT) {
				m_ShowDebugInfoPanel = 0;
			}
		}
		else if (event.GetKeyCode() == Key::U) {
			if (m_ShowDebugInfoPanel == DEBUG_INFO_PANEL_COUNT) {
				m_ShowDebugInfoPanel = 0;
			} else {
				m_ShowDebugInfoPanel = DEBUG_INFO_PANEL_COUNT;
			}
		}
	}
	return false;
}


}

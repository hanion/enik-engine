#include "debug_info.h"
#include <imgui/imgui.h>
#include "renderer/renderer2D.h"
#include "project/project.h"
#include "core/application.h"
#include "core/input.h"

namespace Enik {


void DebugInfoPanel::BeginMenu() {
	if (ImGui::BeginMenu("Debug Info")) {
		ImGui::Checkbox("Performance",    &m_ShowPerformance);
		ImGui::Checkbox("Renderer Stats", &m_ShowRendererStats);
		ImGui::Checkbox("Renderer",       &m_ShowRenderer);
		ImGui::Checkbox("Project",        &m_ShowProject);
		ImGui::Checkbox("Window",         &m_ShowWindow);
		ImGui::Checkbox("Program",        &m_ShowProgram);
		ImGui::EndMenu();
	}
}


void DebugInfoPanel::ShowDebugInfoPanel(Timestep timestep) {
	constexpr ImGuiWindowFlags debug_info_window_flags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground
	;
	constexpr ImVec4 color = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

	ImGui::Begin("DEBUG_INFO", nullptr, debug_info_window_flags);

	if (m_ShowPerformance) {
		ImGui::TextColored(color, "Performance");
		ImGui::Text("	Frame Time = %.2fms", timestep.GetMilliseconds());
		ImGui::Text("	FPS = %.0f", (1.0f/timestep.GetSeconds()));
	}

	if (m_ShowRendererStats) {
		ImGui::Spacing();
		ImGui::Spacing();

		auto stats = Renderer2D::GetStats();
		ImGui::TextColored(color,"Renderer Stats");
		ImGui::Text("	Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("	Quad Count: %d", stats.QuadCount);
		ImGui::Text("	Total Vertex Count: %d", stats.GetTotalVertexCount());
		ImGui::Text("	Total Index  Count: %d", stats.GetTotalIndexCount());
	}

	if (m_ShowRenderer) {
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::TextColored(color, "Renderer");
		ImGui::Text("	Vendor: %s",   (const char*)glGetString(GL_VENDOR));
		ImGui::Text("	Renderer: %s", (const char*)glGetString(GL_RENDERER));
		ImGui::Text("	Version: %s",  (const char*)glGetString(GL_VERSION));
	}

	if (m_ShowProject) {
		ImGui::Spacing();
		ImGui::Spacing();

		ProjectConfig& config = Project::GetActive()->GetConfig();

		ImGui::TextColored(color, "Project");
		ImGui::Text("	Name: %s",  config.project_name.c_str());
		ImGui::Text("	Path: %s",  Project::GetAbsolutePath(".").c_str());
		ImGui::Text("	Start Scene: %s",  config.start_scene.c_str());
		ImGui::Text("	Script Module: %s",  config.script_module_path.c_str());
	}

	if (m_ShowWindow) {
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

	if (m_ShowProgram) {
		ImGui::Spacing();
		ImGui::Spacing();

		auto current_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::seconds>(current_time - m_StartTime).count();

		int minutes = duration / 60;
		int seconds = duration % 60;

		ImGui::TextColored(color, "Program");
		ImGui::Text("	Time: %d minutes %d seconds", minutes, seconds);
	}


	ImGui::End();
}


bool DebugInfoPanel::OnKeyReleased(KeyReleasedEvent& event) {
	if (Input::IsKeyPressed(Key::LeftControl)) {
		if (event.GetKeyCode() == Key::U) {
			m_ShowPerformance = m_ShowRendererStats = m_ShowRenderer = m_ShowProject = m_ShowWindow = m_ShowProgram = not m_ShowPerformance;
		}
	}
    return false;
}


}
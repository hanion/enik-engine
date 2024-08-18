#include "editor_panel.h"
#include "core/log.h"
#include "utils/editor_colors.h"


namespace Enik {

int EditorPanel::s_PanelIDCounter = 0;

void EditorPanel::OnImGuiRender() {
	if (BeginPanel()) {
		RenderContent();
	}
	ImGui::End();
}

bool EditorPanel::BeginPanel() {
	ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::PanelTabPadding);
	bool is_open = ImGui::Begin(m_WindowName.c_str(), nullptr, ImGuiWindowFlags_None);
	ImGui::PopStyleVar();

	if (not is_open) {
		return false;
	}

	return true;
}

void EditorPanel::DockTo(ImGuiID dockspace_id) {
	ImGui::DockBuilderDockWindow(m_WindowName.c_str(), dockspace_id);
}

}

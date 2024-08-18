#include "editor_tab.h"
#include "imgui.h"
#include "utils/editor_colors.h"
#include "utils/imgui_utils.h"
#include <iostream>


namespace Enik {

int EditorTab::s_TabIDCounter = 0;

void EditorTab::OnImGuiRender() {
	if (BeginDockspace()) {
		RenderContent();
		if (not m_DockspaceInitialized) {
			InitializeDockspace();
			m_DockspaceInitialized = true;
		}
	}
	ImGui::End();
}

void EditorTab::DockTo(ImGuiID dockspace_id) {
	ImGui::DockBuilderDockWindow(m_WindowName.c_str(), dockspace_id);
	EN_CORE_TRACE("Tab {}", m_WindowName);
}

bool EditorTab::BeginDockspace() {
	if (m_Name.empty()) {
		return false;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::EditorTabPadding);

	int pushed_color_count = 0;
	ImGuiUtils::ColorFile(m_Name, pushed_color_count);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
	if (m_IsDirty) {
		flags |= ImGuiWindowFlags_UnsavedDocument;
	}

	bool dockspace_open = ImGui::Begin(m_WindowName.c_str(), &m_IsOpen, flags);

	ImGui::PopStyleColor(pushed_color_count);
	ImGui::PopStyleVar(4);

	if (!dockspace_open) {
		return false;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, EditorVars::PanelTabPadding);

	m_DockspaceID = ImGui::GetID(m_WindowName.c_str());
	ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::PopStyleVar();

	return true;
}


}


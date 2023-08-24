#include <pch.h>
#include "file_system.h"
#include <imgui/imgui.h>


namespace Enik {


FileSystemPanel::FileSystemPanel(const Ref<Scene> context) {
	SetContext(context);	
}

void FileSystemPanel::SetContext(const Ref<Scene>& context) {
	m_Context = context;
}

void FileSystemPanel::OnImGuiRender() {
	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("File System")) {
		ImGui::End();
		return;
	}


	if (not m_HasSearched) {
		SearchDirectory();
	}


	ImGui::BeginDisabled(m_CurrentPath == m_AssetsPath);
	if (ImGui::Button(" ^ ")) {
		if (!m_CurrentPath.empty()) {
			m_CurrentPath = m_CurrentPath.parent_path();
			m_HasSearched = false;
		}
	}
	ImGui::EndDisabled();

	ImGui::SameLine();
	std::filesystem::path relativePath = std::filesystem::relative(m_CurrentPath, m_AssetsPath);
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", relativePath.string().c_str());
	

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();


	ShowDirectoriesTable();

	ImGui::End();
}

// Search directory and sort entries
void FileSystemPanel::SearchDirectory() {
	m_Entries.clear();

	for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath)) {
		m_Entries.push_back(entry);
	}
	std::sort(m_Entries.begin(), m_Entries.end(), 
		[](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
			return a.path().filename().string() < b.path().filename().string();
	});

	m_HasSearched = true;
}

void FileSystemPanel::ShowDirectoriesTable() {
	ImGui::BeginChild("ScrollableTable", ImVec2(0,0), true);

	if (ImGui::BeginTable("Directory", 1)) {
		for (const auto& entry : m_Entries) {
			const auto& path = entry.path();
			const std::string fileName = path.filename().string();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (ImGui::Selectable(fileName.c_str())) {
				if (std::filesystem::is_directory(path)) {
					m_CurrentPath = path;
					m_HasSearched = false;
				}
				else {
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
}
}
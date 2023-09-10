#include "file_system.h"

#include <imgui/imgui.h>
#include <pch.h>

#include "project/project.h"
#include "../utils/sort.h"


namespace Enik {

FileSystemPanel::FileSystemPanel(const Ref<Scene> context) {
	SetContext(context);
}

void FileSystemPanel::SetContext(const Ref<Scene>& context) {
	m_Context = context;
	if (m_CurrentDirectory.empty()) {
		m_CurrentDirectory = Project::GetProjectDirectory();
	}
}

void FileSystemPanel::OnImGuiRender() {
	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("File System") or m_Context == nullptr or m_CurrentDirectory.empty()) {
		ImGui::End();
		return;
	}

	if (not m_HasSearched) {
		SearchDirectory();
	}

	ImGui::BeginDisabled(m_CurrentDirectory == Project::GetProjectDirectory());
	if (ImGui::Button(" ^ ")) {
		if (!m_CurrentDirectory.empty()) {
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
			m_HasSearched = false;
		}
	}
	ImGui::EndDisabled();

	ImGui::SameLine();

	std::string directory = std::filesystem::relative(m_CurrentDirectory, Project::GetProjectDirectory()).string();
	if (directory == ".") {
		directory = "res://";
	}
	else {
		directory = "res://" + directory + "/";
	}
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", directory.c_str());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ShowDirectoriesTable();

	ImGui::End();
}

// Search directory and sort entries
void FileSystemPanel::SearchDirectory() {
	m_Entries.clear();

	for (const auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
		m_Entries.push_back(entry);
	}

	Utils::SortDirectoryEntries(m_Entries);

	m_HasSearched = true;
}

void FileSystemPanel::ShowDirectoriesTable() {
	ImGui::BeginChild("ScrollableTable", ImVec2(0, 0), true);

	if (ImGui::BeginTable("Directory", 1)) {
		for (const auto& entry : m_Entries) {
			const auto& path = entry.path();
			std::string fileName = path.filename().string();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (entry.is_directory()) {
				fileName = fileName + "/";
			}

			if (ImGui::Selectable(fileName.c_str())) {
				if (std::filesystem::is_directory(path)) {
					m_CurrentDirectory = path;
					m_HasSearched = false;
				}
				else {
				}
			}

			if (entry.is_regular_file()) {
				if (ImGui::BeginDragDropSource()) {
					// Set payload to carry the index of our item (could be anything)
					ImGui::SetDragDropPayload("DND_FILE_PATH", path.string().c_str(), path.string().length() + 1);

					// Display preview (could be anything, e.g. when dragging an image we could decide to display
					// the filename and a small preview of the image, etc.)
					ImGui::Text(fileName.c_str());
					ImGui::EndDragDropSource();
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
}
}
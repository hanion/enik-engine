#include "file_system.h"

#include <imgui/imgui.h>
#include <pch.h>

#include "project/project.h"
#include "../utils/utils.h"


namespace Enik {

FileSystemPanel::FileSystemPanel(const Ref<Scene> context) {
	SetContext(context);
}

void FileSystemPanel::SetContext(const Ref<Scene>& context) {
	m_Context = context;
	if (m_CurrentDirectory.empty()) {
		ChangeDirectory(Project::GetProjectDirectory());
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
		Utils::SortDirectoryEntries(m_Entries);
		Utils::FilterFiles(m_Entries, m_Filters);
	}

	ImGui::BeginDisabled(m_CurrentDirectory == Project::GetProjectDirectory());
	if (ImGui::Button(" ^ ")) {
		if (!m_CurrentDirectory.empty()) {
			ChangeDirectory(m_CurrentDirectory.parent_path());
		}
	}
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", m_CurrentDirectoryText.c_str());

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ShowDirectoriesTable();

	ImGui::End();
}

// Search directory
void FileSystemPanel::SearchDirectory() {
	m_Entries.clear();

	for (const auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory)) {
		m_Entries.push_back(entry);
	}

	m_HasSearched = true;
}

void FileSystemPanel::ShowDirectoriesTable() {
	ImGui::BeginChild("ScrollableTable", ImVec2(0, 0), true);


	if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered()) {
		ImGui::OpenPopup("pop_directories_table");
	}
	if (ImGui::BeginPopup("pop_directories_table")) {
		if (ImGui::MenuItem("Create File")) {
			std::ofstream(m_CurrentDirectory / "new_file.escn").close();
		}
		ImGui::EndPopup();
	}

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
					ChangeDirectory(path);
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

void FileSystemPanel::ChangeDirectory(const std::filesystem::path& directory) {
	if (directory.empty() or not std::filesystem::exists(directory)) {
		return;
	}

	m_CurrentDirectory = directory;
	m_HasSearched = false;

	m_CurrentDirectoryText = std::filesystem::relative(m_CurrentDirectory, Project::GetProjectDirectory()).string();
	if (m_CurrentDirectoryText == ".") {
		m_CurrentDirectoryText = "res://";
	}
	else {
		m_CurrentDirectoryText = "res://" + m_CurrentDirectoryText + "/";
	}

	m_FileWatcher = CreateScope<filewatch::FileWatch<std::string>>(m_CurrentDirectory,
		[&](const std::string& path, const filewatch::Event change_type) {
			m_HasSearched = false;
		}
	);
}

}
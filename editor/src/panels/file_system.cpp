#include "file_system.h"

#include <imgui/imgui.h>
#include <pch.h>

#include "project/project.h"
#include "../utils/utils.h"
#include "../dialogs/dialog_confirm.h"
#include "../utils/imgui_utils.h"


namespace Enik {

void FileSystemPanel::SetContext(TextEditorPanel* text_editor_panel) {
	m_TextEditorPanel = text_editor_panel;
	if (m_CurrentDirectory.empty()) {
		ChangeDirectory(Project::GetProjectDirectory());
	}
}

void FileSystemPanel::OnImGuiRender() {
	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("File System") or m_CurrentDirectory.empty()) {
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
			std::string filename = path.filename().string();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (entry.is_directory()) {
				filename = filename + "/";
			}

			int pushed_color_count = 0;
			ImGuiUtils::ColorFileText(path, pushed_color_count);

			if (ImGui::Selectable(filename.c_str())) {
				if (entry.is_directory()) {
					ChangeDirectory(path);
				}
			}

			ImGui::PopStyleColor(pushed_color_count);

			if (ImGui::BeginPopupContextItem()) {
				if (entry.is_regular_file() and path.has_extension()) {
					auto ext = path.extension();
					if (ext == ".escn" or
						ext == ".prefab" or
						ext == ".enik" or
						ext == ".txt" ) {
						if (ImGui::MenuItem("Open File")) {
							m_TextEditorPanel->OpenTextFile(path);
							ImGui::SetWindowFocus("Text Editor");
						}
					}
				}

				ImGui::EndPopup();
			}

			if (entry.is_regular_file()) {
				if (ImGui::BeginDragDropSource()) {
					// Set payload to carry the index of our item (could be anything)
					ImGui::SetDragDropPayload("DND_FILE_PATH", Project::GetRelativePath(path).string().c_str(), path.string().length() + 1);

					// Display preview (could be anything, e.g. when dragging an image we could decide to display
					// the filename and a small preview of the image, etc.)
					ImGui::Text(filename.c_str());
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

	m_FileWatcher = CreateScope<filewatch::FileWatch<std::string>>(m_CurrentDirectory.string(),
		[&](const std::string& path, const filewatch::Event change_type) {
			m_HasSearched = false;
		}
	);
}

}
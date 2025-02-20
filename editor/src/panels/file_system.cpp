#include "file_system.h"

#include <filesystem>
#include <imgui/imgui.h>
#include <pch.h>

#include "project/project.h"
#include "utils/utils.h"
#include "dialogs/dialog_confirm.h"
#include "utils/imgui_utils.h"
#include "audio/audio.h"
#include "utils/editor_colors.h"
#include "editor_layer.h"
#include "tabs/scene_editor_tab.h"


namespace Enik {

void FileSystemPanel::SetContext(EditorLayer* editor, SceneEditorTab* tab) {
	m_EditorLayer = editor;
	m_EditorTab = tab;
	if (m_CurrentDirectory.empty()) {
		ChangeDirectory(Project::GetProjectDirectory());
	}
}

void FileSystemPanel::RenderContent() {
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

	ShowDirectoriesTable();
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


	if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
		ImGui::OpenPopup("pop_directories_table");
	}
	if (ImGui::BeginPopup("pop_directories_table")) {
		if (ImGui::MenuItem("Create File")) {
			std::ofstream(m_CurrentDirectory / "new_file.escn").close();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginTable("Directory", 1)) {
		for (auto& entry : m_Entries) {
			auto& path = entry.path();
			std::string filename = path.filename().string();

			ImGui::PushID(&entry);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (entry.is_directory()) {
				filename = filename + "/";
			}

			int pushed_color_count = 0;
			ImGuiUtils::ColorFile(path, pushed_color_count);

			if (ImGui::Selectable(filename.c_str())) {
				if (entry.is_directory()) {
					ChangeDirectory(path);
				}
				else if (entry.is_regular_file() and path.has_extension()) {
					auto ext = path.extension();
					if (ext == ".anim") {
						if (m_EditorTab) {
							m_EditorTab->m_AnimationEditorPanel.SetAnimation(Project::GetAssetManagerEditor()->ImportAsset(path));
						}
					}
				}
			}

			ImGui::PopStyleColor(pushed_color_count);

			static std::string rename_file_name;

			if (ImGui::IsItemHovered() and ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup("fscm");
			}
			if (ImGui::BeginPopup("fscm")) {
				if (entry.is_regular_file() and path.has_extension()) {
					auto ext = path.extension();
					if (ext == ".escn" or
						ext == ".prefab" or
						ext == ".enik" or
						ext == ".png" or
						ext == ".wav" or
						ext == ".anim" or
						ext == ".txt" ) {
						if (ImGui::MenuItem("Open File")) {
							if (m_EditorLayer) {
								m_EditorLayer->RequestOpenAsset(Project::GetRelativePath(path));
							}
						}
					}
				}

				if (ImGui::MenuItem("Delete")) {
					DialogConfirm::OpenDialog("Delete ?", path.string(),
						[&](){
							if (std::filesystem::exists(path)) {
								// ! std::filesystem::remove(path);
								// ? move to tmp instead of removing
								try {
									std::filesystem::copy(
										path,
										std::filesystem::temp_directory_path() / path.filename(),
										std::filesystem::copy_options::overwrite_existing |
										std::filesystem::copy_options::recursive
									);
									std::filesystem::remove_all(path);
								}
								catch (const std::filesystem::filesystem_error& ex) {
									EN_CORE_ERROR("Error removing: {}", ex.what());
								}
							}
						}
					);
				}

				ImGui::EndPopup();
			}

			if (entry.is_regular_file()) {
				if (ImGui::BeginDragDropSource()) {
					// Set payload to carry the index of our item (could be anything)
					ImGui::SetDragDropPayload("DND_FILE_PATH", Project::GetRelativePath(path).string().c_str(), path.string().length() + 1);

					// Display preview (could be anything, e.g. when dragging an image we could decide to display
					// the filename and a small preview of the image, etc.)
					ImGui::Text("%s", filename.c_str());
					ImGui::EndDragDropSource();
				}
			}
			ImGui::PopID();
		}
		ImGui::Dummy(ImVec2(0,30));
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
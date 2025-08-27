#include <pch.h>
#include "dialog_file.h"

#include <base.h>
#include <imgui/imgui.h>
#include "utils/utils.h"
#include "utils/imgui_utils.h"

namespace Enik {

static DialogFileData s_Data;

void DialogFile::OpenDialog(DialogType type, const std::function<void()>& call_when_confirmed, const std::string& ext) {
	s_Data.type = type;
	s_Data.ext = ext;
	s_Data.is_open = true;
	s_Data.call_function = call_when_confirmed;
	s_Data.has_searched = false;
}

const std::filesystem::path& DialogFile::GetSelectedPath() {
	return s_Data.selected_path;
}

void DialogFile::SetCurrentDir(const std::filesystem::path& directory) {
	s_Data.current_directory = std::filesystem::canonical(directory);
}

DialogFileResult DialogFile::Show() {
	if (not s_Data.is_open) {
		return DialogFileResult::NONE;
	}

	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
	std::string widnow_name;

	if (s_Data.type == DialogType::OPEN_FILE) {
		widnow_name = ("Open File (" + s_Data.ext + ")");
	} else if (s_Data.type == DialogType::SAVE_FILE) {
		widnow_name = ("Save File (" + s_Data.ext + ")");
	} else if (s_Data.type == DialogType::SELECT_DIR) {
		widnow_name = "Select An Empty Directory";
	}

	ImGui::OpenPopup(widnow_name.c_str());
	if (ImGui::BeginPopupModal(widnow_name.c_str(), &s_Data.is_open)) {
		DialogFileResult result;
		try {
			result = ShowPopup();
		} catch (const std::filesystem::filesystem_error& ex) {
			s_Data.current_directory = Project::GetProjectDirectory();
		}
		ImGui::EndPopup();
		return result;
	}

	return DialogFileResult::NONE;
}

const char* DialogTypeToString(DialogType t) {
	switch (t) {
		case OPEN_FILE:  return "Open";
		case SAVE_FILE:  return "Save";
		case SELECT_DIR: return "Select";
	}
	return NULL;
}

DialogFileResult DialogFile::ShowPopup() {
	if (ImGui::Button(" ^ ")) {
		if (!s_Data.current_directory.empty()) {
			s_Data.current_directory = s_Data.current_directory.parent_path();
			s_Data.has_searched = false;
		}
	}

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", s_Data.current_directory.string().c_str());

	ImGui::SameLine();

	static bool create_dir_open = true;
	if (ImGui::Button(" + ")) {
		ImGui::OpenPopup("Create Directory");
		create_dir_open = true;
	}
	ImGui::SetNextWindowSize(ImVec2(150, 100), ImGuiCond_Appearing);
	if (ImGui::BeginPopupModal("Create Directory", &create_dir_open)) {
		static char dirname[128] = "";
		ImGui::InputText("##cdname", dirname, IM_ARRAYSIZE(dirname));

		if (ImGui::Button("Cancel")) {
			dirname[0] = '\0';
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Create")) {
			std::filesystem::path new_dir = s_Data.current_directory / dirname;
			if (!std::filesystem::exists(new_dir)) {
				std::filesystem::create_directory(new_dir);
			}
			s_Data.has_searched = false;
			dirname[0] = '\0'; // reset input
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// InputFieldText
	static char file_path_buffer[256] = "";

	if (not s_Data.has_searched) {
		// Search directory and sort entries
		s_Data.entries.clear();

		for (const auto& entry : fs::directory_iterator(s_Data.current_directory)) {
			s_Data.entries.push_back(entry);
		}

		Utils::SortDirectoryEntries(s_Data.entries);

		if (not s_Data.ext.empty()) {
			Utils::FilterFiles(s_Data.entries, { s_Data.ext }, s_Data.type == DialogType::OPEN_FILE);
		}

		s_Data.has_searched = true;
		strcpy(file_path_buffer, s_Data.current_directory.string().c_str());
		s_Data.selected_path.clear();
	}

	ShowDirectoriesTable(file_path_buffer);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	/* Show Input Field */ {
		const float buttonsWidth = ImGui::CalcTextSize(((std::string)(s_Data.ext + "CancelOpen")).c_str()).x + ImGui::GetStyle().ItemSpacing.x * 4.0f;
		float inputTextWidth = ImGui::GetContentRegionAvail().x - buttonsWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::PushItemWidth(inputTextWidth);
		ImGui::BeginDisabled(s_Data.type == DialogType::OPEN_FILE);

		ImGuiInputTextFlags flag = (s_Data.type == DialogType::OPEN_FILE) ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None;
		ImGui::InputText("##File Path", file_path_buffer, sizeof(file_path_buffer), flag);

		ImGui::EndDisabled();
		ImGui::PopItemWidth();
	}

	if (s_Data.type == DialogType::SAVE_FILE || s_Data.type == DialogType::SELECT_DIR) {
		s_Data.selected_path = file_path_buffer;
	}

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.5f), s_Data.ext.c_str());

	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		s_Data.has_searched = false;

		strcpy(file_path_buffer, "");
		s_Data.is_open = false;
		return DialogFileResult::CANCEL;
	}

	// TODO: currently we are not checking the save file extension

	bool is_valid = IsValidSelection();
	ImGui::BeginDisabled(s_Data.selected_path.empty() || (not is_valid));

	ImGui::SameLine();
	if (ImGui::Button(DialogTypeToString(s_Data.type))) {
		s_Data.has_searched = false;

		if (s_Data.type == DialogType::SAVE_FILE || s_Data.type == DialogType::SELECT_DIR) {
			s_Data.selected_path = file_path_buffer;
		}

		strcpy(file_path_buffer, "");

		s_Data.is_open = false;

		if (s_Data.selected_path.has_extension()) {
			if (s_Data.selected_path.extension() != s_Data.ext and not s_Data.ext.empty()) {
				s_Data.selected_path = (s_Data.selected_path.parent_path() / s_Data.selected_path.stem()).string() + s_Data.ext;
			}
		} else {
			s_Data.selected_path += s_Data.ext;
		}

		EN_CORE_TRACE("Dialog File: selected path {}", s_Data.selected_path.string());

		ImGui::EndDisabled();

		if (s_Data.type == DialogType::SAVE_FILE) {
			s_Data.call_function();
			return DialogFileResult::ACCEPT_SAVE;
		} else if (s_Data.type == DialogType::SELECT_DIR) {
			s_Data.call_function();
			return DialogFileResult::SELECTED_DIR;
		} else {
			s_Data.call_function();
			return DialogFileResult::ACCEPT_OPEN;
		}
	}
	ImGui::EndDisabled();

	return DialogFileResult::NONE;
}

void DialogFile::ShowDirectoriesTable(char* file_path_buffer) {
	// Calculate available content height
	float availableHeight = ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y * 5.0f - ImGui::GetTextLineHeightWithSpacing();
	ImGui::BeginChild("ScrollableTable", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

	if (ImGui::BeginTable("Directory", 1)) {
		for (const auto& entry : s_Data.entries) {
			const auto& path = entry.path();
			std::string filename = path.filename().string();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (entry.is_directory()) {
				filename = filename + "/";
			}

			int pushed_color_count = 0;
			ImGuiUtils::ColorFile(path, pushed_color_count);

			if (ImGui::Selectable(filename.c_str())) {
				if (fs::is_directory(path)) {
					s_Data.current_directory = path;
					strcpy(file_path_buffer, "");
					s_Data.has_searched = false;
				}
				else {
					s_Data.selected_path = path.string();
					strcpy(file_path_buffer, s_Data.selected_path.string().c_str());
				}
			}

			ImGui::PopStyleColor(pushed_color_count);
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
}

bool DialogFile::IsValidSelection() {
	fs::path path = s_Data.selected_path;
	if (fs::exists(path.parent_path()) && fs::is_directory(path.parent_path())) {
		if (fs::is_directory(path)) {
			if (s_Data.type == DialogType::SELECT_DIR && fs::is_empty(path)) {
				return true;
			}
			return false;
		}

		if ((fs::status(path.parent_path()).permissions() & fs::perms::owner_write) != fs::perms::none) {
			if (s_Data.ext.empty()) {
				// extension not required
				return true;
			}
			if (path.has_extension()) {
				// has the correct extension
				if (path.extension() == s_Data.ext) {
					return true;
				}
			}
			else {
				// has no extention, we will append
				return true;
			}
		}
	}

	return false;
}

}
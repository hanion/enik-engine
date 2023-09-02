#include <pch.h>
#include "dialog_file.h"

#include <base.h>
#include <imgui/imgui.h>
#include "../utils/sort.h"

namespace Enik {

static DialogFileData s_Data;

void DialogFile::OpenDialog(DialogType type, const std::function<void()>& call_when_confirmed, const std::string& ext) {
	s_Data.type = type;
	s_Data.ext = ext;
	s_Data.is_open = true;
	s_Data.call_function = call_when_confirmed;
}

const std::filesystem::path& DialogFile::GetSelectedPath() {
	return s_Data.selected_path;
}

DialogFileResult DialogFile::Show() {
	if (not s_Data.is_open) {
		return DialogFileResult::NONE;
	}

	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
	std::string widnow_name;

	if (s_Data.type == DialogType::OPEN_FILE) {
		widnow_name = ("Open File (" + s_Data.ext + ")");
	}
	else if (s_Data.type == DialogType::SAVE_FILE) {
		widnow_name = ("Save File (" + s_Data.ext + ")");
	}

	ImGui::OpenPopup(widnow_name.c_str());
	if (ImGui::BeginPopupModal(widnow_name.c_str(), &s_Data.is_open)) {
		DialogFileResult result = ShowPopup();
		ImGui::EndPopup();
		return result;
	}

	return DialogFileResult::NONE;
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

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// InputFieldText
	static char file_path_buffer[256] = "";

	if (not s_Data.has_searched) {
		// Search directory and sort entries
		s_Data.entries.clear();

		for (const auto& entry : fs::directory_iterator(s_Data.current_directory)) {
			if (entry.is_regular_file() && entry.path().extension() != s_Data.ext) {
				continue;
			}
			s_Data.entries.push_back(entry);
		}

		Utils::SortDirectoryEntries(s_Data.entries);

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

	if (s_Data.type == DialogType::SAVE_FILE) {
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

	bool is_valid = isValidSelection();
	ImGui::BeginDisabled(s_Data.selected_path.empty() || (not is_valid));

	ImGui::SameLine();
	if (ImGui::Button((s_Data.type == DialogType::OPEN_FILE) ? "Open" : "Save")) {
		s_Data.has_searched = false;

		if (s_Data.type == DialogType::SAVE_FILE) {
			s_Data.selected_path = file_path_buffer;
		}

		strcpy(file_path_buffer, "");

		s_Data.is_open = false;

		EN_CORE_TRACE("Dialog File: selected path '{0}'", s_Data.selected_path);

		ImGui::EndDisabled();

		if (s_Data.type == DialogType::SAVE_FILE) {
			s_Data.call_function();
			return DialogFileResult::ACCEPT_SAVE;
		}
		else {
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
			std::string fileName = path.filename().string();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (entry.is_directory()) {
				fileName = fileName + "/";
			}

			if (ImGui::Selectable(fileName.c_str())) {
				if (fs::is_directory(path)) {
					s_Data.current_directory = path;
					strcpy(file_path_buffer, "");  // Clear the file path buffer when changing directory
					s_Data.has_searched = false;
				}
				else {
					s_Data.selected_path = path.string();
					strcpy(file_path_buffer, s_Data.selected_path.c_str());
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
}

bool DialogFile::isValidSelection() {
	fs::path path = s_Data.selected_path;
	if (fs::exists(path.parent_path()) && fs::is_directory(path.parent_path())) {
		// selection can not be a directory, it needs to be a file path
		if (fs::is_directory(path)) {
			return false;
		}

		if ((fs::status(path.parent_path()).permissions() & fs::perms::owner_write) != fs::perms::none) {
			return true;
		}
	}

	return false;
}

}
#include <pch.h>
#include "dialog_file.h"

#include <base.h>
#include <imgui/imgui.h>

namespace Enik {

static DialogFileData s_Data;

const std::filesystem::path& DialogFile::GetSelectedPath() {
	return s_Data.selected_path;
}

DialogResult DialogFile::Show(bool& is_open, DialogType type, const std::string& ext) {
	if (not is_open) {
		return DialogResult::NONE;
	}

	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
	std::string widnow_name;

	if (type == DialogType::OPEN_FILE) {
		widnow_name = ("Open File (" + ext + ")");
	}
	else if (type == DialogType::SAVE_FILE) {
		widnow_name = ("Save File (" + ext + ")");
	}

	ImGui::OpenPopup(widnow_name.c_str());
	if (ImGui::BeginPopupModal(widnow_name.c_str(), &is_open)) {
		DialogResult result = ShowPopup(is_open, type, ext);
		ImGui::EndPopup();
		return result;
	}

	return DialogResult::NONE;
}

DialogResult DialogFile::ShowPopup(bool& is_open, DialogType type, const std::string& ext) {
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
			if (entry.is_regular_file() && entry.path().extension() != ext) {
				continue;
			}
			s_Data.entries.push_back(entry);
		}
		std::sort(s_Data.entries.begin(), s_Data.entries.end(),
				  [](const fs::directory_entry& a, const fs::directory_entry& b) {
					  return a.path().filename().string() < b.path().filename().string();
				  });

		s_Data.has_searched = true;
		strcpy(file_path_buffer, s_Data.current_directory.string().c_str());
		s_Data.selected_path.clear();
	}

	ShowDirectoriesTable(file_path_buffer);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	/* Show Input Field */ {
		const float buttonsWidth = ImGui::CalcTextSize(((std::string)(ext + "CancelOpen")).c_str()).x + ImGui::GetStyle().ItemSpacing.x * 4.0f;
		float inputTextWidth = ImGui::GetContentRegionAvail().x - buttonsWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::PushItemWidth(inputTextWidth);
		ImGui::BeginDisabled(type == DialogType::OPEN_FILE);

		ImGuiInputTextFlags flag = (type == DialogType::OPEN_FILE) ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None;
		ImGui::InputText("##File Path", file_path_buffer, sizeof(file_path_buffer), flag);

		ImGui::EndDisabled();
		ImGui::PopItemWidth();
	}

	if (type == DialogType::SAVE_FILE) {
		s_Data.selected_path = file_path_buffer;
	}

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.5f), ext.c_str());

	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		s_Data.has_searched = false;

		strcpy(file_path_buffer, "");
		is_open = false;
		return DialogResult::CANCEL;
	}

	// TODO: currently we are not checking the save file extension

	bool is_valid = isValidSelection();
	ImGui::BeginDisabled(s_Data.selected_path.empty() || (not is_valid));

	ImGui::SameLine();
	if (ImGui::Button((type == DialogType::OPEN_FILE) ? "Open" : "Save")) {
		s_Data.has_searched = false;

		if (type == DialogType::SAVE_FILE) {
			s_Data.selected_path = file_path_buffer;
		}

		strcpy(file_path_buffer, "");

		is_open = false;

		EN_CORE_TRACE("Dialog File: selected path '{0}'", s_Data.selected_path);

		ImGui::EndDisabled();
		return DialogResult::ACCEPT;
	}
	ImGui::EndDisabled();

	return DialogResult::NONE;
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
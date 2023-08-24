#include <pch.h>
#include "dialog_file.h"
#include <imgui/imgui.h>
#include <base.h>


namespace Enik {

static DialogFileData s_Data;

const std::string& DialogFile::GetSelectedPath() {
	return s_Data.selectedPath;
}


DialogResult DialogFile::Show(bool& isOpen, DialogType type, const std::string& ext) {
	if (not isOpen) { 
		return DialogResult::NONE; 
	}
	

	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
	std::string widnowName;

	if (type == DialogType::OPEN_FILE) {
		widnowName = ("Open File ("+ext+")");
	}
	else if (type == DialogType::SAVE_FILE) {
		widnowName = ("Save File ("+ext+")");
	}

	ImGui::OpenPopup(widnowName.c_str());
	if (ImGui::BeginPopupModal(widnowName.c_str(), &isOpen)) {
		DialogResult result = ShowPopup(isOpen, type, ext);
		ImGui::EndPopup();
		return result;
	}

	return DialogResult::NONE;
}

DialogResult DialogFile::ShowPopup(bool& isOpen, DialogType type, const std::string& ext) {
	if (ImGui::Button(" ^ ")) {
		if (!s_Data.currentDirectory.empty()) {
			s_Data.currentDirectory = s_Data.currentDirectory.parent_path();
			s_Data.hasSearched = false;
		}
	}

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", s_Data.currentDirectory.string().c_str());
	

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();


	// InputFieldText
	static char filePathBuffer[256] = "";


	if (not s_Data.hasSearched) {
		// Search directory and sort entries
		s_Data.entries.clear();

		for (const auto& entry : fs::directory_iterator(s_Data.currentDirectory)) {
			if (entry.is_regular_file() && entry.path().extension() != ext) {
				continue;
			}
			s_Data.entries.push_back(entry);
		}
		std::sort(s_Data.entries.begin(), s_Data.entries.end(), 
			[](const fs::directory_entry& a, const fs::directory_entry& b) {
				return a.path().filename().string() < b.path().filename().string();
		});
	
		s_Data.hasSearched = true;
		strcpy(filePathBuffer, s_Data.currentDirectory.string().c_str());
		s_Data.selectedPath.clear();
	}



	ShowDirectoriesTable(filePathBuffer);


	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();




	/* Show Input Field */ {
		const float buttonsWidth = ImGui::CalcTextSize(((std::string)(ext + "CancelOpen")).c_str()).x + ImGui::GetStyle().ItemSpacing.x * 4.0f;
		float inputTextWidth = ImGui::GetContentRegionAvail().x - buttonsWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::PushItemWidth(inputTextWidth);
		ImGui::BeginDisabled(type == DialogType::OPEN_FILE);

		ImGuiInputTextFlags flag = (type == DialogType::OPEN_FILE) ? ImGuiInputTextFlags_ReadOnly : ImGuiInputTextFlags_None;
		ImGui::InputText("##File Path", filePathBuffer, sizeof(filePathBuffer), flag);

		ImGui::EndDisabled();
		ImGui::PopItemWidth();
	}


	if (type == DialogType::SAVE_FILE) {
		s_Data.selectedPath = filePathBuffer;
	}


	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 0.5f), ext.c_str());
	
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		s_Data.hasSearched = false;
	
		strcpy(filePathBuffer, "");
		isOpen = false;
		return DialogResult::CANCEL;
	}

	// TODO: currently we are not checking the save file extension

	bool is_valid = isValidSelection();
	ImGui::BeginDisabled(s_Data.selectedPath.empty() || (not is_valid));

	ImGui::SameLine();
	if (ImGui::Button((type == DialogType::OPEN_FILE) ? "Open" : "Save")) {
		s_Data.hasSearched = false;

		if (type == DialogType::SAVE_FILE) {
			s_Data.selectedPath = filePathBuffer;
		}

		strcpy(filePathBuffer, "");
		
		isOpen = false;
		
		EN_CORE_TRACE("Dialog File: selected path '{0}'", s_Data.selectedPath);
		
		ImGui::EndDisabled();
		return DialogResult::ACCEPT;
	}
	ImGui::EndDisabled();

	return DialogResult::NONE;
}

void DialogFile::ShowDirectoriesTable(char* filePathBuffer) {
	// Calculate available content height
	float availableHeight = ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y*5.0f - ImGui::GetTextLineHeightWithSpacing();
	ImGui::BeginChild("ScrollableTable", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

	if (ImGui::BeginTable("Directory", 1)) {
		for (const auto& entry : s_Data.entries) {
			const auto& path = entry.path();
			const std::string fileName = path.filename().string();

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (ImGui::Selectable(fileName.c_str())) {
				if (fs::is_directory(path)) {
					s_Data.currentDirectory = path;
					strcpy(filePathBuffer, ""); // Clear the file path buffer when changing directory
					s_Data.hasSearched = false;
				}
				else {
					s_Data.selectedPath = path.string();
					strcpy(filePathBuffer, s_Data.selectedPath.c_str());
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
}


bool DialogFile::isValidSelection() {
	fs::path path = s_Data.selectedPath;
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
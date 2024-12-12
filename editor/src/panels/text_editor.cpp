#include "text_editor.h"

#include <imgui/imgui.h>
#include <pch.h>

#include "dialogs/dialog_confirm.h"
#include "dialogs/dialog_file.h"
#include "utils/editor_colors.h"
#include "project/project.h"

namespace Enik {

void TextEditorPanel::RenderContent() {
	m_WindowFocused = ImGui::IsWindowFocused();

	if (ImGui::BeginMenuBar()) {
		if (IsEditing()) {
			bool menu_open = ImGui::BeginMenu(m_CurrentFile.filename().string().c_str());
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip(Project::GetRelativePath(m_CurrentFile).string().c_str());
			}

			if (menu_open) {
				if (ImGui::MenuItem("Save File")) {
					DialogConfirm::OpenDialog(
						"Save File ?",
						CurrentFilePath(),
						[&]() {
							SaveCurrentTextFile();
							OpenTextFile(m_CurrentFile);
						}
					);
				}
				if (ImGui::MenuItem("Open File")) {
					DialogFile::OpenDialog(DialogType::OPEN_FILE,
						[&]() {
							OpenTextFile(DialogFile::GetSelectedPath());
						}
					);
				}
				ImGui::EndMenu();
			}
		} else {
			if (ImGui::MenuItem("Open File")) {
				DialogFile::OpenDialog(DialogType::OPEN_FILE,
					[&]() {
						OpenTextFile(DialogFile::GetSelectedPath());
					}
				);
			}
		}

		ImGui::EndMenuBar();
	}



	ImGuiIO& io = ImGui::GetIO();
	ImGui::PushFont(io.Fonts->Fonts.back());
	ImGui::PushStyleColor(ImGuiCol_FrameBg, EditorColors::te_bg);
	ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, EditorColors::te_bg);

	ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
	ImGui::InputTextMultiline("##text_editor", m_Buffer.data(), m_Buffer.size(), ImVec2(-1.0f, -1.0f), flags);

	ImGui::PopStyleColor(2);
	ImGui::PopFont();


	/* Drag drop target */ {
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
				std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
				path = Project::GetAbsolutePath(path);
				if (std::filesystem::exists(path) and std::filesystem::is_regular_file(path)) {
					OpenTextFile(path);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
}



bool TextEditorPanel::OpenTextFile(const std::filesystem::path& path) {
	if (path.empty() or not std::filesystem::exists(Project::GetAbsolutePath(path))) {
		return false;
	}

	m_CurrentFile = Project::GetAbsolutePath(path);

	const char* filename = m_CurrentFile.string().c_str();

	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	// resize buffer
	const std::streamsize exta_space = 2048;
	m_Buffer.resize(size + exta_space + 1);

	file.read(m_Buffer.data(), size);
	m_Buffer[size] = '\0';

	file.close();

	return true;
}

bool TextEditorPanel::SaveCurrentTextFile() {
	const char* filename = m_CurrentFile.string().c_str();

	std::ofstream file = std::ofstream(filename);
	if (not file.is_open()) {
		return false;
	}

	file << m_Buffer.data();
	file.close();

	return true;
}

void TextEditorPanel::AskToSaveFile() {
	if (m_WindowFocused and IsEditing()) {
		DialogConfirm::OpenDialog(
			"Save Text File ?",
			Project::GetRelativePath(CurrentFilePath()).string(),
			[&]() {
				SaveCurrentTextFile();
			}
		);
	}
}

}
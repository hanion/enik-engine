#include "home_tab.h"
#include "asset/importer/texture_importer.h"
#include "dialogs/dialog_file.h"
#include "imgui.h"
#include "project/project.h"
#include "editor_layer.h"
#include "scene/scene_serializer.h"
#include "utils/editor_assets.h"
#include "utils/editor_colors.h"

namespace Enik {

HomeTab::HomeTab() : EditorTab("Home") {
	m_NoTabBar = true;
}

void HomeTab::RenderContent() {
	ImGui::PushStyleColor(ImGuiCol_WindowBg, EditorColors::bg_dark);
	ImGui::Begin("Home");
	ImGui::PopStyleColor();

	ImGuiIO& io = ImGui::GetIO();
	ImGui::PushFont(io.Fonts->Fonts.back());

	ImVec2 window_size = ImGui::GetWindowSize();

	ImVec2 button_size = ImVec2(200.0f, ImGui::GetFontSize() * 1.5f);
	float button_spacing = ImGui::GetStyle().ItemSpacing.y;
	float start_x = (window_size.x - button_size.x) / 2.0f;

	ImVec2 banner_size = ImVec2(EditorAssets::Banner->GetWidth()*0.6f, EditorAssets::Banner->GetHeight()*0.6f);
	ImGui::SetCursorPos(ImVec2((window_size.x - banner_size.x) * 0.5f, (window_size.y*0.5f-banner_size.y)));
	ImGui::Image(
		reinterpret_cast<void*>(static_cast<uintptr_t>(EditorAssets::Banner->GetRendererID())),
		banner_size,
		ImVec2(0, 1), ImVec2(1, 0)
	);

	ImGui::SetCursorPos(ImVec2(start_x, ImGui::GetCursorPosY()));
	if (ImGui::Button("Open", button_size)) {
		ImGui::OpenPopup("OpenAsset");

	}
	ImGui::SetCursorPos(ImVec2(start_x, ImGui::GetCursorPosY() + button_spacing));
	if (ImGui::Button("New", button_size)) {
		ImGui::OpenPopup("NewAsset");
	}

	ImGui::SetCursorPos(ImVec2(start_x, ImGui::GetCursorPosY() + button_spacing));
	if (ImGui::Button("Exit", button_size)) {
		m_EditorLayer->ExitEditor();
	}

// 	float footer_height = ImGui::GetFontSize() * 1.5f;
// 	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), window_size.y - footer_height));
// 	ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::dim);
// 	ImGui::Text("enik-engine v0.1.9");
// 	ImGui::PopStyleColor();

	if(ImGui::BeginPopup("OpenAsset")) {
		if (Project::GetActive()) {
			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::scene);
			if (ImGui::MenuItem("Scene")) {
				DialogFile::OpenDialog(DialogType::OPEN_FILE,
					[this](){
						m_EditorLayer->RequestOpenAsset(Project::GetRelativePath(DialogFile::GetSelectedPath()));
						m_IsOpen = false;
					}
					, ".escn"
				);
			}
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::prefab);
			if (ImGui::MenuItem("Prefab")) {
				DialogFile::OpenDialog(DialogType::OPEN_FILE,
					[this](){
						m_EditorLayer->RequestOpenAsset(Project::GetRelativePath(DialogFile::GetSelectedPath()));
						m_IsOpen = false;
					}
					, ".prefab"
				);
			}
			ImGui::PopStyleColor();
		}
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::project);
		if (ImGui::MenuItem("Project")) {
			DialogFile::OpenDialog(DialogType::OPEN_FILE,
				[this](){
					m_EditorLayer->LoadProject(DialogFile::GetSelectedPath());
					m_IsOpen = false;
				}
				, ".enik"
			);
			
		}
		ImGui::PopStyleColor();
		ImGui::EndPopup();
	} else if (ImGui::BeginPopup("NewAsset")) {
		if (Project::GetActive()) {
			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::scene);
			if (ImGui::MenuItem("Scene")) {
				static const auto save_file_func = [this]() {
					Ref<Scene> new_scene = CreateRef<Scene>();
					SceneSerializer serializer = SceneSerializer(new_scene);
					serializer.Serialize(DialogFile::GetSelectedPath().string());
					m_EditorLayer->RequestOpenAsset(Project::GetRelativePath(DialogFile::GetSelectedPath()));
					m_IsOpen = false;
				};

				DialogFile::OpenDialog(DialogType::SAVE_FILE,
					[](){
						std::filesystem::path new_path = DialogFile::GetSelectedPath();
						std::filesystem::path relative = Project::GetRelativePath(new_path);

						if (std::filesystem::exists(new_path)) {
							DialogConfirm::OpenDialog(
								"Override ?",
								relative.string() + " already exists.\nDo you want to override?",
								save_file_func
							);
						}
						else {
						   save_file_func();
						}

					}
				, ".escn"
				);
			}
			ImGui::PopStyleColor();
		}

		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::project);
		if (ImGui::MenuItem("Project")) {
			// TODO: implement new project creation
		}
		ImGui::PopStyleColor();

		ImGui::EndPopup();
	}
	ImGui::PopFont();
	ImGui::End();
}

void HomeTab::InitializeDockspace() {
	ImGui::DockBuilderDockWindow("Home", m_DockspaceID);
}

}

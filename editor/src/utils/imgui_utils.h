#pragma once
#include <base.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "file_metadata.h"
#include "utils/editor_colors.h"
#include "asset/asset_manager.h"
#include "dialogs/dialog_file.h"
#include "project/project.h"

namespace Enik {
namespace ImGuiUtils {

void PrefixLabel(std::string_view title);

// NOTE: call `ImGui::PopStyleColor(pushed_color_count);` after calling this !
void ColorFile(const std::filesystem::path& path, int& pushed_color_count);



template <typename T>
bool AssetButton(AssetHandle& handle) {
	bool pressed = false;

	std::string label;
	std::string tooltip = "Select Asset";

	bool has_valid_asset = false;
	if (handle && AssetManager::IsAssetHandleValid(handle)) {
		has_valid_asset = true;
		const AssetMetadata& metadata = Project::GetAssetManagerEditor()->GetMetadata(handle);
		label = metadata.FilePath.filename().string();
		tooltip = Project::GetRelativePath(metadata.FilePath).string();
	} else {
		label = FileMetadata::GetPlaceholder<T>();
	}

	FileMetadata::ColorFileText<T>();

	if (ImGui::Button(label.c_str())) {
		if (has_valid_asset) {
			pressed = true;
		} else {
			DialogFile::OpenDialog(
				DialogType::OPEN_FILE, [&handle]() {
					handle = Project::GetAssetManagerEditor()->ImportAsset(DialogFile::GetSelectedPath());
				}
				, FileMetadata::GetFileExtension<T>()
			);
		}
	}

	if (ImGui::BeginDragDropSource()) {
		const AssetMetadata& metadata = Project::GetAssetManagerEditor()->GetMetadata(handle);
		ImGui::SetDragDropPayload("DND_FILE_PATH", metadata.FilePath.string().c_str(), metadata.FilePath.string().length() + 1);
		ImGui::Text("%s", metadata.FilePath.filename().string().c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::IsItemHovered()){
		ImGui::SetTooltip(tooltip.c_str());
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup("AssetRC");
		}
	}

	ImGui::PopStyleColor();

	if (ImGui::BeginPopup("AssetRC")) {
		if (ImGui::MenuItem("Select Asset")) {
			DialogFile::OpenDialog(
				DialogType::OPEN_FILE, [&handle]() {
					handle = Project::GetAssetManagerEditor()->ImportAsset(DialogFile::GetSelectedPath());
				}
				, FileMetadata::GetFileExtension<T>()
			);
		}
		if (handle) {
			if (ImGui::MenuItem("Remove Asset")) {
				handle = 0;
			}
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
			std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
			if (path.has_extension() && path.extension() == FileMetadata::GetFileExtension<T>()) {
				handle = Project::GetAssetManagerEditor()->ImportAsset(Project::GetAbsolutePath(path));
			}
		}
		ImGui::EndDragDropTarget();
	}

	return pressed;
}

}
}

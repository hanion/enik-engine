#pragma once
#include <base.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "../utils/editor_colors.h"

namespace Enik {
namespace ImGuiUtils {

inline void PrefixLabel(std::string_view title) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const ImGuiStyle& style = ImGui::GetStyle();
	float full_width = ImGui::GetContentRegionAvail().x;
	float item_width = ImGui::CalcItemWidth() + style.ItemSpacing.x;
	ImVec2 text_size = ImGui::CalcTextSize(title.data());
	ImRect text_rect;
	text_rect.Min = ImGui::GetCursorScreenPos();
	text_rect.Max = text_rect.Min;
	text_rect.Max.x += full_width - item_width;
	text_rect.Max.y += text_size.y;

	ImGui::SetCursorScreenPos(text_rect.Min);

	ImGui::AlignTextToFramePadding();
	// Adjust text rect manually because we render it directly into a drawlist instead of using public functions.
	text_rect.Min.y += window->DC.CurrLineTextBaseOffset;
	text_rect.Max.y += window->DC.CurrLineTextBaseOffset + 1; // +1 so there is no letters bottom clip

	ImGui::ItemSize(text_rect);
	if (ImGui::ItemAdd(text_rect, window->GetID(title.data(), title.data() + title.size()))) {
		ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), text_rect.Min, text_rect.Max, text_rect.Max.x,
			text_rect.Max.x, title.data(), title.data() + title.size(), &text_size);

		if (text_rect.GetWidth() < text_size.x && ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%.*s", (int)title.size(), title.data());
		}
	}

	ImGui::SetCursorScreenPos(ImVec2(text_rect.Max.x, text_rect.Max.y - (text_size.y + window->DC.CurrLineTextBaseOffset)));
	ImGui::SameLine();
}

// call `ImGui::PopStyleColor(pushed_color_count);` after calling this !
inline void ColorFile(const std::filesystem::path& path, int& pushed_color_count) {
	if (path.empty()) {
		return;
	}

	std::string extension = path.extension().string();
	if (extension.empty()) {
		return;
	}

	if (extension == ".prefab") {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::blue);
		pushed_color_count++;
	}
	else if (extension == ".escn") {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::green);
		pushed_color_count++;
	}
	else if (extension == ".png") {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::cyan);
		pushed_color_count++;
	}
	else if (extension == ".wav") {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::teal);
		pushed_color_count++;
	}
	else if (extension == ".enik") {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::yellow);
		pushed_color_count++;
	}
}


}
}

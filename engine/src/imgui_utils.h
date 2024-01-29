#pragma once
#include <base.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Enik {
namespace ImGuiUtils {

inline void PrefixLabel(std::string_view title) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const ImGuiStyle& style = ImGui::GetStyle();
	float fullWidth = ImGui::GetContentRegionAvail().x;
	float itemWidth = ImGui::CalcItemWidth() + style.ItemSpacing.x;
	ImVec2 textSize = ImGui::CalcTextSize(title.data());
	ImRect textRect;
	textRect.Min = ImGui::GetCursorScreenPos();
	textRect.Max = textRect.Min;
	textRect.Max.x += fullWidth - itemWidth;
	textRect.Max.y += textSize.y;

	ImGui::SetCursorScreenPos(textRect.Min);

	ImGui::AlignTextToFramePadding();
	// Adjust text rect manually because we render it directly into a drawlist instead of using public functions.
	textRect.Min.y += window->DC.CurrLineTextBaseOffset;
	textRect.Max.y += window->DC.CurrLineTextBaseOffset + 1; // +1 so there is no letters bottom clip

	ImGui::ItemSize(textRect);
	if (ImGui::ItemAdd(textRect, window->GetID(title.data(), title.data() + title.size()))) {
		ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), textRect.Min, textRect.Max, textRect.Max.x,
			textRect.Max.x, title.data(), title.data() + title.size(), &textSize);

		if (textRect.GetWidth() < textSize.x && ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%.*s", (int)title.size(), title.data());
		}
	}

	ImGui::SetCursorScreenPos(ImVec2(textRect.Max.x, textRect.Max.y - (textSize.y + window->DC.CurrLineTextBaseOffset)));
	ImGui::SameLine();

}

}
}

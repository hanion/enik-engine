#include "dialog_confirm.h"

#include <imgui/imgui.h>
#include <pch.h>

namespace Enik {

static DialogConfirmData s_Data;

void DialogConfirm::OpenDialog(const std::string& text, const std::function<void()>& call_when_confirmed) {
	s_Data.text = text;
	s_Data.is_open = true;
	s_Data.call_function = call_when_confirmed;
}

DialogConfirm::DialogConfirmResult DialogConfirm::Show() {
	if (not s_Data.is_open) {
		return DialogConfirm::DialogConfirmResult::CANCEL;
	}

	ImGui::OpenPopup(s_Data.text.c_str());

	if (ImGui::BeginPopupModal(s_Data.text.c_str(), &s_Data.is_open, ImGuiWindowFlags_AlwaysAutoResize)) {
		DialogConfirm::DialogConfirmResult result = ShowPopup();
		ImGui::EndPopup();
		return result;
	}

	return DialogConfirm::DialogConfirmResult::CANCEL;
}

DialogConfirm::DialogConfirmResult DialogConfirm::ShowPopup() {
	if (ImGui::Button("Cancel")) {
		s_Data.is_open = false;
		return DialogConfirm::DialogConfirmResult::CANCEL;
	}

	ImGui::SameLine();

	if (ImGui::Button("Confirm")) {
		s_Data.is_open = false;
		s_Data.call_function();
		return DialogConfirm::DialogConfirmResult::CONFIRM;
	}

	return DialogConfirm::DialogConfirmResult::CANCEL;
}

}
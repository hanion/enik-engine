#include "extern_functions.h"

#include "script_system/script_system.h"
#include "../imgui_utils.h"

namespace Enik {

Entity FindEntityByUUID(UUID uuid) {
	return ScriptSystem::GetSceneContext()->FindEntityByUUID(uuid);
}

Entity FindEntityByName(const std::string& name) {
    return ScriptSystem::GetSceneContext()->FindEntityByName(name);
}


void InspectorExportFloat(const std::string& text, float& value, float v_speed, float v_min, float v_max, const char* format) {
	ImGuiUtils::PrefixLabel(text);
	ImGui::DragFloat(("##"+text).c_str(), &value, v_speed, v_min, v_max, format);
}

void InspectorExportFloat2(const std::string& text, float v[2], float v_speed, float v_min, float v_max, const char* format) {
	ImGuiUtils::PrefixLabel(text);
	ImGui::DragFloat2(("##"+text).c_str(), v, v_speed, v_min, v_max, format);
}

void InspectorExportFloat3(const std::string& text, float v[3], float v_speed, float v_min, float v_max, const char* format) {
	ImGuiUtils::PrefixLabel(text);
	ImGui::DragFloat3(("##"+text).c_str(), v, v_speed, v_min, v_max, format);
}

}
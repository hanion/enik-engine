#include "inspector.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "scene/components.h"
#include <glm/gtc/type_ptr.hpp>


namespace Enik {

InspectorPanel::InspectorPanel(const Ref<Scene>& context) {
	SetContext(context);
}

void InspectorPanel::SetContext(const Ref<Scene>& context, SceneTreePanel* sceneTreePanel) {
	m_Context = context;
	m_SceneTreePanel = sceneTreePanel;
}

void InspectorPanel::OnImGuiRender() {
	if (!ImGui::Begin("Inspector")) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginTable("InspectorTable", 1)) {
		ImGui::EndTable();
		return;
	}

	if (m_SceneTreePanel->m_SelectionContext) {
		DrawEntityInInspector(m_SceneTreePanel->m_SelectionContext);
	}

	ImGui::EndTable();
	ImGui::End();
}


void InspectorPanel::DrawEntityInInspector(Entity entity) {
	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
	treeNodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;


	if (entity.Has<Component::Tag>()) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		auto& text =  entity.Get<Component::Tag>().Text;

		char buffer[256];
		memset(buffer, 0 ,sizeof(buffer));
		strcpy(buffer, text.c_str());
		if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
			text = std::string(buffer);
		}
	}




}


}
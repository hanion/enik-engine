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


	if (entity.Has<Component::Transform>()) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (ImGui::TreeNodeEx((void*)typeid(Component::Transform).hash_code(), treeNodeFlags, "Transform")) {
			auto& transform = entity.Get<Component::Transform>();
			ImGui::DragFloat2("Position", glm::value_ptr(transform.Position), 0.01f);

			float rot = glm::degrees(transform.Rotation);
			if (ImGui::DragFloat("Rotation", &rot, 0.1f)) {
				transform.Rotation = glm::radians(rot);
			}

			ImGui::DragFloat2("Scale", glm::value_ptr(transform.Scale), 0.01f);
			
			ImGui::TreePop();
		}
	}



}


}
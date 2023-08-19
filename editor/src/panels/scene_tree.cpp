#include "scene_tree.h"
#include "scene/components.h"
#include <imgui/imgui.h>

namespace Enik {

SceneTreePanel::SceneTreePanel(const Ref<Scene>& context) {
	SetContext(context);
}

void SceneTreePanel::SetContext(const Ref<Scene>& context) {
	m_Context = context;
}

void SceneTreePanel::OnImGuiRender() {
	if (!ImGui::Begin("Scene Tree")) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginTable("SceneTreeTable", 1)) {
		ImGui::EndTable();
		return;
	}
	
	m_Context->m_Registry.each([&](auto entityID) {
		Entity entity = Entity(entityID, m_Context.get());
		DrawEntityInSceneTree(entity);
	});

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
		m_SelectionContext = {};
	}

	ImGui::EndTable();
	ImGui::End();
}


void SceneTreePanel::DrawEntityInSceneTree(Entity entity) {
	ImGui::PushID(entity);

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();
	
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (m_SelectionContext == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	Component::Tag& tag = entity.Get<Component::Tag>();
	bool node_open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.Text.c_str());

	if (ImGui::IsItemClicked()) {
		m_SelectionContext = entity;
	}
	
	if (node_open) {
		// Temp
		ImGui::TextColored(ImVec4(0.1f, 0.6f, 0.1f, 1.0f), "Entity %d", (uint32_t)entity);
		ImGui::TreePop();
	}

	ImGui::PopID();
}


}
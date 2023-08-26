#include "scene_tree.h"

#include <imgui/imgui.h>

#include "scene/components.h"

namespace Enik {

SceneTreePanel::SceneTreePanel(const Ref<Scene>& context) {
	SetContext(context);
}

void SceneTreePanel::SetContext(const Ref<Scene>& context) {
	m_Context = context;
	m_SelectionContext = {};
}

void SceneTreePanel::SetSelectedEntity(const Entity& entity) {
	m_SelectionContext = entity;
}

void SceneTreePanel::OnImGuiRender() {
	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_FirstUseEver);

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
	if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered()) {
		ImGui::OpenPopup("pop");
	}
	if (ImGui::BeginPopup("pop")) {
		if (ImGui::MenuItem("Create Entity")) {
			m_Context->CreateEntity("Empty Entity");
		}

		ImGui::EndPopup();
	}

	ImGui::EndTable();
	ImGui::End();
}

void SceneTreePanel::DrawEntityInSceneTree(Entity entity) {
	ImGui::PushID(entity);

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (m_SelectionContext == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	Component::Tag& tag = entity.Get<Component::Tag>();
	bool node_open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.Text.c_str());

	if (ImGui::IsItemClicked()) {
		SetSelectedEntity(entity);
	}

	bool delete_entity = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Delete Entity")) {
			delete_entity = true;
		}
		ImGui::EndPopup();
	}

	if (node_open) {
		ImGui::TextColored(ImVec4(0.1f, 0.5f, 0.1f, 1.0f), "Entity %d, ID %lu", (uint32_t)entity, (uint64_t)entity.Get<Component::ID>());
		ImGui::TreePop();
	}

	ImGui::PopID();

	if (delete_entity) {
		m_Context->DestroyEntity(entity);
		if (m_SelectionContext == entity) {
			m_SelectionContext = {};
		}
	}
}

}
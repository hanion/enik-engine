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

UUID SceneTreePanel::GetSelectedEntityUUID() {
	if (m_SelectionContext and m_SelectionContext.Has<Component::ID>()) {
		return m_SelectionContext.Get<Component::ID>().uuid;
	}
	return -1;
}

void SceneTreePanel::SetSelectedEntity(const Entity& entity) {
	m_SelectionContext = entity;
}

void SceneTreePanel::SetSelectedEntityWithUUID(const UUID& uuid) {
	m_Context->m_Registry.view<Component::ID>().each([=](auto entity, auto& id) {
		if (id == uuid) {
			SetSelectedEntity(Entity(entity, m_Context.get()));
			return;
		}
	});
}

void SceneTreePanel::OnImGuiRender() {
	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Scene Tree")) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginTable("SceneTreeTable", 1) or m_Context == nullptr) {
		ImGui::EndTable();
		ImGui::End();
		return;
	}



	static char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, m_Context->GetName().c_str());

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();

	static const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
	bool scene_node_open = ImGui::TreeNodeEx("##SceneNameTreeNode", flags);

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) {
			auto payload_id = static_cast<const UUID*>(payload->Data);
			if (payload_id != nullptr) {
				auto payload_entity = m_Context->FindEntityByUUID(*payload_id);
				if (payload_entity) {
					// reparent entity to root level
					payload_entity.GetOrAdd<Component::Family>().Reparent(payload_entity, {});
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	ImGuiUtils::PrefixLabel("Scene");
	ImGui::PushItemWidth(-1); // Set item width to available width
	if (ImGui::InputText("##SceneName", buffer, sizeof(buffer))) {
		m_Context->SetName(buffer);
	}
	ImGui::PopItemWidth();

	if (scene_node_open) {
		m_Context->m_Registry.each([&](auto entityID) {
			Entity entity = Entity(entityID, m_Context.get());
			if (entity.Has<Component::Family>()) {
				if (not entity.Get<Component::Family>().Parent) {
					DrawEntityInSceneTree(entity);
				}
			} else {
				DrawEntityInSceneTree(entity);
			}
		});

		m_MouseReleased = false;
		ImGui::TreePop();
	}



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

void SceneTreePanel::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
	if (event.GetMouseButton() == Mouse::ButtonLeft) {
		m_MouseReleased = true;
	}
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

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("DND_ENTITY", (void*)&entity.Get<Component::ID>().uuid, sizeof(UUID));

		ImGui::Text(std::to_string(entity.Get<Component::ID>().uuid).c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) {
			auto payload_id = static_cast<const UUID*>(payload->Data);
			if (payload_id != nullptr) {
				auto payload_entity = m_Context->FindEntityByUUID(*payload_id);
				if (payload_entity) {
					payload_entity.GetOrAdd<Component::Family>().Reparent(payload_entity, entity);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (m_MouseReleased and ImGui::IsItemFocused() and ImGui::IsItemHovered()) {
		SetSelectedEntity(entity);
		m_MouseReleased = false;
	}

	bool delete_entity = false;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Create Entity")) {
			Entity new_entity = m_Context->CreateEntity("Empty Entity");
			new_entity.Add<Component::Family>().Parent = entity;
			entity.GetOrAdd<Component::Family>().AddChild(new_entity);
		}
		if (ImGui::MenuItem("Delete Entity")) {
			delete_entity = true;
		}
		ImGui::EndPopup();
	}

	if (node_open) {
		ImGui::TextColored(ImVec4(0.1f, 0.5f, 0.1f, 1.0f), "Entity %d, ID %lu", (uint32_t)entity, (uint64_t)entity.Get<Component::ID>());
		if (entity.Has<Component::Family>()) {
			for (auto& child : entity.Get<Component::Family>().Children) {
				if (child and child.Has<Component::ID>()) {
					DrawEntityInSceneTree(child);
				}
			}
		}
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
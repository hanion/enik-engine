#include "scene_tree.h"

#include <imgui/imgui.h>

#include "scene/components.h"
#include "../dialogs/dialog_confirm.h"
#include "../dialogs/dialog_file.h"
#include "scene/scene_serializer.h"

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



	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	static char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, m_Context->GetName().c_str());

	ImGui::SetNextItemWidth(-1);
	if (ImGui::InputText("##SceneName", buffer, sizeof(buffer))) {
		m_Context->SetName(buffer);
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) {
			auto payload_id = static_cast<const UUID*>(payload->Data);
			if (payload_id != nullptr) {
				auto payload_entity = m_Context->FindEntityByUUID(*payload_id);
				if (payload_entity) {
					// reparent entity to root level
					payload_entity.Reparent({});
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	ImGui::Spacing();

	m_Context->m_Registry.each([&](auto entityID) {
		Entity entity = Entity(entityID, m_Context.get());
		if (entity.Has<Component::Family>()) {
			if (not entity.HasParent()) {
				DrawEntityInSceneTree(entity);
			}
		} else {
			DrawEntityInSceneTree(entity);
		}
	});



	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
		m_SelectionContext = {};
	}
	if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered()) {
		ImGui::OpenPopup("pop_scene_tree");
	}
	if (ImGui::BeginPopup("pop_scene_tree")) {
		if (ImGui::MenuItem("Create Entity")) {
			m_SelectionContext = m_Context->CreateEntity("Empty Entity");
		}

		ImGui::EndPopup();
	}

	ImGui::EndTable();
	ImGui::End();
	m_MouseReleased = false;
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

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
	if (m_SelectionContext == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool node_open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.GetTag().c_str());

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
					payload_entity.Reparent(entity);
				}
			}
		}
		else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
			std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
			if (std::filesystem::exists(path) and path.extension() == ".prefab") {
				if (payload->IsDelivery()) {
					Entity prefab = SceneSerializer(m_Context).DeserializePrefab(path.string());
					prefab.Reparent(entity);
					SetSelectedEntity(prefab);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (m_MouseReleased and ImGui::IsItemFocused() and ImGui::IsItemHovered()) {
		SetSelectedEntity(entity);
		m_MouseReleased = false;
	}

	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Create Entity")) {
			Entity new_entity = m_Context->CreateEntity("Empty Entity");
			new_entity.Reparent(entity);
			m_SelectionContext = new_entity;
		}
		if (ImGui::MenuItem("Create Prefab")) {
			static Entity s_entity_to_prefab;
			s_entity_to_prefab = entity;
			DialogFile::OpenDialog(DialogType::SAVE_FILE,
				[&]() {
					SceneSerializer serializer = SceneSerializer(m_Context);
					serializer.SerializePrefab(DialogFile::GetSelectedPath().string(), s_entity_to_prefab);
				}
				, ".prefab"
			);
		}
		if (ImGui::MenuItem("Delete Entity")) {
			static Entity s_entity_to_delete = entity;
			DialogConfirm::OpenDialog("Delete Entity ?",
				[&](){
					m_Context->DestroyEntity(s_entity_to_delete);
				}
			);
		}
		ImGui::EndPopup();
	}

	if (node_open) {
		if (entity.HasFamily()) {
			for (auto& child : entity.GetChildren()) {
				if (child and child.Has<Component::ID>()) {
					DrawEntityInSceneTree(child);
				}
			}
		}
		ImGui::TreePop();
	}

	ImGui::PopID();

}

}
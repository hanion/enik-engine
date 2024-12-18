#include "scene_tree.h"

#include <imgui/imgui.h>

#include "core/log.h"
#include "scene/components.h"
#include "dialogs/dialog_confirm.h"
#include "dialogs/dialog_file.h"
#include "scene/scene_serializer.h"
#include "project/project.h"
#include "utils/editor_colors.h"

namespace Enik {

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
	if (!uuid) {
		return;
	}
	m_Context->m_Registry.view<Component::ID>().each([=](auto entity, auto& id) {
		if (id == uuid) {
			SetSelectedEntity(Entity(entity, m_Context.get()));
			return;
		}
	});
}

void SceneTreePanel::RenderContent() {
	if (!ImGui::BeginTable("SceneTreeTable", 1) or m_Context == nullptr) {
		ImGui::EndTable();
		ImGui::End();
		return;
	}



	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	char buffer[256];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, m_Context->GetName().c_str());

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


	/* Add Entity Button */ {
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->Style.FramePadding.x * 3.0f);
		float line_width = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		if (ImGui::Button("+", ImVec2(line_width, 0))) {
			ImGui::OpenPopup("AddEntity");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)){
			ImGui::SetTooltip("Add Entity");
		}
		if (ImGui::BeginPopup("AddEntity")) {
			if (ImGui::MenuItem("Empty Entity")) {
				Entity new_entity = m_Context->CreateEntity("Empty Entity");
				m_SelectionContext = new_entity;
			}
			ImGui::EndPopup();
		}
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

	ImGui::EndTable();
	m_MouseReleased = false;
}

void SceneTreePanel::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
	if (event.GetMouseButton() == Mouse::ButtonLeft) {
		m_MouseReleased = true;
	}
}

void SceneTreePanel::DrawEntityInSceneTree(Entity entity) {
	ImGui::PushID(entity.GetID());

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
		| ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen
		| ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowOverlap;
	if (m_SelectionContext == entity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	// no arrow for leaf nodes
	if (not entity.Has<Component::Family>() or entity.GetChildren().size() < 1) {
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	// color entities with components
	int pushed_style_color_count = 0;
	if (entity.Has<Component::Prefab>()) {
		if (entity.Get<Component::Prefab>().RootPrefab) {
			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::blue);
		} else {
			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::blue_a);
		}
		pushed_style_color_count++;
	} else if (entity.Has<Component::NativeScript>()) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::orange);
		pushed_style_color_count++;
	} else if (entity.Has<Component::AnimationPlayer>()) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::purple);
		pushed_style_color_count++;
	} else if (entity.Has<Component::AudioSources>()) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::teal);
		pushed_style_color_count++;
	} else if (entity.Has<Component::SpriteRenderer>()) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::cyan);
		pushed_style_color_count++;
	}

	bool node_open = ImGui::TreeNodeEx((void*)(uint64_t)entity, flags, "%s", entity.GetTag().c_str());

	ImGui::PopStyleColor(pushed_style_color_count);

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("DND_ENTITY", (void*)&entity.Get<Component::ID>().uuid, sizeof(UUID));

		ImGui::Text("%s", std::to_string(entity.Get<Component::ID>().uuid).c_str());
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
			path = Project::GetAbsolutePath(path);
			if (std::filesystem::exists(path) and path.extension() == ".prefab") {
				if (payload->IsDelivery()) {
					Entity prefab = SceneSerializer(m_Context).InstantiatePrefab(path.string());
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
					serializer.CreatePrefab(DialogFile::GetSelectedPath().string(), s_entity_to_prefab);
				}
				, ".prefab"
			);
		}
		if (ImGui::MenuItem("Delete Entity")) {
			static Entity s_entity_to_delete;
			s_entity_to_delete = entity;
			DialogConfirm::OpenDialog("Delete Entity ?", entity.GetTag(),
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
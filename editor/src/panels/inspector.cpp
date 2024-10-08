#include "inspector.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include "dialogs/dialog_file.h"
#include "project/project.h"
#include "script_system/script_system.h"
#include "core/input.h"
#include "utils/editor_colors.h"
#include "audio/audio.h"


namespace Enik {

void InspectorPanel::SetContext(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel) {
	m_Context = context;
	m_SceneTreePanel = scene_tree_panel;
}

void InspectorPanel::RenderContent() {
	if (!ImGui::BeginTable("InspectorTable", 1)) {
		ImGui::EndTable();
		return;
	}

	Entity selectedEntity = m_SceneTreePanel->GetSelectedEntity();
	if (selectedEntity) {
		DrawEntityInInspector(selectedEntity);
	}

	ImGui::EndTable();
}

void InspectorPanel::DrawEntityInInspector(Entity entity) {
	if (not entity or not entity.Has<Component::ID>()) {
		return;
	}

	ImGui::PushID(entity.GetID());

	int pushed_style_color = 0;
	if (entity.Has<Component::Prefab>() and not entity.Get<Component::Prefab>().RootPrefab) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::blue_a);
		pushed_style_color++;
	}


	/* Tag */ {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		std::string& text = entity.GetTag();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, text.c_str());

		if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
			text = std::string(buffer);
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)){
			ImGui::SetTooltip("%lu", (uint64_t)entity.Get<Component::ID>());
		}
	}


	/* Add Component Button */ {
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->Style.FramePadding.x * 3.0f);
		float line_width = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		if (ImGui::Button("+", ImVec2(line_width, 0))) {
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)){
			ImGui::SetTooltip("Add Component");
		}
		if (ImGui::BeginPopup("AddComponent")) {
			DisplayComponentInPopup<Component::Camera>("Camera");
			DisplayComponentInPopup<Component::SpriteRenderer>("Sprite Renderer");
			DisplayComponentInPopup<Component::RigidBody>("Rigid Body");
			DisplayComponentInPopup<Component::Collider>("Collider");
			DisplayComponentInPopup<Component::AudioSources>("Audio Sources");
			DisplayNativeScriptsInPopup();
			ImGui::EndPopup();
		}
	}

	ImGui::Spacing();

	DisplayComponentInInspector<Component::Prefab>("Prefab", entity, true, [&]() {
		auto& pref = entity.Get<Component::Prefab>();
		ImGuiUtils::PrefixLabel("RootPrefab");
		ImGui::Checkbox("##RootPrefab", &pref.RootPrefab);
		ImGuiUtils::PrefixLabel("PrefabPath");
		ImGui::Text("%s", pref.PrefabPath.string().c_str());
	});

	DisplayComponentInInspector<Component::Transform>("Transform", entity, false, [&]() {
		auto& transform = entity.Get<Component::Transform>();
		ImGuiUtils::PrefixLabel("Position");
		ImGui::DragFloat3("##Position", glm::value_ptr(transform.LocalPosition), 0.01f);

		ImGuiUtils::PrefixLabel("Rotation");
		float rot = glm::degrees(transform.LocalRotation);
		if (ImGui::DragFloat("##Rotation", &rot, 0.1f)) {
			transform.LocalRotation = glm::radians(rot);
		}

		ImGuiUtils::PrefixLabel("Scale");
		ImGui::DragFloat2("##Scale", glm::value_ptr(transform.LocalScale), 0.01f);
	});

	if (entity.Has<Component::NativeScript>()) {
		auto& script = entity.Get<Component::NativeScript>();
		DisplayComponentInInspector<Component::NativeScript>(script.ScriptName, entity, true, [&]() {
			DisplayNativeScript(script);
		});
	}

	DisplayComponentInInspector<Component::Camera>("Camera", entity, true, [&]() {
		auto& cam = entity.Get<Component::Camera>();
		ImGuiUtils::PrefixLabel("Primary");
		ImGui::Checkbox("##Primary", &cam.Primary);

		float size = cam.Cam.GetSize();
		ImGuiUtils::PrefixLabel("Size");
		if (ImGui::DragFloat("##Size", &size, 0.01f, 0.01f)) {
			cam.Cam.SetSize(size);
		}

		ImGuiUtils::PrefixLabel("Fixed Aspect Ratio");
		if (ImGui::Checkbox("##Fixed Aspect Ratio", &cam.FixedAspectRatio)) {
			m_Context->OnViewportResize(m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);
		}
		if (cam.FixedAspectRatio) {
			float ratio = cam.Cam.GetAspectRatio();
			ImGuiUtils::PrefixLabel("Aspect Ratio");
			if (ImGui::DragFloat("##Aspect Ratio", &ratio, 0.01f, 0.001f)) {
				cam.Cam.SetAspectRatio(ratio);
			}
		}
	});

	DisplayComponentInInspector<Component::SpriteRenderer>("Sprite Renderer", entity, true, [&]() {
		auto& sprite = entity.Get<Component::SpriteRenderer>();

		ImGuiUtils::PrefixLabel("Color");
		ImGui::ColorEdit4("##Sprite Color", glm::value_ptr(sprite.Color));
		// ImGui::ColorEdit3("Sprite Color", glm::value_ptr(sprite.Color));

		DisplaySpriteTexture(sprite);

		DisplaySubTexture(sprite);
	});

	DisplayComponentInInspector<Component::RigidBody>("Rigid Body", entity, true, [&]() {
		auto& rigid_body = entity.Get<Component::RigidBody>();

		ImGuiUtils::PrefixLabel("Mass");
		ImGui::DragFloat("##Mass", &rigid_body.Mass, 0.01f, 0.01f);

		ImGuiUtils::PrefixLabel("UseGravity");
		ImGui::Checkbox("##UseGravity", &rigid_body.UseGravity);

		ImGui::SeparatorText("Debug");

		ImGuiUtils::PrefixLabel("Velocity");
		ImGui::DragFloat3("##Velocity", glm::value_ptr(rigid_body.Velocity), 0.01f, 0.01f);

		ImGuiUtils::PrefixLabel("Force");
		ImGui::DragFloat3("##Force", glm::value_ptr(rigid_body.Force), 0.01f, 0.01f);

	});

	DisplayComponentInInspector<Component::Collider>("Collider", entity, true, [&]() {
		auto& collider = entity.Get<Component::Collider>();

		std::string text = collider.String();

		ImGuiUtils::PrefixLabel("Shape");
		if (ImGui::BeginCombo("##ColliderShape", text.c_str())) {
			if (ImGui::Selectable("Circle",
				collider.Shape == Component::ColliderShape::CIRCLE)) {
				collider.Shape =  Component::ColliderShape::CIRCLE;
			}
			if (ImGui::Selectable("Plane",
				collider.Shape == Component::ColliderShape::PLANE)) {
				collider.Shape =  Component::ColliderShape::PLANE;
			}
			if (ImGui::Selectable("Box",
				collider.Shape == Component::ColliderShape::BOX)) {
				collider.Shape =  Component::ColliderShape::BOX;
			}
			ImGui::EndCombo();
		}

		ImGuiUtils::PrefixLabel("Is Area");
		ImGui::Checkbox("##IsArea", &collider.IsArea);

		switch (collider.Shape) {
			case Component::ColliderShape::CIRCLE: {
				ImGuiUtils::PrefixLabel("Radius");
				ImGui::DragFloat("##Radius", &collider.Float, 0.01f);

				ImGuiUtils::PrefixLabel("Center");
				ImGui::DragFloat3("##Center", glm::value_ptr(collider.Vector), 0.01f);
				break;
			}
			case Component::ColliderShape::PLANE: {
				ImGuiUtils::PrefixLabel("Thickness");
				ImGui::DragFloat("##Thickness", &collider.Float, 0.01f);

				ImGuiUtils::PrefixLabel("Normal");
				ImGui::DragFloat3("##Normal", glm::value_ptr(collider.Vector), 0.01f);
				break;
			}
			case Component::ColliderShape::BOX: {
				ImGuiUtils::PrefixLabel("Center");
				ImGui::DragFloat3("##Center", glm::value_ptr(collider.Vector), 0.01f);
				break;
			}
		}

	});

	DisplayComponentInInspector<Component::AudioSources>("Audio Sources", entity, true, [&]() {
		auto& sources = entity.Get<Component::AudioSources>();

		size_t to_remove_index = SIZE_MAX;

		for (size_t i = 0; i < sources.SourcePaths.size(); i++) {
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer, sources.SourcePaths[i].c_str());

			auto file = std::filesystem::path(buffer);
			if (file.empty()) {
				file = "[AudioSource]";
				ImGuiUtils::PrefixLabel("Source");
			}
			else {
				ImGuiUtils::PrefixLabel(file.stem().c_str());
			}


			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::teal);
			if (ImGui::Button(file.filename().c_str())) {
				sources.Play(sources.SourcePaths[i].stem());
			}

			if (ImGui::IsItemHovered()){
				ImGui::SetTooltip("%s", file.c_str());

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
					to_remove_index = i;
				}
			}
			ImGui::PopStyleColor();

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
					if (payload->DataSize > 0) {
						char* payload_data = new char[payload->DataSize];
						if (payload_data != nullptr) {
							memcpy(payload_data, payload->Data, payload->DataSize);
							strcpy(buffer, reinterpret_cast<const char*>(payload_data));
							std::filesystem::path path = { buffer };
							if (path.has_extension() and path.extension() == ".wav") {
								sources.SourcePaths[i] = path;
							}
							delete[] payload_data;
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (ImGui::Button("Add", ImVec2(-1,0))) {
			sources.SourcePaths.emplace_back("");
		}

		if (to_remove_index != SIZE_MAX) {
			sources.SourcePaths.erase(sources.SourcePaths.begin() + to_remove_index);
		}

	});

	ImGui::PopStyleColor(pushed_style_color);
	ImGui::PopID();
}

template <typename T>
void InspectorPanel::DisplayComponentInInspector(const std::string& name, Entity& entity, const bool can_delete, const std::function<void()>& lambda) {
	if (not entity.Has<T>()) {
		return;
	}

	constexpr ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	bool remove_component = false;

	if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), tree_node_flags, "%s", name.c_str())) {
		if (can_delete) {
			if (Input::IsMouseButtonPressed(1) and ImGui::IsItemHovered()) {
				ImGui::OpenPopup("ComponentSettings");
			}

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->Style.FramePadding.x * 1.0f);
			float line_width = GImGui->Font->FontSize + GImGui->Style.FramePadding.x * 2.0f;
			if (ImGui::Button("...", ImVec2(line_width, 0))) {
				ImGui::OpenPopup("ComponentSettings");
			}
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Delete Component")) {
					remove_component = true;
				}
				ImGui::EndPopup();
			}
		}

		lambda();

		ImGui::TreePop();
	}

	if (remove_component) {
		entity.Remove<T>();
	}
}

template <typename T>
void InspectorPanel::DisplayComponentInPopup(const std::string& name) {
	ImGui::BeginDisabled(m_SceneTreePanel->GetSelectedEntity().Has<T>());

	if (ImGui::MenuItem(name.c_str())) {
		m_SceneTreePanel->GetSelectedEntity().Add<T>();
		ImGui::CloseCurrentPopup();
		if (std::is_same<T, Component::Camera>::value) {
			m_Context->OnViewportResize(m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);
		}
	}

	ImGui::EndDisabled();
}





void InspectorPanel::DisplaySpriteTexture(Component::SpriteRenderer& sprite) {
	std::function<void()> BeDragDropTargetTexture = [&]() {
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
				std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
				path = Project::GetAbsolutePath(path);
				if (std::filesystem::exists(path) and path.extension() == ".png") {
					sprite.Texture = Texture2D::Create(path.string());
					auto relative = std::filesystem::relative(path, Project::GetProjectDirectory());
					sprite.TexturePath = relative;
					if (sprite.SubTexture != nullptr) {
						sprite.SubTexture->SetTexture(sprite.Texture);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	};

	ImGuiUtils::PrefixLabel("Texture");

	if (sprite.TexturePath.empty()) {
		if (ImGui::Button("Add Texture")) {
			DialogFile::OpenDialog(
				DialogType::OPEN_FILE,
				[&]() {
					auto relative = std::filesystem::relative(DialogFile::GetSelectedPath(), Project::GetProjectDirectory());
					sprite.TexturePath = relative;
					sprite.Texture = Texture2D::Create(DialogFile::GetSelectedPath().string());
					if (sprite.SubTexture != nullptr) {
						sprite.SubTexture->SetTexture(sprite.Texture);
					}
				},
				".png");
		}
		BeDragDropTargetTexture();
		return;
	}

	ImVec4 tint_col = ImVec4(sprite.Color.r, sprite.Color.g, sprite.Color.b, sprite.Color.a);
	ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
	ImVec2 avail = ImGui::GetContentRegionAvail();

	// ! to not crash at ImGui::TreePop();
	if (avail.x < 0 or avail.y < -10.0f) {
		ImGui::Button("##TooSmallToShowTexture");
		return;
	}

	avail.y = (avail.y < 32) ? avail.y : 32;
	avail.x -= GImGui->Style.FramePadding.x;

	ImTextureID tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(0));
	ImVec2 tex_size = ImVec2(0, 0);

	if (sprite.Texture) {
		tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(sprite.Texture->GetRendererID()));
		tex_size = ImVec2(sprite.Texture->GetWidth(), sprite.Texture->GetHeight());
	}
	else if (sprite.SubTexture) {
		tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(sprite.SubTexture->GetTexture()->GetRendererID()));
		tex_size = ImVec2(sprite.SubTexture->GetTexture()->GetWidth(), sprite.SubTexture->GetTexture()->GetHeight());
	}

	if (tex_size.x > avail.x) {
		tex_size.y = tex_size.y - ((tex_size.x - avail.x) * (tex_size.y / tex_size.x));
		tex_size.x = avail.x;
	}
	if (tex_size.y > avail.y) {
		tex_size.x = tex_size.x - ((tex_size.y - avail.y) * (tex_size.x / tex_size.y));
		tex_size.y = avail.y;
	}

	tex_size = ImVec2(glm::max(32.0f, tex_size.x), glm::max(32.0f, tex_size.y));
	ImVec2 childSize = ImVec2(tex_size.x + GImGui->Style.FramePadding.x, tex_size.y + GImGui->Style.FramePadding.y);
	if (ImGui::BeginChild("TextureChild", childSize, false, ImGuiWindowFlags_NoScrollbar)) {
		ImGui::Image(tex_id, tex_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), tint_col, border_col);

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) and not sprite.TexturePath.empty()) {
			if (ImGui::BeginTooltip()) {
				std::string width  = std::to_string(sprite.Texture->GetWidth());
				std::string height = std::to_string(sprite.Texture->GetHeight());
				std::string txt = sprite.TexturePath.string() + " - " + width + "x" + height;
				ImGui::Text("%s", txt.c_str());
				ImVec2 texture_tooltip_size = ImVec2(tex_size.x*8.0f, tex_size.y*8.0f);
				ImGui::Image(tex_id, texture_tooltip_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
				ImGui::EndTooltip();
			}
		}

		if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered()) {
			ImGui::OpenPopup("popup_remove_texture");
		}
		if (ImGui::BeginPopup("popup_remove_texture")) {
			if (ImGui::MenuItem("Remove Texture")) {
				sprite.TexturePath.clear();
				sprite.Texture = nullptr;
				sprite.SubTexture = nullptr;
			}
			ImGui::EndPopup();
		}

		BeDragDropTargetTexture();
		ImGui::EndChild();
	}

	if (sprite.Texture != nullptr) {
		ImGuiUtils::PrefixLabel("Tile Scale");
		ImGui::DragFloat("##Tile Scale", &sprite.TileScale, 0.01f);
	}

	ImGuiUtils::PrefixLabel("Filter");
	if (ImGui::Checkbox("##Filter", &sprite.mag_filter_linear)) {
		sprite.Texture = Texture2D::Create(Project::GetAbsolutePath(sprite.TexturePath).string(), sprite.mag_filter_linear);
		if (sprite.SubTexture != nullptr) {
			sprite.SubTexture->SetTexture(sprite.Texture);
		}
	}
}

void InspectorPanel::DisplaySubTexture(Component::SpriteRenderer& sprite) {
	if (sprite.Texture == nullptr) {
		return;
	}
	if (sprite.SubTexture == nullptr) {
		ImVec2 fill_width = ImVec2(ImGui::GetContentRegionAvail().x, 0);
		if (ImGui::Button("Create Sub Texture", fill_width)) {
			sprite.SubTexture = SubTexture2D::CreateFromTileIndex(
				sprite.Texture, glm::vec2(18), glm::vec2(0), glm::vec2(2));
		}
		return;
	}

	static ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
	tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	tree_node_flags |= ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	bool remove_sub_texture = false;

	if (ImGui::TreeNodeEx("##DisplaySubTexture", tree_node_flags, "Sub Texture")) {
		if (Input::IsMouseButtonPressed(1) and ImGui::IsItemHovered()) {
			ImGui::OpenPopup("SubTextureSettings");
		}

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->Style.FramePadding.x);
		if (ImGui::Button("...", ImVec2(lineHeight, lineHeight))) {
			ImGui::OpenPopup("SubTextureSettings");
		}
		if (ImGui::BeginPopup("SubTextureSettings")) {
			if (ImGui::MenuItem("Delete SubTexture")) {
				remove_sub_texture = true;
			}
			ImGui::EndPopup();
		}

		ImGuiUtils::PrefixLabel("Tile Size");
		if (ImGui::DragFloat2("##TileSize", glm::value_ptr(sprite.SubTexture->GetTileSize()), 0.01f)) {
			sprite.SubTexture->UpdateSubTexture2D();
		}
		ImGuiUtils::PrefixLabel("Tile Index");
		if (ImGui::DragFloat2("##TileIndex", glm::value_ptr(sprite.SubTexture->GetTileIndex()), 0.01f)) {
			sprite.SubTexture->UpdateSubTexture2D();
		}
		ImGuiUtils::PrefixLabel("Tile Separation");
		if (ImGui::DragFloat2("##TileSeparation", glm::value_ptr(sprite.SubTexture->GetTileSeparation()), 0.01f)) {
			sprite.SubTexture->UpdateSubTexture2D();
		}

		ImGui::TreePop();
	}

	if (remove_sub_texture) {
		sprite.SubTexture.reset();
	}
}

void InspectorPanel::DisplayNativeScriptsInPopup() {
	if (ImGui::BeginMenu("Native Script")) {

		ImGui::BeginDisabled(m_SceneTreePanel->GetSelectedEntity().Has<Component::NativeScript>());
		for (auto& pair : ScriptRegistry::GetRegistry()) {
			if (ImGui::MenuItem(pair.first.c_str())) {
				m_SceneTreePanel->GetSelectedEntity().Add<Component::NativeScript>().Bind(pair.first, pair.second);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndDisabled();

		ImGui::EndMenu();
	}
}

void InspectorPanel::DisplayNativeScript(Component::NativeScript& script) {
	for (auto& pair : script.NativeScriptFields) {
		NativeScriptField& field = pair.second;

		EN_CORE_ASSERT(field.Value != nullptr);

		ImGuiUtils::PrefixLabel(field.Name);

		std::string field_name = ("##"+field.Name);
		const char* label = field_name.c_str();
		float speed = 0.01f;

		switch (field.Type) {
			case FieldType::NONE:
				EN_CORE_ERROR("DisplayNativeScript field.Type is NONE !");
				continue;
			case FieldType::BOOL:
				ImGui::Checkbox(label, static_cast<bool*>(field.Value));
				continue;
			case FieldType::INT:
				ImGui::DragInt(label, static_cast<int*>(field.Value));
				continue;
			case FieldType::FLOAT:
				ImGui::DragFloat(label, static_cast<float*>(field.Value), speed);
				continue;
			case FieldType::DOUBLE:
				ImGui::DragFloat(label, (float*)static_cast<double*>(field.Value), speed);
				continue;
			case FieldType::VEC2:
				ImGui::DragFloat2(label, static_cast<float*>(field.Value), speed);
				continue;
			case FieldType::VEC3:
				ImGui::DragFloat3(label, static_cast<float*>(field.Value), speed);
				continue;
			case FieldType::VEC4:
				ImGui::DragFloat4(label, static_cast<float*>(field.Value), speed);
				continue;
			case FieldType::PREFAB: {
				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy(buffer, static_cast<std::string*>(field.Value)->c_str());

				auto file = std::filesystem::path(buffer);
				if (file.empty()) {
					file = "[Prefab]";
				}
				ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::blue);
				ImGui::Button(file.filename().c_str());
				if (ImGui::IsItemHovered()){
					ImGui::SetTooltip("%s", file.c_str());
				}
				ImGui::PopStyleColor();

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
						if (payload->DataSize > 0) {
							char* payload_data = new char[payload->DataSize];
							if (payload_data != nullptr) {
								memcpy(payload_data, payload->Data, payload->DataSize);
								strcpy(buffer, reinterpret_cast<const char*>(payload_data));
								std::filesystem::path path = { buffer };
								if (path.has_extension() and path.extension() == ".prefab") {
									*static_cast<std::string*>(field.Value) = path.string();
								}
								delete[] payload_data;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
				continue;
			}
			case FieldType::STRING: {
				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy(buffer, static_cast<std::string*>(field.Value)->c_str());
				if (ImGui::InputTextMultiline(label, buffer, sizeof(buffer))) {
					*static_cast<std::string*>(field.Value) = std::string(buffer);
				}

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
						if (payload->DataSize > 0) {
							char* payload_data = new char[payload->DataSize];
							if (payload_data != nullptr) {
								memcpy(payload_data, payload->Data, payload->DataSize);
								strcpy(buffer, reinterpret_cast<const char*>(payload_data));
								*static_cast<std::string*>(field.Value) = std::string(buffer);
								delete[] payload_data;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}

				continue;
			}
			case FieldType::ENTITY: {
				uint64_t* id = static_cast<uint64_t*>(field.Value);

				std::string entity_name;

				if (Entity entity = m_Context->FindEntityByUUID(*id)) {
					entity_name = entity.GetTag();
				}
				else {
					entity_name = "[Entity]";
				}

				if (ImGui::Button(entity_name.c_str())) {
					m_SceneTreePanel->SetSelectedEntityWithUUID(*id);
				}

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) {
						auto payload_id = static_cast<const UUID*>(payload->Data);
						if (payload_id != nullptr) {
							*id = *payload_id;
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (ImGui::IsItemHovered()){
					ImGui::SetTooltip("%s", std::to_string(*id).c_str());
				}

				continue;
			}
		}
	}
}

}
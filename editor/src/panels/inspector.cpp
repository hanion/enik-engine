#include "inspector.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include "asset/asset_manager.h"
#include "asset/asset_manager_editor.h"
#include "dialogs/dialog_file.h"
#include "project/project.h"
#include "renderer/font.h"
#include "scene/components.h"
#include "script_system/script_system.h"
#include "core/input.h"
#include "utils/editor_colors.h"
#include "audio/audio.h"
#include "renderer/renderer2D.h"
#include "utils/imgui_utils.h"
#include "editor_layer.h"


namespace Enik {

constexpr ImGuiTreeNodeFlags inner_tree_node_flags = ImGuiTreeNodeFlags_OpenOnDoubleClick |
	ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;


void InspectorPanel::SetContext(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel, AnimationEditorPanel* animation_panel, EditorLayer* el) {
	m_Context = context;
	m_SceneTreePanel = scene_tree_panel;
	m_AnimationEditorPanel = animation_panel;
	m_EditorLayer = el;
}

void InspectorPanel::RenderContent() {
	Entity selectedEntity = m_SceneTreePanel->GetSelectedEntity();
	if (selectedEntity) {
		DrawEntityInInspector(selectedEntity);
	}
}

template <typename T>
int color_component() {
	if constexpr (std::is_same<T, Component::Prefab>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::blue);
	} else if constexpr (std::is_same<T, Component::NativeScript>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::orange);
	} else if constexpr (std::is_same<T, Component::SpriteRenderer>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::cyan);
	} else if constexpr (std::is_same<T, Component::AnimationPlayer>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::purple);
	} else if constexpr (std::is_same<T, Component::AudioSources>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::teal);
	} else if constexpr (std::is_same<T, Component::Text>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::pale_pink);
	} else if constexpr (std::is_same<T, Component::SceneControl>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::persistent);
	} else if constexpr (std::is_same<T, Component::RigidBody>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::rigidbody);
	} else if constexpr (std::is_same<T, Component::CollisionBody>::value) {
		ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::rigidbody);
	} else {
		return 0;
	}
	return 1;
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
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->Font->FontSize);
		float line_width = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		if (ImGui::Button("+", ImVec2(line_width, 0))) {
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)){
			ImGui::SetTooltip("Add Component");
		}
		if (ImGui::BeginPopup("AddComponent")) {
			DisplayComponentInPopup<Component::SceneControl>("SceneControl");
			DisplayComponentInPopup<Component::Camera>("Camera");
			DisplayComponentInPopup<Component::SpriteRenderer>("Sprite Renderer");
			DisplayComponentInPopup<Component::RigidBody>("Rigid Body");
			DisplayComponentInPopup<Component::CollisionBody>("Collision Body");
			DisplayComponentInPopup<Component::CollisionShape>("Collision Shape");
			DisplayComponentInPopup<Component::AudioSources>("Audio Sources");
			DisplayComponentInPopup<Component::AnimationPlayer>("Animation Player");
			DisplayComponentInPopup<Component::Text>("Text");
			DisplayNativeScriptsInPopup();
			ImGui::EndPopup();
		}
	}

	ImGui::Spacing();

	if (!ImGui::BeginChild("inpsector_child")) {
		ImGui::PopStyleColor(pushed_style_color);
		return;
	}

	if (!ImGui::BeginTable("InspectorTable", 1)) {
		ImGui::EndChild();
		ImGui::PopStyleColor(pushed_style_color);
		return;
	}


	DisplayComponentInInspector<Component::SceneControl>("Scene Control", entity, true, [&]() {
		auto& sc = entity.Get<Component::SceneControl>();
		ImGuiUtils::PrefixLabel("Persistent");
		ImGui::Checkbox("##Persistent", &sc.Persistent);
	});


	DisplayComponentInInspector<Component::Prefab>("Prefab", entity, true, [&]() {
		auto& pref = entity.Get<Component::Prefab>();
		if (pref.RootPrefab) {
			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::prefab);
			ImGuiUtils::PrefixLabel("Prefab");
			if (ImGui::Button(pref.PrefabPath.filename().string().c_str())) {
				m_EditorLayer->RequestOpenAsset(pref.PrefabPath);
			}
			if (ImGui::IsItemHovered()){
				ImGui::SetTooltip("%s", pref.PrefabPath.string().c_str());
			}
			ImGui::PopStyleColor();
		}
	});

	DisplayComponentInInspector<Component::Transform>("Transform", entity, false, [&]() {
		auto& transform = entity.Get<Component::Transform>();
		ImGuiUtils::PrefixLabel("Position");
		ImGui::DragFloat3("##Position", glm::value_ptr(transform.LocalPosition), 0.01f);

		ImGuiUtils::PrefixLabel("Rotation");
		glm::vec3 euler_angles = glm::degrees(glm::eulerAngles(transform.LocalRotation));
		if (ImGui::DragFloat3("##Rotation", glm::value_ptr(euler_angles), 0.1f)) {
			transform.LocalRotation = glm::quat(glm::radians(euler_angles));
		}

		ImGuiUtils::PrefixLabel("Scale");
		ImGui::DragFloat3("##Scale", glm::value_ptr(transform.LocalScale), 0.01f);
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

	DisplayComponentInInspector<Component::AnimationPlayer>("Animation Player", entity, true, [&]() {
		Component::AnimationPlayer& ap = entity.Get<Component::AnimationPlayer>();

		ImGuiUtils::PrefixLabel("Current");
		if (ImGuiUtils::AssetButton<Animation>(ap.CurrentAnimation)) {
			m_AnimationEditorPanel->SetAnimation(ap.CurrentAnimation);
		}

		ImGuiUtils::PrefixLabel("Paused");
		ImGui::Checkbox("##APPaused", &ap.Paused);
		ImGuiUtils::PrefixLabel("Time");
		ImGui::DragFloat("##APTime", &ap.CurrentTime);
		

		if (!ImGui::TreeNodeEx("Animations", inner_tree_node_flags)){
			return;
		}

		for (auto it = ap.Animations.begin(); it != ap.Animations.end(); ++it) {
			AssetHandle handle = it->second;
			ImGui::PushID(&it->first);

			ImGuiUtils::PrefixLabel(it->first);
			if (ImGuiUtils::AssetButton<Animation>(handle) && handle) {
				Ref<Animation> anim = AssetManager::GetAsset<Animation>(handle);
				m_AnimationEditorPanel->SetAnimation(handle);
				ap.Animations[anim->Name] = handle;
				ImGui::PopID();
				break; // modified the map, should break
			}
			if (!handle || !AssetManager::IsAssetHandleValid(handle)) {
				ap.Animations.erase(it);
				ImGui::PopID();
				break; // modified the map, should break
			}
			ImGui::PopID();
		}

		static AssetHandle new_handle = 0;
		ImGuiUtils::PrefixLabel("");
		ImGuiUtils::AssetButton<Animation>(new_handle);
		if (new_handle && AssetManager::IsAssetHandleValid(new_handle)) {
			Ref<Animation> new_animation = AssetManager::GetAsset<Animation>(new_handle);
			ap.Animations[new_animation->Name] = new_handle;
			new_handle = 0;
		}

		ImGui::TreePop();
	});

	DisplayComponentInInspector<Component::RigidBody>("Rigid Body", entity, true, [&]() {
		auto& body = entity.Get<Component::RigidBody>();

		ImGuiUtils::PrefixLabel("Layer");
		if (ImGui::BeginCombo("##Layer", std::to_string(body.Layer).c_str())) {
			if (ImGui::Selectable("0")) { body.Layer = 0; }
			if (ImGui::Selectable("1")) { body.Layer = 1; }
			if (ImGui::Selectable("2")) { body.Layer = 2; }
			ImGui::EndCombo();
		}

		ImGuiUtils::PrefixLabel("Mass");
		float mass = body.GetMass();
		if (ImGui::DragFloat("##Mass", &mass, 0.01f, 0.001f)) {
			if (mass > 0.0f) {
				body.SetMass(mass);
			}
		}

		ImGuiUtils::PrefixLabel("Gravity Factor");
		float gf = body.GetGravityFactor();
		if (ImGui::DragFloat("##GF", &gf, 0.01f)) {
			body.SetGravityFactor(gf);
		}
	});

	DisplayComponentInInspector<Component::CollisionBody>("Collision Body", entity, true, [&]() {
		auto& body = entity.Get<Component::CollisionBody>();

		ImGuiUtils::PrefixLabel("Layer");
		if (ImGui::BeginCombo("##Layer", std::to_string(body.Layer).c_str())) {
			if (ImGui::Selectable("0")) { body.Layer = 0; }
			if (ImGui::Selectable("1")) { body.Layer = 1; }
			if (ImGui::Selectable("2")) { body.Layer = 2; }
			ImGui::EndCombo();
		}

		ImGuiUtils::PrefixLabel("Is Static");
		bool is_static = body.MotionType == JPH::EMotionType::Static;
		if (ImGui::Checkbox("##IsStatic", &is_static)) {
			body.MotionType = is_static ? JPH::EMotionType::Static : JPH::EMotionType::Kinematic;
		}

		ImGuiUtils::PrefixLabel("Is Sensor");
		ImGui::Checkbox("##IsSensor", &body.IsSensor);
	});

	DisplayComponentInInspector<Component::CollisionShape>("Collision Shape", entity, true, [&]() {
		auto& cs = entity.Get<Component::CollisionShape>();

		std::string text = cs.ToString();

		ImGuiUtils::PrefixLabel("Shape");
		if (ImGui::BeginCombo("##ColliderShape", text.c_str())) {
			if (ImGui::Selectable("Box",
				cs.Shape == Component::CollisionShape::Type::BOX)) {
				cs.Shape =  Component::CollisionShape::Type::BOX;
			}
			if (ImGui::Selectable("Circle",
				cs.Shape == Component::CollisionShape::Type::CIRCLE)) {
				cs.Shape =  Component::CollisionShape::Type::CIRCLE;
			}
			ImGui::EndCombo();
		}

		switch (cs.Shape) {
			case Component::CollisionShape::Type::CIRCLE: {
				ImGuiUtils::PrefixLabel("Radius");
				ImGui::DragFloat("##Radius", &cs.CircleRadius, 0.01f);
			} break;
			case Component::CollisionShape::Type::BOX: {
				ImGuiUtils::PrefixLabel("Scale");
				ImGui::DragFloat3("##Scale", glm::value_ptr(cs.BoxScale), 0.01f);
			} break;
			case Component::CollisionShape::Type::NONE: break;
		}

	});

	DisplayComponentInInspector<Component::AudioSources>("Audio Sources", entity, true, [&]() {
		auto& sources = entity.Get<Component::AudioSources>();

		size_t to_remove_index = SIZE_MAX;

		for (size_t i = 0; i < sources.SourcePaths.size(); i++) {
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy(buffer, sources.SourcePaths[i].string().c_str());

			auto file = std::filesystem::path(buffer);
			if (file.empty()) {
				file = "[AudioSource]";
				ImGuiUtils::PrefixLabel("Source");
			}
			else {
				ImGuiUtils::PrefixLabel(file.stem().string());
			}


			ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::teal);
			if (ImGui::Button(file.filename().string().c_str())) {
				sources.Play(sources.SourcePaths[i].stem().string());
			}

			if (ImGui::IsItemHovered()){
				ImGui::SetTooltip("%s", file.string().c_str());

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

	DisplayComponentInInspector<Component::Text>("Text", entity, true, [&]() {
		Component::Text& text = entity.Get<Component::Text>();

		ImGuiUtils::PrefixLabel("Color");
		ImGui::ColorEdit4("##TextColor", glm::value_ptr(text.Color));

		ImGuiUtils::PrefixLabel("Font");
		ImGuiUtils::AssetButton<FontAsset>(text.Font);

		if (text.Font) {
			if (ImGui::TreeNodeEx("Atlas", inner_tree_node_flags)) {
				Ref<Texture2D> texture = AssetManager::GetAsset<FontAsset>(text.Font)->AtlasTexture;
				ImTextureID tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(texture->GetRendererID()));
				ImVec2 tex_size = ImGui::GetContentRegionAvail();
				tex_size.y = tex_size.x;
				ImGui::Image(tex_id, tex_size);
				ImGui::TreePop();
			}
		}

		ImGuiUtils::PrefixLabel("Scale");
		ImGui::DragFloat("##TextScale", &text.Scale, 0.1f, 0.1f, 100.0f, "%.1f");

		ImGuiUtils::PrefixLabel("Visible");
		ImGui::DragFloat("##Visible", &text.Visible, 0.001f, 0.0f, 1.0f, "%.3f");

		ImGuiUtils::PrefixLabel("Data");
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, text.Data.c_str());
		if (ImGui::InputTextMultiline("##TextData", buffer, sizeof(buffer))) {
			text.Data = std::string(buffer);
		}
	});

	ImGui::PopStyleColor(pushed_style_color);
	ImGui::EndTable();
	ImGui::Dummy(ImVec2(0,60));
	ImGui::EndChild();
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
	
	int pushed_color = color_component<T>();

	bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), tree_node_flags, "%s", name.c_str());

	ImGui::PopStyleColor(pushed_color);


	if (can_delete) {
		if (ImGui::IsMouseClicked(1) and ImGui::IsItemHovered()) {
			ImGui::OpenPopup("ComponentSettings");
		}

		float avail_width = ImGui::GetContentRegionAvail().x;
		float button_width = GImGui->Font->FontSize + GImGui->Style.FramePadding.x * 2.0f;
		ImGui::SameLine(avail_width - button_width);
		if (ImGui::Button("...", ImVec2(button_width, 0))) {
			ImGui::OpenPopup("ComponentSettings");
		}
		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Delete Component")) {
				remove_component = true;
			}
			ImGui::EndPopup();
		}
	}

	if (open) {
		lambda();
		ImGui::TreePop();
	}

	if (remove_component) {
		entity.Remove<T>();
	}
}

template <typename T>
void InspectorPanel::DisplayComponentInPopup(const std::string& name) {
	Entity s = m_SceneTreePanel->GetSelectedEntity();
	bool disabled = s.Has<T>();

	if (!disabled) {
		if (std::is_same<T, Component::RigidBody>::value) {
			disabled = s.Has<Component::CollisionBody>();
		} else if (std::is_same<T, Component::CollisionBody>::value) {
			disabled = s.Has<Component::RigidBody>();
		}
	}

	if (disabled) {
		return;
	}

	int pushed = color_component<T>();
	ImGui::BeginDisabled(disabled);

	if (ImGui::MenuItem(name.c_str())) {
		m_SceneTreePanel->GetSelectedEntity().Add<T>();
		ImGui::CloseCurrentPopup();
		if (std::is_same<T, Component::Camera>::value) {
			m_Context->OnViewportResize(m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);
		}
	}

	ImGui::EndDisabled();
	ImGui::PopStyleColor(pushed);
}





void InspectorPanel::DisplaySpriteTexture(Component::SpriteRenderer& sprite) {
	ImGuiUtils::PrefixLabel("Texture");
	ImGuiUtils::AssetButton<Texture2D>(sprite.Handle);
	bool hovered = ImGui::IsItemHovered();

	ImGuiUtils::PrefixLabel("Tile Scale");
	ImGui::DragFloat("##Tile Scale", &sprite.TileScale, 0.01f);

	if (sprite.Handle == 0 || !hovered) {
		return;
	}

	Ref<Texture2D> texture = Renderer2D::s_ErrorTexture;
	if (sprite.Handle and AssetManager::IsAssetHandleValid(sprite.Handle)) {
		texture = AssetManager::GetAsset<Texture2D>(sprite.Handle);
	}

	ImTextureID tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(texture->GetRendererID()));
	ImVec2 tex_size = ImVec2(texture->GetWidth(), texture->GetHeight());

	constexpr float target_y = 256.0f;
	float scale = target_y / tex_size.y;
	tex_size.y = target_y;
	tex_size.x *= scale;

	if (ImGui::BeginTooltip()) {
		std::string width  = std::to_string(texture->GetWidth());
		std::string height = std::to_string(texture->GetHeight());
		std::string txt = std::to_string(sprite.Handle) + " - " + width + "x" + height;
		ImGui::Text("%s", txt.c_str());
		ImGui::Image(tex_id, tex_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::EndTooltip();
	}


// TODO: make texture import settings, this option should not be on the sprite, but on the texture asset
// 	ImGuiUtils::PrefixLabel("Filter");
// 	if (ImGui::Checkbox("##Filter", &sprite.mag_filter_linear)) {
// 		sprite.Texture = Texture2D::Create(Project::GetAbsolutePath(sprite.TexturePath).string(), sprite.mag_filter_linear);
// 		if (sprite.SubTexture != nullptr) {
// 			sprite.SubTexture->SetTexture(sprite.Texture);
// 		}
// 	}
}

void InspectorPanel::DisplaySubTexture(Component::SpriteRenderer& sprite) {
	if (sprite.Handle == 0) {
		return;
	}

	if (sprite.SubTexture == nullptr) {
		ImGuiUtils::PrefixLabel("Sub Texture");
		ImVec2 fill_width = ImVec2(ImGui::GetContentRegionAvail().x, 0);
		if (ImGui::Button("Create Sub Texture", fill_width)) {
			sprite.SubTexture = SubTexture2D::CreateFromTileIndex(
				sprite.Handle, glm::vec2(18), glm::vec2(0), glm::vec2(2));
		}
		return;
	}

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	bool remove_sub_texture = false;
	bool open = ImGui::TreeNodeEx("Sub Texture", inner_tree_node_flags | ImGuiTreeNodeFlags_AllowItemOverlap);

	if (ImGui::IsMouseClicked(1) and ImGui::IsItemHovered()) {
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

	if (open) {
		ImGuiUtils::PrefixLabel("Tile Size");
		if (ImGui::DragFloat2("##TileSize", glm::value_ptr(sprite.SubTexture->TileSize), 0.01f)) {
			Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(sprite.Handle);
			sprite.SubTexture->UpdateSubTexture2D(texture);
		}
		ImGuiUtils::PrefixLabel("Tile Index");
		if (ImGui::DragFloat2("##TileIndex", glm::value_ptr(sprite.SubTexture->TileIndex), 0.01f)) {
			Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(sprite.Handle);
			sprite.SubTexture->UpdateSubTexture2D(texture);
		}
		ImGuiUtils::PrefixLabel("Tile Separation");
		if (ImGui::DragFloat2("##TileSeparation", glm::value_ptr(sprite.SubTexture->TileSeparation), 0.01f)) {
			Ref<Texture2D> texture = AssetManager::GetAsset<Texture2D>(sprite.Handle);
			sprite.SubTexture->UpdateSubTexture2D(texture);
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
				ImGui::Button(file.filename().string().c_str());
				if (ImGui::IsItemHovered()){
					ImGui::SetTooltip("%s", file.string().c_str());
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
				UUID& id = *static_cast<UUID*>(field.Value);
				if (EntityButton(id)) {
					m_SceneTreePanel->SetSelectedEntityWithUUID(id);
				}
				continue;
			}
		}
	}
}


bool InspectorPanel::EntityButton(UUID& id) {
	bool pressed = false;
	std::string entity_name;

	if (Entity entity = m_Context->FindEntityByUUID(id)) {
		entity_name = entity.GetTag();
	}
	else {
		entity_name = "[Entity]";
	}

	if (ImGui::Button(entity_name.c_str())) {
		pressed = true;
	}
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ENTITY")) {
			auto payload_id = static_cast<const UUID*>(payload->Data);
			if (payload_id != nullptr) {
				id = *payload_id;
			}
		}
		ImGui::EndDragDropTarget();
	}
	if (ImGui::IsItemHovered()){
		ImGui::SetTooltip("%s", std::to_string(id).c_str());
	}

	return pressed;
}


}

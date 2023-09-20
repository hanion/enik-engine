#include "inspector.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include "../dialogs/dialog_file.h"
#include "project/project.h"
#include "script_system/script_system.h"


namespace Enik {

InspectorPanel::InspectorPanel(const Ref<Scene>& context) {
	SetContext(context);
}

void InspectorPanel::SetContext(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel) {
	m_Context = context;
	m_SceneTreePanel = scene_tree_panel;
}

void InspectorPanel::OnImGuiRender() {
	ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Inspector")) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginTable("InspectorTable", 1)) {
		ImGui::EndTable();
		return;
	}

	Entity selectedEntity = m_SceneTreePanel->GetSelectedEntity();
	if (selectedEntity) {
		DrawEntityInInspector(selectedEntity);

		ImVec2 buttonSize(150, GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f);
		ImVec2 buttonPosition((ImGui::GetContentRegionAvail().x - buttonSize.x) * 0.5f, ImGui::GetCursorPosY());
		ImGui::SetCursorPos(buttonPosition);

		if (ImGui::Button("Add Component", buttonSize)) {
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent")) {
			DisplayComponentInPopup<Component::Camera>("Camera");
			DisplayComponentInPopup<Component::SpriteRenderer>("Sprite Renderer");
			DisplayComponentInPopup<Component::RigidBody>("Rigid Body");
			DisplayComponentInPopup<Component::Collider>("Collider");
			DisplayNativeScriptsInPopup();
			ImGui::EndPopup();
		}
	}

	ImGui::EndTable();
	ImGui::End();
}

void InspectorPanel::DrawEntityInInspector(Entity entity) {
	DisplayComponentInInspector<Component::Tag>("Tag", entity, false, [&]() {
		auto& text = entity.Get<Component::Tag>().Text;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy(buffer, text.c_str());
		ImGuiUtils::PrefixLabel("Text");
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
			text = std::string(buffer);
		}
	});

	DisplayComponentInInspector<Component::Transform>("Transform", entity, false, [&]() {
		auto& transform = entity.Get<Component::Transform>();
		ImGuiUtils::PrefixLabel("Position");
		ImGui::DragFloat3("##Position", glm::value_ptr(transform.Position), 0.01f);

		ImGuiUtils::PrefixLabel("Rotation");
		float rot = glm::degrees(transform.Rotation);
		if (ImGui::DragFloat("##Rotation", &rot, 0.1f)) {
			transform.Rotation = glm::radians(rot);
		}

		ImGuiUtils::PrefixLabel("Scale");
		ImGui::DragFloat2("##Scale", glm::value_ptr(transform.Scale), 0.01f);
	});

	DisplayComponentInInspector<Component::Camera>("Camera", entity, true, [&]() {
		auto& cam = entity.Get<Component::Camera>();
		ImGui::Checkbox("Primary", &cam.Primary);

		static float size = cam.Cam.GetSize();
		ImGuiUtils::PrefixLabel("Size");
		if (ImGui::DragFloat("##Size", &size, 0.01f, 0.01f)) {
			cam.Cam.SetSize(size);
		}

		if (ImGui::Checkbox("Fixed Aspect Ratio", &cam.FixedAspectRatio)) {
			m_Context->OnViewportResize(m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);
		}
		if (cam.FixedAspectRatio) {
			static float ratio = cam.Cam.GetAspectRatio();
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

	DisplayComponentInInspector<Component::NativeScript>("Native Script", entity, true, [&]() {
		auto& script = entity.Get<Component::NativeScript>();
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 0.2f, 1.0f), script.ScriptName.c_str());

		if (not script.Instance or script.Instance == nullptr) {
			script.Instance = script.InstantiateScript();
		}
		script.Instance->OnInspectorRender();
	});

	DisplayComponentInInspector<Component::RigidBody>("Rigid Body", entity, true, [&]() {
		auto& rigid_body = entity.Get<Component::RigidBody>();

		glm::vec3& velocity = rigid_body.Velocity;
		ImGuiUtils::PrefixLabel("Velocity");
		ImGui::DragFloat3("##Velocity", glm::value_ptr(velocity), 0.01f, 0.01f);

		glm::vec3& force = rigid_body.Force;
		ImGuiUtils::PrefixLabel("Force");
		ImGui::DragFloat3("##Force", glm::value_ptr(force), 0.01f, 0.01f);

		float& mass = rigid_body.Mass;
		ImGuiUtils::PrefixLabel("Mass");
		ImGui::DragFloat("##Mass", &mass, 0.01f, 0.01f);
	});

	DisplayComponentInInspector<Component::Collider>("Collider", entity, true, [&]() {
		auto& collider = entity.Get<Component::Collider>();

		std::string text = (collider.Shape == Component::ColliderShape::CIRCLE) ? "Circle" : "Plane";

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
			ImGui::EndCombo();
		}

		if (collider.Shape == Component::ColliderShape::CIRCLE) {
			ImGuiUtils::PrefixLabel("Radius");
			ImGui::DragFloat("##Radius", &collider.flat, 0.01f);

			ImGuiUtils::PrefixLabel("Center");
			ImGui::DragFloat3("##Center", glm::value_ptr(collider.vector), 0.01f);
		}
		else {
			ImGuiUtils::PrefixLabel("Thickness");
			ImGui::DragFloat("##Thickness", &collider.flat, 0.01f);

			ImGuiUtils::PrefixLabel("Normal");
			ImGui::DragFloat3("##Normal", glm::value_ptr(collider.vector), 0.01f);
		}

	});
}

template <typename T>
void InspectorPanel::DisplayComponentInInspector(const std::string& name, Entity& entity, const bool can_delete, const std::function<void()>& lambda) {
	if (not entity.Has<T>()) {
		return;
	}

	static ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
	tree_node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	tree_node_flags |= ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;

	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);

	bool remove_component = false;

	if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), tree_node_flags, name.c_str())) {
		if (can_delete) {
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->Style.FramePadding.x);
			if (ImGui::Button("...", ImVec2(lineHeight, lineHeight))) {
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

		// FIXME: if inspector panel is too small this crashed the program
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
	}

	if (std::is_same<T, Component::Camera>::value) {
		m_Context->OnViewportResize(m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);
	}

	ImGui::EndDisabled();
}





void InspectorPanel::DisplaySpriteTexture(Component::SpriteRenderer& sprite) {
	std::function<void()> BeDragDropTargetTexture = [&]() {
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
				std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
				if (std::filesystem::exists(path) and path.extension() == ".png") {
					sprite.Texture = Texture2D::Create(path);
					auto relative = std::filesystem::relative(path, Project::GetProjectDirectory());
					sprite.TexturePath = relative;
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
					sprite.Texture = Texture2D::Create(DialogFile::GetSelectedPath());
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

	avail.y = (avail.y < 128) ? avail.y : 128;
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

		if (ImGui::IsItemHovered() and not sprite.TexturePath.empty()) {
			ImGui::SetTooltip("%s", sprite.TexturePath.c_str());
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

	if (ImGui::Checkbox("Filter", &sprite.mag_filter_linear)) {
		sprite.Texture = Texture2D::Create(Project::GetAbsolutePath(sprite.TexturePath), sprite.mag_filter_linear);
	}
}

void InspectorPanel::DisplaySubTexture(Component::SpriteRenderer& sprite) {
	if (sprite.Texture == nullptr) {
		return;
	}
	if (sprite.SubTexture == nullptr) {
		if (ImGui::Button("Create Sub Texture")) {
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

}
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
		LabelPrefix("Tag");
		if (ImGui::InputText("Tag", buffer, sizeof(buffer))) {
			text = std::string(buffer);
		}
	}


	if (entity.Has<Component::Transform>()) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (ImGui::TreeNodeEx((void*)typeid(Component::Transform).hash_code(), treeNodeFlags, "Transform")) {
			auto& transform = entity.Get<Component::Transform>();
			LabelPrefix("Position");
			ImGui::DragFloat2("Position", glm::value_ptr(transform.Position), 0.01f);

			LabelPrefix("Rotation");
			float rot = glm::degrees(transform.Rotation);
			if (ImGui::DragFloat("Rotation", &rot, 0.1f)) {
				transform.Rotation = glm::radians(rot);
			}

			LabelPrefix("Scale");
			ImGui::DragFloat2("Scale", glm::value_ptr(transform.Scale), 0.01f);
			
			ImGui::TreePop();
		}
	}


	if (entity.Has<Component::Camera>()) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (ImGui::TreeNodeEx((void*)typeid(Component::Camera).hash_code(), treeNodeFlags, "Camera")) {
			auto& cam = entity.Get<Component::Camera>();

			ImGui::Checkbox("Primary", &cam.Primary);
			
			static float size = cam.Cam.GetSize();
			LabelPrefix("Size");
			if (ImGui::DragFloat("Size", &size, 0.01f, 0.01f)) {
				cam.Cam.SetSize(size);
			}
			
			if (ImGui::Checkbox("Fixed Aspect Ratio", &cam.FixedAspectRatio)) {
				m_Context->OnViewportResize(m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);
			}
			if (cam.FixedAspectRatio) {
				static float ratio = cam.Cam.GetAspectRatio();
				LabelPrefix("Aspect Ratio");
				if (ImGui::DragFloat("Aspect Ratio", &ratio, 0.01f, 0.001f)) {
					cam.Cam.SetAspectRatio(ratio);
				}
			}
			
			ImGui::TreePop();
		}
	}


	if (entity.Has<Component::SpriteRenderer>()) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);

		if (ImGui::TreeNodeEx((void*)typeid(Component::SpriteRenderer).hash_code(), treeNodeFlags, "Sprite Renderer")) {
			auto& sprite = entity.Get<Component::SpriteRenderer>();
			
			LabelPrefix("Color");
			ImGui::ColorEdit4("Sprite Color", glm::value_ptr(sprite.Color));
			// ImGui::ColorEdit3("Sprite Color", glm::value_ptr(sprite.Color));
			
			/* Texture */ {
				// ImVec4 tint_col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
				ImVec4 tint_col = ImVec4(sprite.Color.r,sprite.Color.g,sprite.Color.b,sprite.Color.a);
				ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

				if (sprite.Texture) {
					ImTextureID tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(sprite.Texture->GetRendererID()));
					ImVec2 tex_size = ImVec2(sprite.Texture->GetWidth(), sprite.Texture->GetHeight());
					ImGui::Image(tex_id, tex_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), tint_col, border_col);
				}
				else if (sprite.SubTexture) {
					ImTextureID tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(sprite.SubTexture->GetTexture()->GetRendererID()));
					ImVec2 tex_size = ImVec2(sprite.SubTexture->GetTexture()->GetWidth()/2.0f, sprite.SubTexture->GetTexture()->GetHeight()/2.0f);	
					ImGui::Image(tex_id, tex_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), tint_col, border_col);
				}

			}
			
			ImGui::TreePop();
		}
	}


	if (entity.Has<Component::NativeScript>()) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		
		if (ImGui::TreeNodeEx((void*)typeid(Component::NativeScript).hash_code(), treeNodeFlags, "Script")) {
			ImGui::TextColored(ImVec4(0.4f, 0.7f, 0.2f, 1.0f), "Has Script");
			ImGui::TreePop();
		}
	}

}



void InspectorPanel::LabelPrefix(std::string_view title, InspectorPanel::ItemLabelFlag flag) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const ImVec2 lineStart = ImGui::GetCursorScreenPos();
	const ImGuiStyle& style = ImGui::GetStyle();
	float fullWidth = ImGui::GetContentRegionAvail().x;
	float itemWidth = ImGui::CalcItemWidth() + style.ItemSpacing.x;
	ImVec2 textSize = ImGui::CalcTextSize(title.begin(), title.end());
	ImRect textRect;
	textRect.Min = ImGui::GetCursorScreenPos();
	if (flag & ItemLabelFlag::Right)
		textRect.Min.x = textRect.Min.x + itemWidth;
	textRect.Max = textRect.Min;
	textRect.Max.x += fullWidth - itemWidth;
	textRect.Max.y += textSize.y;

	ImGui::SetCursorScreenPos(textRect.Min);

	ImGui::AlignTextToFramePadding();
	// Adjust text rect manually because we render it directly into a drawlist instead of using public functions.
	textRect.Min.y += window->DC.CurrLineTextBaseOffset;
	textRect.Max.y += window->DC.CurrLineTextBaseOffset + 1; // +1 so there is no letters bottom clip

	ImGui::ItemSize(textRect);
	if (ImGui::ItemAdd(textRect, window->GetID(title.data(), title.data() + title.size()))) {
		ImGui::RenderTextEllipsis(ImGui::GetWindowDrawList(), textRect.Min, textRect.Max, textRect.Max.x,
			textRect.Max.x, title.data(), title.data() + title.size(), &textSize);

		if (textRect.GetWidth() < textSize.x && ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%.*s", (int)title.size(), title.data());
		}
	}
	if (flag & ItemLabelFlag::Left) {
		ImGui::SetCursorScreenPos(ImVec2(textRect.Max.x, textRect.Max.y - (textSize.y + window->DC.CurrLineTextBaseOffset)));
		ImGui::SameLine();
	}
	else if (flag & ItemLabelFlag::Right) {
		ImGui::SetCursorScreenPos(lineStart);
	}
}

}
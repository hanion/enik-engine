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
	ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_FirstUseEver);

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
		ImVec2 buttonPosition( (ImGui::GetContentRegionAvail().x - buttonSize.x) * 0.5f, ImGui::GetCursorPosY());
		ImGui::SetCursorPos(buttonPosition);

		if (ImGui::Button("Add Component", buttonSize)) {
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent")) {
			DisplayComponentInPopup<Component::Camera>("Camera");
			DisplayComponentInPopup<Component::SpriteRenderer>("Sprite Renderer");
			ImGui::EndPopup();
		}
	}

	ImGui::EndTable();
	ImGui::End();
}


void InspectorPanel::DrawEntityInInspector(Entity entity) {

	DisplayComponentInInspector<Component::Tag>("Tag",entity,false,[&]() {
		auto& text =  entity.Get<Component::Tag>().Text;

		char buffer[256];
		memset(buffer, 0 ,sizeof(buffer));
		strcpy(buffer, text.c_str());
		LabelPrefix("Text");
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
			text = std::string(buffer);
		}
	});


	DisplayComponentInInspector<Component::Transform>("Transform", entity, false, [&]() {
		auto& transform = entity.Get<Component::Transform>();
		LabelPrefix("Position");
		ImGui::DragFloat3("##Position", glm::value_ptr(transform.Position), 0.01f);

		LabelPrefix("Rotation");
		float rot = glm::degrees(transform.Rotation);
		if (ImGui::DragFloat("##Rotation", &rot, 0.1f)) {
			transform.Rotation = glm::radians(rot);
		}

		LabelPrefix("Scale");
		ImGui::DragFloat2("##Scale", glm::value_ptr(transform.Scale), 0.01f);
	});

	DisplayComponentInInspector<Component::Camera>("Camera",entity, true, [&]() {
		auto& cam = entity.Get<Component::Camera>();
		ImGui::Checkbox("Primary", &cam.Primary);
		
		static float size = cam.Cam.GetSize();
		LabelPrefix("Size");
		if (ImGui::DragFloat("##Size", &size, 0.01f, 0.01f)) {
			cam.Cam.SetSize(size);
		}
		
		if (ImGui::Checkbox("Fixed Aspect Ratio", &cam.FixedAspectRatio)) {
			m_Context->OnViewportResize(m_Context->m_ViewportWidth, m_Context->m_ViewportHeight);
		}
		if (cam.FixedAspectRatio) {
			static float ratio = cam.Cam.GetAspectRatio();
			LabelPrefix("Aspect Ratio");
			if (ImGui::DragFloat("##Aspect Ratio", &ratio, 0.01f, 0.001f)) {
				cam.Cam.SetAspectRatio(ratio);
			}
		}
	});

	DisplayComponentInInspector<Component::SpriteRenderer>("Sprite Renderer", entity, true, [&]() {
		auto& sprite = entity.Get<Component::SpriteRenderer>();
			
		LabelPrefix("Color");
		ImGui::ColorEdit4("##Sprite Color", glm::value_ptr(sprite.Color));
		// ImGui::ColorEdit3("Sprite Color", glm::value_ptr(sprite.Color));
		
		LabelPrefix("Tile Scale");
		ImGui::DragFloat("##Tile Scale", &sprite.TileScale, 0.01f);

		/* Texture */ {
			LabelPrefix("Texture");		
			// ImVec4 tint_col = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			ImVec4 tint_col = ImVec4(sprite.Color.r,sprite.Color.g,sprite.Color.b,sprite.Color.a);
			ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
			ImVec2 avail = ImGui::GetContentRegionAvail();

			// to not crash in line 245, at ImGui::TreePop();
			if (avail.x < 0 or avail.y < -10.0f) {
				ImGui::Button("##TooSmallToShowTexture");
				return;
			}
			
			avail.y = (avail.y < 128) ? avail.y : 128;
			avail.x -= GImGui->Style.FramePadding.x;

			ImTextureID tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(0));
			ImVec2 tex_size = ImVec2(0,0);

			if (sprite.Texture) {
				tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(sprite.Texture->GetRendererID()));
				tex_size = ImVec2(sprite.Texture->GetWidth(), sprite.Texture->GetHeight());
			}
			else if (sprite.SubTexture) {
				tex_id = reinterpret_cast<ImTextureID>(static_cast<uint32_t>(sprite.SubTexture->GetTexture()->GetRendererID()));
				tex_size = ImVec2(sprite.SubTexture->GetTexture()->GetWidth(), sprite.SubTexture->GetTexture()->GetHeight());	
			}

			if (tex_size.x > avail.x) {
				tex_size.y = tex_size.y - ( (tex_size.x-avail.x) * (tex_size.y/tex_size.x) );
				tex_size.x = avail.x;
			}
			if (tex_size.y > avail.y) {
				tex_size.x = tex_size.x - ( (tex_size.y-avail.y) * (tex_size.x/tex_size.y) );
				tex_size.y = avail.y;
			}

			tex_size = ImVec2(glm::max(32.0f, tex_size.x), glm::max(32.0f, tex_size.y)); 
			ImVec2 childSize = ImVec2(tex_size.x + GImGui->Style.FramePadding.x, tex_size.y + GImGui->Style.FramePadding.y);
			if (ImGui::BeginChild("TextureChild", childSize, false, ImGuiWindowFlags_NoScrollbar)) {
				ImGui::Image(tex_id, tex_size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), tint_col, border_col);
				
				/* Drag drop target */ {
					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_FILE_PATH")) {
							std::filesystem::path path = std::filesystem::path(static_cast<const char*>(payload->Data));
							if (std::filesystem::exists(path) and path.extension() == ".png") {								
								sprite.Texture = Texture2D::Create(path);
							}
						}
						ImGui::EndDragDropTarget();
					}
				}
				ImGui::EndChild();
			}
		}
	});

	DisplayComponentInInspector<Component::NativeScript>("Native Script", entity, true, [&]() {
		ImGui::TextColored(ImVec4(0.4f, 0.7f, 0.2f, 1.0f), "Has Script");
	});

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



template <typename T>
void InspectorPanel::DisplayComponentInInspector(const std::string& name, Entity& entity, const bool canDelete, const std::function<void()>& lambda) {
	if (not entity.Has<T>()) { return; }

	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
	treeNodeFlags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen; 
	treeNodeFlags |= ImGuiTreeNodeFlags_FramePadding   | ImGuiTreeNodeFlags_AllowItemOverlap;


	ImGui::TableNextRow();
	ImGui::TableSetColumnIndex(0);
	
	bool removeComponent = false;

	if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str())) {
		
		if (canDelete) {
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - GImGui->Style.FramePadding.x);
			if (ImGui::Button("...", ImVec2(lineHeight,lineHeight))) {
				ImGui::OpenPopup("ComponentSettings");
			}
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Delete Component")) {
					removeComponent = true;
				}
				ImGui::EndPopup();
			}
		}

		lambda();
		
		// FIXME: if inspector panel is too small this crashed the program
		ImGui::TreePop();
	}

	if (removeComponent) {
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
}
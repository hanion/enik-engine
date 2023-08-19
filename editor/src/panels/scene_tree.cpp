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
	ImGui::Begin("Scene Tree");

	m_Context->m_Registry.each([&](auto entityID) {
		Entity entity = Entity(entityID, m_Context.get());
		DrawEntityInSceneTree(entity);
	});

	ImGui::End();
}


void SceneTreePanel::DrawEntityInSceneTree(Entity entity) {
	Component::Tag& tag = entity.Get<Component::Tag>();
	ImGui::Text("%s",tag.Text.c_str());

}


}
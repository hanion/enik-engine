#pragma once
#include <base.h>

#include "scene/entity.h"
#include "scene/scene.h"
#include "scene_tree.h"
#include "scene/components.h"

namespace Enik {

class InspectorPanel {
public:
	InspectorPanel() = default;
	InspectorPanel(const Ref<Scene>& context);
	~InspectorPanel() {}

	void SetContext(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel = nullptr);

	void OnImGuiRender();

private:
	void DrawEntityInInspector(Entity entity);


	template <typename T>
	void DisplayComponentInInspector(const std::string& name, Entity& entity, const bool can_delete, const std::function<void()>& lambda);

	template <typename T>
	void DisplayComponentInPopup(const std::string& name);

	void DisplaySpriteTexture(Component::SpriteRenderer& sprite);
	void DisplaySubTexture(Component::SpriteRenderer& sprite);

	void DisplayNativeScriptsInPopup();

private:
	Ref<Scene> m_Context;

	SceneTreePanel* m_SceneTreePanel;
};

}
#pragma once
#include <base.h>

#include "scene/entity.h"
#include "scene/scene.h"
#include "scene_tree.h"

namespace Enik {

class InspectorPanel {
public:
	InspectorPanel() = default;
	InspectorPanel(const Ref<Scene>& context);
	~InspectorPanel() {}

	void SetContext(const Ref<Scene>& context, SceneTreePanel* sceneTreePanel = nullptr);

	void OnImGuiRender();

private:
	void DrawEntityInInspector(Entity entity);
	
	enum ItemLabelFlag {
		Left = 1u << 0u,
		Right = 1u << 1u,
		Default = Left,
	};
	void LabelPrefix(std::string_view title, InspectorPanel::ItemLabelFlag flags = InspectorPanel::ItemLabelFlag::Default);
private:
	Ref<Scene> m_Context;

	SceneTreePanel* m_SceneTreePanel;
};

}
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
	
private:
	Ref<Scene> m_Context;

	SceneTreePanel* m_SceneTreePanel;
};

}
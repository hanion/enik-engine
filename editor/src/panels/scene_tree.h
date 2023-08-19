#pragma once
#include <base.h>
#include "scene/scene.h"
#include "scene/entity.h"

namespace Enik {

class SceneTreePanel {
public:
	SceneTreePanel() = default;
	SceneTreePanel(const Ref<Scene>& context);
	~SceneTreePanel() {}

	void SetContext(const Ref<Scene>& context);

	void OnImGuiRender();

private:
	void DrawEntityInSceneTree(Entity entity);

private:
	Ref<Scene> m_Context;

	Entity m_SelectionContext;

	friend class InspectorPanel;

};


}
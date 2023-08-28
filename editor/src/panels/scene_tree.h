#pragma once
#include <base.h>

#include "scene/entity.h"
#include "scene/scene.h"
#include "imgui_utils.h"


namespace Enik {

class SceneTreePanel {
public:
	SceneTreePanel() = default;
	SceneTreePanel(const Ref<Scene>& context);
	~SceneTreePanel() {}

	void SetContext(const Ref<Scene>& context);

	bool IsSelectedEntityValid() const { return m_Context->m_Registry.valid(m_SelectionContext); }

	Entity GetSelectedEntity() const { return m_SelectionContext; }
	void SetSelectedEntity(const Entity& entity);

	void OnImGuiRender();

private:
	void DrawEntityInSceneTree(Entity entity);

private:
	Ref<Scene> m_Context;

	Entity m_SelectionContext;
};

}
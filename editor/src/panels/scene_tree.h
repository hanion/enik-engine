#pragma once
#include <base.h>

#include "editor_panel.h"
#include "scene/entity.h"
#include "scene/scene.h"
#include "utils/imgui_utils.h"


namespace Enik {

class SceneTreePanel : public EditorPanel {
public:
	SceneTreePanel() : EditorPanel("Scene Tree") {}
	~SceneTreePanel() {}

	void SetContext(const Ref<Scene>& context);

	bool IsSelectedEntityValid() const { return m_Context->m_Registry.valid(m_SelectionContext); }

	Entity GetSelectedEntity() const { return m_SelectionContext; }
	UUID GetSelectedEntityUUID();
	void SetSelectedEntity(const Entity& entity);
	void SetSelectedEntityWithUUID(const UUID& uuid);

	virtual void RenderContent() override final;

	void OnMouseButtonReleased(MouseButtonReleasedEvent& event);

private:
	void DrawEntityInSceneTree(Entity entity);

private:
	Ref<Scene> m_Context;

	bool m_MouseReleased = false;

	Entity m_SelectionContext;
};

}
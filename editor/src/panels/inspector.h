#pragma once
#include <base.h>

#include "editor_panel.h"
#include "panels/animation_editor.h"
#include "scene/entity.h"
#include "scene/scene.h"
#include "scene_tree.h"
#include "scene/components.h"

namespace Enik {

class InspectorPanel : public EditorPanel {
public:
	InspectorPanel() : EditorPanel("Inspector") {}
	~InspectorPanel() {}

	void SetContext(const Ref<Scene>& context, SceneTreePanel* scene_tree_panel = nullptr, AnimationEditorPanel* animation_panel = nullptr);

private:
	virtual void RenderContent() override final;

	void DrawEntityInInspector(Entity entity);


	template <typename T>
	void DisplayComponentInInspector(const std::string& name, Entity& entity, const bool can_delete, const std::function<void()>& lambda);

	template <typename T>
	void DisplayComponentInPopup(const std::string& name);

	void DisplaySpriteTexture(Component::SpriteRenderer& sprite);
	void DisplaySubTexture(Component::SpriteRenderer& sprite);

	void DisplayNativeScriptsInPopup();
	void DisplayNativeScript(Component::NativeScript& script);


	bool EntityButton(UUID& id);

private:
	Ref<Scene> m_Context;

	SceneTreePanel* m_SceneTreePanel;
	AnimationEditorPanel* m_AnimationEditorPanel;
};

}
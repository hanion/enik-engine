#pragma once
#include <base.h>

#include "editor_panel.h"
#include "scene/entity.h"
#include "scene/scene.h"
#include "scene_tree.h"
#include "scene/components.h"

namespace Enik {

class AnimationEditorPanel : public EditorPanel {
public:
	AnimationEditorPanel() : EditorPanel("Animation Editor") {}

	void SetAnimation(const AssetHandle& handle) { m_EditingAnimation = handle; }
	bool HasAnimation() { return m_EditingAnimation != 0; }

private:
	virtual void RenderContent() override final;

	void DrawTimelineWithHandles(float timeline_start, float timeline_end, Track& track, float duration);

	void DrawPreviewTimeline(float timeline_start, float timeline_end, float duration, int track_count);

	void DrawTimelinePanel(const Ref<Animation>& animation);
	void DrawKeyframeDetailsPanel(const Ref<Animation>& animation);

private:
	AssetHandle m_EditingAnimation = 0;
	float m_left_panel_width = 0.8f;
	float m_current_time = 0.0f;
	bool m_playing = false;
	Track* m_selected_track = nullptr;
	Keyframe* m_selected_keyframe = nullptr;
};

}


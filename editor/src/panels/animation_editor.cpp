#include "animation_editor.h"
#include "asset/asset_manager.h"
#include "asset/importer/animation_importer.h"
#include "gtc/type_ptr.hpp"
#include "imgui.h"
#include "scene/animation.h"
#include "utils/editor_colors.h"
#include "utils/imgui_utils.h"
#include <variant>

namespace Enik {
constexpr ImU32 timeline_color = IM_COL32(50, 50, 50, 155);
constexpr ImU32 accent_color = IM_COL32(191, 127, 255, 255); //(ImU32)((ImColor)(EditorColors::purple));
constexpr ImU32 handle_color_inactive = IM_COL32(100, 100, 100, 255);

void AnimationEditorPanel::DrawPreviewTimeline(float timeline_start, float timeline_end, float duration, int track_count) {
	constexpr float track_height = 5.0f;
	constexpr float handle_size = 10.0f;
	constexpr float stick_size = 2.0f;

	ImVec2 timeline_size = ImVec2(ImGui::GetContentRegionAvail().x, track_height);
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

	ImVec2 timeline_start_pos = cursor_pos;
	ImVec2 timeline_end_pos = ImVec2(cursor_pos.x + timeline_size.x, cursor_pos.y + timeline_size.y);
	draw_list->AddRectFilled(timeline_start_pos, timeline_end_pos, timeline_color);

	float timeline_width = timeline_end_pos.x - timeline_start_pos.x;
	float normalized_preview_time = (m_current_time - timeline_start) / (timeline_end - timeline_start);
	float handle_x = timeline_start_pos.x + normalized_preview_time * timeline_width;
	ImVec2 handle_center = ImVec2(handle_x, timeline_start_pos.y + timeline_size.y / 2);
	ImVec2 handle_min = ImVec2(handle_center.x - handle_size / 2, handle_center.y - handle_size / 2);
	ImVec2 handle_max = ImVec2(handle_center.x + handle_size / 2, handle_center.y + handle_size / 2);
	draw_list->AddRectFilled(handle_min, handle_max, accent_color);

	ImGui::SetCursorScreenPos(ImVec2(timeline_start_pos.x, timeline_start_pos.y - track_height));
	ImGui::InvisibleButton("PreviewTimeline", ImVec2(timeline_width, track_height + handle_size));

	if (ImGui::IsItemActive()) {
		float mouse_x = ImGui::GetIO().MousePos.x;
		float clamped_mouse_x = std::clamp(mouse_x, timeline_start_pos.x, timeline_end_pos.x);
		float normalized_position = (clamped_mouse_x - timeline_start_pos.x) / timeline_width;
		m_current_time = timeline_start + normalized_position * (timeline_end - timeline_start);
	}

	float stick_height = 15.0f + track_count * 34.0f;
	ImVec2 stick_min = ImVec2(handle_center.x - stick_size / 2, handle_center.y - stick_size / 2);
	ImVec2 stick_max = ImVec2(handle_center.x + stick_size / 2, handle_center.y + stick_height);
	draw_list->AddRectFilled(stick_min, stick_max, accent_color);

	ImGui::SetCursorScreenPos(ImVec2(cursor_pos.x, cursor_pos.y + handle_size + 5.0f));
}


void AnimationEditorPanel::DrawTimelineWithHandles(float timeline_start, float timeline_end, Track& track, float duration) {
	ImGui::PushID(&track);
	ImVec2 timeline_size = ImVec2(ImGui::GetContentRegionAvail().x, 10);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
	ImVec2 timeline_start_pos = cursor_pos;
	ImVec2 timeline_end_pos = ImVec2(cursor_pos.x + timeline_size.x, cursor_pos.y + timeline_size.y / 2);

	draw_list->AddRectFilled(timeline_start_pos, timeline_end_pos, timeline_color);

	float timeline_width = timeline_end_pos.x - timeline_start_pos.x;
	for (size_t i = 0; i < track.Keyframes.size(); ++i) {
		auto& kf = track.Keyframes[i];
		ImGui::PushID(&kf);
		float normalized_position = (track.Keyframes[i].Time - timeline_start) / (timeline_end - timeline_start);
		float handle_x = timeline_start_pos.x + normalized_position * timeline_width;
		ImVec2 handle_pos = ImVec2(handle_x, timeline_start_pos.y + timeline_size.y / 4);
		ImVec2 handle_size = ImVec2(10, 10);

		ImVec2 handle_min = ImVec2(handle_pos.x - handle_size.x / 2, handle_pos.y - handle_size.y / 2);
		ImVec2 handle_max = ImVec2(handle_pos.x + handle_size.x / 2, handle_pos.y + handle_size.y / 2);
		bool is_selected = (m_selected_keyframe == &kf);
		ImU32 handle_color = is_selected ? accent_color : handle_color_inactive;
		draw_list->AddRectFilled(handle_min, handle_max, handle_color);

		ImGui::SetCursorScreenPos(handle_min);
		ImGui::InvisibleButton("Handle", handle_size);

		if (is_selected && ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			float mouse_x = ImGui::GetIO().MousePos.x;
			float clamped_mouse_x = std::clamp(mouse_x, timeline_start_pos.x, timeline_end_pos.x);
			float newNormalized_position = (clamped_mouse_x - timeline_start_pos.x) / timeline_width;
			track.Keyframes[i].Time = timeline_start + newNormalized_position * (timeline_end - timeline_start);
		}

		if (ImGui::IsItemClicked()) {
			m_selected_keyframe = &kf;
			m_selected_track = &track;
		}
		ImGui::PopID();
	}

	ImGui::SetCursorScreenPos(timeline_start_pos);
	ImGui::InvisibleButton("Timeline", timeline_size);

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("TimelineContextMenu");
	}
	if (ImGui::BeginPopupContextItem("TimelineContextMenu")) {
		if (ImGui::MenuItem("Add Keyframe")) {
			float mouse_x = ImGui::GetIO().MousePos.x;
			float clamped_mouse_x = std::clamp(mouse_x, timeline_start_pos.x, timeline_end_pos.x);
			float new_normalized_position = (clamped_mouse_x - timeline_start_pos.x) / timeline_width;
			float new_time = timeline_start + new_normalized_position * (timeline_end - timeline_start);

			Keyframe new_keyframe;
			new_keyframe.Time = new_time;
			new_keyframe.InitializeZero(track.Property);
			track.Keyframes.emplace_back(new_keyframe);
			m_selected_track = &track;
			track.SortKeyframes();
			for (auto& kf : track.Keyframes) {
				if (kf.Value == new_keyframe.Value && kf.Time == new_keyframe.Time) {
					m_selected_keyframe = &kf;
					break;
				}
			}
		}
		ImGui::EndPopup();
	}

	ImGui::PopID();
}

void AnimationEditorPanel::RenderContent() {
	ImGuiUtils::AssetButton<Animation>(m_EditingAnimation);
	if (!m_EditingAnimation || !AssetManager::IsAssetHandleValid(m_EditingAnimation)) {
		return;
	}
	Ref<Animation> animation = AssetManager::GetAsset<Animation>(m_EditingAnimation);

	ImGui::SameLine();
	ImGui::Checkbox("Play", &m_playing);
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	ImGui::DragFloat("Time    ", &m_current_time, 0.001f, 0.0f, animation->Duration, "%.3f");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	ImGui::DragFloat("Duration    ", &animation->Duration, 0.001f, 0.0f, 10000.0f, "%.3f");

	ImGui::SameLine();
	if (ImGui::Button("Save Animation")) {
		auto& path = Project::GetAssetManagerEditor()->GetMetadata(m_EditingAnimation).FilePath;
		AnimationImporter::SerializeAnimation(animation, path);
	}

	// TODO: preview
	if (m_playing) {
		m_current_time += 0.007; // 144 fps
		if (m_current_time > animation->Duration) {
			m_current_time = 0.0f;
			m_playing = false;
		}
	}

	ImGui::Spacing();
	
	ImVec2 panel_size = ImGui::GetContentRegionAvail();
	float timeline_width = m_left_panel_width * panel_size.x;
	float inspector_width = panel_size.x - timeline_width - ImGui::GetStyle().FramePadding.x * 2;

	ImGui::BeginChild("TimelinePanel", ImVec2(timeline_width, panel_size.y), true);
	DrawTimelinePanel(animation);
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("KeyframeDetails", ImVec2(inspector_width, panel_size.y), true);
	DrawKeyframeDetailsPanel(animation);
	ImGui::EndChild();
}

void AnimationEditorPanel::DrawTimelinePanel(const Ref<Animation>& animation) {
	ImGui::Dummy({0,2.0f});
	DrawPreviewTimeline(0.0f, animation->Duration, animation->Duration, animation->Tracks.size());
	for (auto& track : animation->Tracks) {
		ImGui::Text("%s ", Animation::TrackPropertyToString(track.Property).c_str());
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			m_selected_track = &track;
			ImGui::OpenPopup("TrackDetailPopup");
		}
		DrawTimelineWithHandles(0.0f, animation->Duration, track, animation->Duration);
	}
	ImGui::Spacing();
	if (ImGui::Button("Add Track")) {
		ImGui::OpenPopup("AnimationAddTrack");
	}
	if (ImGui::BeginPopup("AnimationAddTrack")) {
		for (const auto& pair : StringToTrackProperty) {
			bool has = false;
			for (const auto& tr : animation->Tracks) {
				if (tr.Property == pair.second) {
					has = true;
				}
			}
			if (!has && ImGui::MenuItem(pair.first.c_str())) {
				Track new_track;
				new_track.Property = pair.second;
				animation->Tracks.push_back(new_track);
			}
		}
		ImGui::EndPopup();
	}
	if (m_selected_track && ImGui::BeginPopup("TrackDetailPopup")) {
		if (ImGui::MenuItem("RemoveTrack")) {
			for (auto it = animation->Tracks.begin(); it < animation->Tracks.end(); ++it) {
				if (m_selected_track->Property == it->Property) {
					animation->Tracks.erase(it);
					m_selected_track = nullptr;
					break;
				}
			}
		}
		ImGui::EndPopup();
	}
}

void AnimationEditorPanel::DrawKeyframeDetailsPanel(const Ref<Animation>& animation) {
	if (!m_selected_keyframe || !m_selected_track) {
		return;
	}

	ImGuiUtils::PrefixLabel("Time");
	ImGui::DragFloat("##Time", &m_selected_keyframe->Time, 0.01f, 0.0f, animation->Duration, "%.2f");
	ImGuiUtils::PrefixLabel("Value");

	switch (m_selected_track->Property) {
		case TrackProperty::NONE:
			break;
		case TrackProperty::PositionX:
		case TrackProperty::PositionY:
		case TrackProperty::PositionZ:
		case TrackProperty::ScaleX:
		case TrackProperty::ScaleY:
		case TrackProperty::ColorR:
		case TrackProperty::ColorG:
		case TrackProperty::ColorB:
		case TrackProperty::ColorA:
		case TrackProperty::Rotation:
		case TrackProperty::CameraSize:
			ImGui::DragFloat("##Value", &std::get<float>(m_selected_keyframe->Value), 0.01f);
			break;
		case TrackProperty::Position:
		case TrackProperty::Scale:
			ImGui::DragFloat3("##Value", glm::value_ptr(std::get<glm::vec3>(m_selected_keyframe->Value)), 0.01f);
			break;
		case TrackProperty::Color:
			ImGui::DragFloat4("##Value", glm::value_ptr(std::get<glm::vec4>(m_selected_keyframe->Value)), 0.01f);
			break;
	}

	if (ImGui::Button("Remove Keyframe", ImVec2(-1,0))) {
		m_selected_track->Keyframes.erase(
			std::remove_if(m_selected_track->Keyframes.begin(), m_selected_track->Keyframes.end(),
				  [&](const Keyframe& kf) { return &kf == m_selected_keyframe; }),
			m_selected_track->Keyframes.end()
		);
		m_selected_keyframe = nullptr;
	}
}

}

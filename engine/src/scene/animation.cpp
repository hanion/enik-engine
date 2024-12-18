#include "animation.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "tween.h"


namespace Enik {

const std::map<std::string, TrackProperty> StringToTrackProperty = {
	{"Position", TrackProperty::Position},
	{"Rotation", TrackProperty::Rotation},
	{"Scale", TrackProperty::Scale},
	{"Color", TrackProperty::Color},
	{"PositionX", TrackProperty::PositionX},
	{"PositionY", TrackProperty::PositionY},
	{"PositionZ", TrackProperty::PositionZ},
	{"ScaleX", TrackProperty::ScaleX},
	{"ScaleY", TrackProperty::ScaleY},
	{"ColorR", TrackProperty::ColorR},
	{"ColorG", TrackProperty::ColorG},
	{"ColorB", TrackProperty::ColorB},
	{"ColorA", TrackProperty::ColorA}
};


void Keyframe::InitializeZero(TrackProperty property) {
	switch (property) {
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
			Value = 0.0f;
			break;
		case TrackProperty::Position:
		case TrackProperty::Scale:
			Value = glm::vec3(0);
			break;
		case TrackProperty::Color:
			Value = glm::vec4(0);
			break;
	}
}


const std::string Animation::TrackPropertyToString(TrackProperty property) {
	for (const auto& pair : StringToTrackProperty) {
		if (pair.second == property) {
			return pair.first;
		}
	}
	return "Unknown";
}
TrackProperty Animation::TrackPropertyFromString(const std::string& str) {
	auto it = StringToTrackProperty.find(str);
	if (it != StringToTrackProperty.end()) {
		return it->second;
	}
	return TrackProperty::NONE;
}


bool Animation::HasProperty(const Entity& entity, const TrackProperty& property) {
	switch (property) {
		case TrackProperty::NONE:
		case TrackProperty::Position:
		case TrackProperty::Rotation:
		case TrackProperty::Scale:
		case TrackProperty::PositionX:
		case TrackProperty::PositionY:
		case TrackProperty::PositionZ:
		case TrackProperty::ScaleX:
		case TrackProperty::ScaleY:
			return (entity.Has<Component::Transform>());
		case TrackProperty::Color:
		case TrackProperty::ColorR:
		case TrackProperty::ColorG:
		case TrackProperty::ColorB:
		case TrackProperty::ColorA:
			return (entity.Has<Component::SpriteRenderer>());
		
	}
	return false;
}

void Animation::ApplyPropertyValue(const Entity& entity, const TrackProperty& property, const AnimationKeyValue& value) {
	Component::Transform& transform = entity.Get<Component::Transform>();
	Component::SpriteRenderer* sprite = nullptr;
	if (entity.Has<Component::SpriteRenderer>()) {
		sprite = &entity.Get<Component::SpriteRenderer>();
	}

	switch (property) {
		case TrackProperty::NONE: break;
		case TrackProperty::Position:  transform.LocalPosition = std::get<glm::vec3>(value); break;
		case TrackProperty::PositionX: transform.LocalPosition.x = std::get<float>(value);   break;
		case TrackProperty::PositionY: transform.LocalPosition.y = std::get<float>(value);   break;
		case TrackProperty::PositionZ: transform.LocalPosition.z = std::get<float>(value);   break;

		case TrackProperty::Rotation: transform.LocalRotation = std::get<float>(value); break;

		case TrackProperty::Scale:  transform.LocalScale = std::get<glm::vec3>(value); break;
		case TrackProperty::ScaleX: transform.LocalScale.x = std::get<float>(value);   break;
		case TrackProperty::ScaleY: transform.LocalScale.y = std::get<float>(value);   break;

		case TrackProperty::Color:  if(!sprite){break;} sprite->Color = std::get<glm::vec4>(value); break;
		case TrackProperty::ColorR: if(!sprite){break;} sprite->Color.r = std::get<float>(value);   break;
		case TrackProperty::ColorG: if(!sprite){break;} sprite->Color.g = std::get<float>(value);   break;
		case TrackProperty::ColorB: if(!sprite){break;} sprite->Color.b = std::get<float>(value);   break;
		case TrackProperty::ColorA: if(!sprite){break;} sprite->Color.a = std::get<float>(value);   break;
	}
}





float Animation::ease(float t, EASE easing_function){
	switch (easing_function) {
		case EASE::NONE: return t;
	}
	EN_CORE_ERROR("Animation::ease() unreachable");
	return t;
}

AnimationKeyValue Animation::lerp(AnimationKeyValue a, AnimationKeyValue b, float t) {
	if (std::holds_alternative<float>(a) && std::holds_alternative<float>(b)) {
		return std::get<float>(a) + t * (std::get<float>(b) - std::get<float>(a));
	}

	if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
		float a_float = static_cast<float>(std::get<int>(a));
		float b_float = static_cast<float>(std::get<int>(b));
		return a_float + t * (b_float - a_float);
	}

	if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
		return t >= 1.0f ? std::get<bool>(b) : std::get<bool>(a);
	}

	EN_CORE_ERROR("Unsupported type for lerp operation");
	return 0;
}






void Animation::Update(const Entity& entity, float time) const {
	for (const auto& track : Tracks) {
		if (!HasProperty(entity, track.Property)) {
			continue;
		}

		const Keyframe* curr_kf = nullptr;
		const Keyframe* next_kf = nullptr;

		for (size_t i = 0; i < track.Keyframes.size() - 1; ++i) {
			if (time >= track.Keyframes[i].Time && time < track.Keyframes[i + 1].Time) {
				curr_kf = &track.Keyframes[i];
				next_kf = &track.Keyframes[i + 1];
				break;
			}
		}

		if (!curr_kf || !next_kf) {
			continue;
		}

		float track_duration = next_kf->Time - curr_kf->Time;
		float t = (time - curr_kf->Time) / track_duration;

		// TODO: easing functions
		t = Animation::ease(t);

		auto interpolated_value = Animation::lerp(curr_kf->Value, next_kf->Value, t);
		ApplyPropertyValue(entity, track.Property, interpolated_value);
	}
}

}
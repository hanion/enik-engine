#pragma once
#include "asset/asset.h"
#include "base.h"
#include "core/timestep.h"
#include <variant>
#include <glm/glm.hpp>
#include "pch.h"

namespace Enik {

class Entity;

enum class EASE {
	NONE = 0,
};
enum class TrackProperty {
	NONE,
	Position, Rotation, Scale, Color,

	PositionX, PositionY, PositionZ,
	ScaleX, ScaleY,
	ColorR, ColorG, ColorB, ColorA,

	CameraSize,
};
extern const std::map<std::string, TrackProperty> StringToTrackProperty;
using AnimationKeyValue = std::variant<float, int, bool, std::string, glm::vec2, glm::vec3, glm::vec4>;




struct Keyframe {
	float Time = 0.0f;
	AnimationKeyValue Value = 0.0f;

	void InitializeZero(TrackProperty property);
};

struct Track {
	TrackProperty Property;
	std::vector<Keyframe> Keyframes;

	void SortKeyframes() {
		std::sort(Keyframes.begin(), Keyframes.end(), [](const Keyframe& a, const Keyframe& b) {
			return a.Time < b.Time;
		});
	}
};

// NOTE: does not hold state, just asset data, shared
class Animation : public Asset {
public:
	static AssetType GetStaticType() { return AssetType::Animation; }
	virtual AssetType GetType() const override { return GetStaticType(); }
public:
	std::vector<Track> Tracks = {};
	std::string Name = "";
	float Duration = 0;
 	bool Looping = false;

public:
	void SortKeyframes() {
		for (Track& track : Tracks) {
			track.SortKeyframes();
		}
	}
	void Update(const Entity& entity, float time) const;
	static float ease(float t, EASE easing_function = EASE::NONE);
	static AnimationKeyValue lerp(AnimationKeyValue a, AnimationKeyValue b, float t);

	
	static const std::string TrackPropertyToString(TrackProperty property);
	static TrackProperty TrackPropertyFromString(const std::string& str);

private:
	static bool HasProperty(const Entity& entity, const TrackProperty& property);
	static void ApplyPropertyValue(const Entity& entity, const TrackProperty& property, const AnimationKeyValue& value);


};



}
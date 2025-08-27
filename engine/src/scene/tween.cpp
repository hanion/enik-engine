#include "tween.h"


// TODO: Tween::Stack(<same as Create>);
// but it hold them at a different location, s_Data.Stack
// it finishes them one by one
// just like godot await, or yield
// this could be very useful

namespace Enik {


struct tween {
	tween(float* value, float start_value, float end_value, float duration, float creation_time, const std::function<void()>* call_on_end = nullptr)
		: Value(value), StartValue(start_value), EndValue(end_value), Duration(duration), CreationTime(creation_time), CallOnEnd(call_on_end) {
	}
	float* Value;
	float StartValue;
	float EndValue;
	float Duration;
	float CreationTime;
	const std::function<void()>* CallOnEnd;
};


struct TweenData {
	float ElapsedTime = 0.0f;
	std::vector<tween> Tweens;
};

static TweenData s_Data;


void Tween::Create(float* value, float end_value, float duration) {
	if (not value) {
		EN_CORE_ERROR("Tween value can not be nullptr!");
		return;
	}
	Create(value, *value, end_value, duration);
}

void Tween::Create(float* value, float start_value, float end_value, float duration) {
	if (not value) {
		EN_CORE_ERROR("Tween value can not be nullptr!");
		return;
	}
	// if (start_value == end_value) {
	// 	return;
	// }

	tween tw = tween(value, start_value, end_value, duration, s_Data.ElapsedTime);
	s_Data.Tweens.push_back(tw);
}

void Tween::Create(float* value, float end_value, float duration, const std::function<void()>* call_on_end) {
	if (not value) {
		EN_CORE_ERROR("Tween value can not be nullptr!");
		return;
	}
	// if (*value == end_value) {
	// 	return;
	// }

	tween tw = tween(value, *value, end_value, duration, s_Data.ElapsedTime, call_on_end);
	s_Data.Tweens.push_back(tw);
}


inline float lerp(float a, float b, float t) {
	return a + t * (b - a);
}


void Tween::StepAll(Timestep ts) {
	if (s_Data.Tweens.empty()) {
		s_Data.ElapsedTime = 0.0f;
		return;
	}

	s_Data.ElapsedTime += ts;

	auto it = s_Data.Tweens.begin();
	while (it != s_Data.Tweens.end()) {
		tween& tw = *it;

		if (not tw.Value) {
			it = s_Data.Tweens.erase(it);
			continue;
		}

		float elapsed_time_since_creation = (s_Data.ElapsedTime - tw.CreationTime);

		float t = std::min(elapsed_time_since_creation / tw.Duration, 1.0f);
		*(tw.Value) = lerp(tw.StartValue, tw.EndValue, t);


		if (elapsed_time_since_creation > tw.Duration) {
			it = s_Data.Tweens.erase(it);
			if (tw.CallOnEnd) {
				(*tw.CallOnEnd)();
			}
		} else {
			++it;
		}
	}

}

void Tween::ResetData() {
	s_Data.Tweens.clear();
	s_Data.ElapsedTime = 0.0f;
}


}
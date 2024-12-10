#pragma once
#include "base.h"
#include "core/timestep.h"


namespace Enik {

class Tween {
public:
	static void Create(float* value, float end_value, float duration);
	static void Create(float* value, float start_value, float end_value, float duration);
	static void Create(float* value, float end_value, float duration, const std::function<void()>* call_on_end);

	static void StepAll(Timestep ts);

	static void ResetData();
};

}
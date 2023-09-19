#pragma once
#include "scene/entity.h"
#include "scene/components.h"

namespace Enik {

extern "C" const float GetFixedUpdateRate();

extern "C" Entity FindEntityByUUID(UUID uuid);

extern "C" Entity FindEntityByName(const std::string& name);

template <typename T>
T* GetScriptInstance(Entity entity) {
	return (T*)entity.Get<Component::NativeScript>().Instance;
}

extern "C" void InspectorExportFloat(const std::string& text, float& value,
	float v_speed = 0.1f, float v_min = 0.0f, float v_max = 0.0f,
	const char *format = "%.2f");

extern "C" void InspectorExportFloat2(const std::string& text, float v[2],
	float v_speed = 0.1f, float v_min = 0.0f, float v_max = 0.0f,
	const char *format = "%.2f");

extern "C" void InspectorExportFloat3(const std::string& text, float v[3],
	float v_speed = 0.1f, float v_min = 0.0f, float v_max = 0.0f,
	const char *format = "%.2f");

extern "C" void InspectorExportButton(const std::string& text, const std::function<void()>& lambda);

}
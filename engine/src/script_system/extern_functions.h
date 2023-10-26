#pragma once
#include "scene/entity.h"
#include "scene/components.h"

namespace Enik {

extern "C" const float GetFixedUpdateRate();

extern "C" Entity FindEntityByUUID(UUID uuid);

extern "C" Entity FindEntityByName(const std::string& name);

template <typename T>
T* GetScriptInstance(Entity entity) {
	return (T*)entity.GetScriptInstance();
}

}
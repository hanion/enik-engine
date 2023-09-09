#pragma once
#include "scene/entity.h"
#include "scene/components.h"

namespace Enik {

extern "C" Entity FindEntityByUUID(UUID uuid);

extern "C" Entity FindEntityByName(const std::string& name);

}
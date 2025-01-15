#include "extern_functions.h"

#include "script_system/script_system.h"
#include "physics/physics.h"

namespace Enik {
const float GetFixedUpdateRate() {
    return PHYSICS_UPDATE_RATE;
}

Entity FindEntityByUUID(UUID uuid) {
	return ScriptSystem::GetSceneContext()->FindEntityByUUID(uuid);
}

Entity FindEntityByName(const std::string& name) {
    return ScriptSystem::GetSceneContext()->FindEntityByName(name);
}


}
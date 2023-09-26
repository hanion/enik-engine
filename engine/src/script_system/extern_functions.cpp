#include "extern_functions.h"

#include "script_system/script_system.h"
#include "../imgui_utils.h"
#include "physics/physics_world.h"

namespace Enik {
const float GetFixedUpdateRate() {
    return PhysicsWorld::GetFixedUpdateRate();
}

Entity FindEntityByUUID(UUID uuid) {
	return ScriptSystem::GetSceneContext()->FindEntityByUUID(uuid);
}

Entity FindEntityByName(const std::string& name) {
    return ScriptSystem::GetSceneContext()->FindEntityByName(name);
}


}
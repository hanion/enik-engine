#include "extern_functions.h"

#include "script_system/script_system.h"

namespace Enik {

Entity FindEntityByUUID(UUID uuid) {
	return ScriptSystem::GetSceneContext()->FindEntityByUUID(uuid);
}

Entity FindEntityByName(const std::string& name) {
    return ScriptSystem::GetSceneContext()->FindEntityByName(name);
}

}
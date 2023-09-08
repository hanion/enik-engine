#pragma once
#include <dlfcn.h>

#include "scene/scriptable_entity.h"
#include "script_system/script_registry.h"
#include "project/project.h"

namespace Enik {

class ScriptSystem {
public:

	static void LoadScriptModule();
	static void UnloadScriptModule();

};

}
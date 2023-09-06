#include "script_registry.h"

namespace Enik {

static std::unordered_map<std::string, ScriptableEntity* (*)()> s_ScriptRegistry;

void ScriptRegistry::RegisterScriptClass(const std::string& class_name, ScriptableEntity* (*create_function)()) {
	s_ScriptRegistry[class_name] = create_function;
}

std::unordered_map<std::string, ScriptableEntity* (*)()>& ScriptRegistry::GetRegistry() {
	return s_ScriptRegistry;
}

}
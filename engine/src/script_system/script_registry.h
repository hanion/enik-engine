#pragma once
#include "scene/scriptable_entity.h"

namespace Enik {

namespace ScriptRegistry {
	extern "C" void RegisterScriptClass(const std::string& class_name, ScriptableEntity* (*create_function)());
	extern "C" std::unordered_map<std::string, ScriptableEntity* (*)()>& GetRegistry();
}

}
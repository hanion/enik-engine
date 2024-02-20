#pragma once
#include "scene/scriptable_entity.h"

namespace Enik {

namespace ScriptRegistry {
	EN_API extern "C" void RegisterScriptClass(const std::string& class_name, ScriptableEntity* (*create_function)());
	EN_API std::unordered_map<std::string, ScriptableEntity* (*)()>& GetRegistry();
	EN_API void ClearRegistry();
}

}
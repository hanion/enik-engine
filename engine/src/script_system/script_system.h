#pragma once
#include <dlfcn.h>

#include "scene/scriptable_entity.h"
#include "script_system/script_registry.h"
#include "project/project.h"

namespace Enik {

// same signature as in the script module lib
typedef void (*register_all_fn)();
static register_all_fn register_all;
static void* script_module_handle;

class ScriptSystem {
public:

static void LoadScriptModule() {
	auto lib = Project::GetAbsolutePath(Project::GetActive()->GetConfig().script_module_path);

	if (lib.empty() or not std::filesystem::exists(lib)) {
		EN_CORE_ERROR("Cannot open script module: Invalid Path '{0}'", lib.c_str());
		return;
	}

	// open the library
	script_module_handle = dlopen(lib.c_str(), RTLD_LAZY | RTLD_GLOBAL);

	if (not script_module_handle) {
		EN_CORE_ERROR("Cannot open script module: {0}", dlerror());
		return;
	}

	// reset errors
	dlerror();

	// load the symbol
	register_all = reinterpret_cast<register_all_fn>(dlsym(script_module_handle, "RegisterAllScripts"));

	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		EN_CORE_ERROR("Cannot load symbol 'RegisterAllScripts': {0}", dlsym_error);
		dlclose(script_module_handle);
		return;
	}

	if (register_all and register_all != nullptr) {
		register_all();
	}

	// close the library
	if (script_module_handle and script_module_handle != nullptr) {
		dlclose(script_module_handle);
	}

	EN_CORE_INFO("Loaded script module '{0}'", lib.c_str());
}

};

}
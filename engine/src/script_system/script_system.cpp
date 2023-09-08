#include "script_system.h"

namespace Enik {

// same signature as in the script module lib
typedef void (*register_all_fn)();
static register_all_fn register_all;
static void* script_module_handle;

void ScriptSystem::LoadScriptModule() {
	UnloadScriptModule();
	ScriptRegistry::ClearRegistry();

	auto lib = Project::GetAbsolutePath(Project::GetActive()->GetConfig().script_module_path);

	if (lib.empty() or not std::filesystem::exists(lib)) {
		EN_CORE_ERROR("Error while opening script module: Invalid Path\n    '{0}'", lib.c_str());
		return;
	}

	// open the library
	script_module_handle = dlopen(lib.c_str(), RTLD_LAZY);

	const char* dl_error = dlerror();
	if (not script_module_handle or script_module_handle == nullptr or dl_error) {
		EN_CORE_ERROR("Error while opening script module:\n    {0}", dl_error);
		return;
	}

	// load the symbol
	void* symbol = dlsym(script_module_handle, "RegisterAllScripts");

	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		EN_CORE_ERROR("Error while loading symbol 'RegisterAllScripts':\n    {0}", dlsym_error);
		dlclose(script_module_handle);
		return;
	}

	if (symbol and symbol != nullptr) {
		register_all = reinterpret_cast<register_all_fn>(symbol);
		if (register_all and register_all != nullptr) {
			register_all();
		}
		EN_CORE_INFO("Loaded script module '{0}'", lib.c_str());
	}
}

void ScriptSystem::UnloadScriptModule() {
	// close the library
	if (script_module_handle and script_module_handle != nullptr) {
		if (dlclose(script_module_handle) != 0) {
			const char* dlclose_error = dlerror();
			if (dlclose_error) {
				EN_CORE_ERROR("Error while unloading script module:\n    {0}", dlclose_error);
			}
		}

		script_module_handle = nullptr;
		register_all = nullptr;

		EN_CORE_INFO("Unloaded script module");
	}
}

}
#include "script_system.h"
#include "core/application.h"

namespace Enik {

static ScriptSystem::ScriptSystemData s_Data;

// same signature as in the script module lib
typedef void (*register_all_fn)();
static register_all_fn register_all;
static void* script_module_handle;

void ScriptSystem::LoadScriptModuleFirstTime() {
	s_Data.reload_pending = true;
	ReloadScriptModule();

	auto sm_path = Project::GetAbsolutePath(Project::GetActive()->GetConfig().script_module_path);
	s_Data.file_watcher = CreateScope<filewatch::FileWatch<std::string>>(sm_path, OnFileWatcherEvent);
}

void ScriptSystem::ReloadScriptModule() {
	if (s_Data.reload_pending) {
		auto new_script_module_path = CopyScriptModule();
		if (not new_script_module_path.empty()) {
			UnloadScriptModule();
			ScriptRegistry::ClearRegistry();

			LoadScriptModule(new_script_module_path);
			s_Data.reload_pending = false;
		}
	}
}



void ScriptSystem::LoadScriptModule(const std::filesystem::path& script_module_path) {

	if (script_module_path.empty() or not std::filesystem::exists(script_module_path)) {
		EN_CORE_ERROR("Error while opening script module: Invalid Path\n    '{0}'", script_module_path.c_str());
		return;
	}

	// open the library
	script_module_handle = dlopen(script_module_path.c_str(), RTLD_LAZY);

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
		s_Data.current_script_module_path = script_module_path;
		EN_CORE_INFO("Loaded script module '{0}'", script_module_path.c_str());
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

		if (not s_Data.current_script_module_path.empty() and std::filesystem::exists(s_Data.current_script_module_path)) {
			std::filesystem::remove(s_Data.current_script_module_path);
		}
	}
}

void ScriptSystem::SetSceneContext(Scene* scene) {
	s_Data.scene_context = scene;
}
Scene* ScriptSystem::GetSceneContext() {
    return s_Data.scene_context;
}

void ScriptSystem::OnFileWatcherEvent(const std::string& path, const filewatch::Event change_type) {
	switch (change_type) {
		case filewatch::Event::modified:
			if (not s_Data.reload_pending) {
				s_Data.reload_pending = true;
				Application::Get().SubmitToMainThread([](){
					ReloadScriptModule();
				});
			}
			break;
		case filewatch::Event::removed:
			if (not s_Data.reload_pending) {
				Application::Get().SubmitToMainThread([](){
					UnloadScriptModule();
				});
			}
			break;
		default:
			break;
	}
}

std::filesystem::path ScriptSystem::CopyScriptModule() {
	auto new_path = std::filesystem::temp_directory_path() / ("enik_script_module_" + std::to_string(UUID()));

	std::error_code error_code;
	if (std::filesystem::copy_file(Project::GetAbsolutePath(Project::GetActive()->GetConfig().script_module_path), new_path, error_code)) {
		return new_path;
	}

	EN_CORE_ERROR("Error while copying script module {0}", error_code.message());

    return std::filesystem::path();
}

}
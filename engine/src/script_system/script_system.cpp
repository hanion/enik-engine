#include "script_system.h"
#include "core/application.h"
#include "script_system/script_registry.h"
#include "project/project.h"

#if EN_STATIC_SCRIPT_MODULE
extern "C" void RegisterAllScripts();
#endif

#ifdef EN_PLATFORM_LINUX
#include <dlfcn.h>
#elif defined(EN_PLATFORM_WINDOWS)
#include <windows.h>
#endif

namespace Enik {

static ScriptSystem::ScriptSystemData s_Data;

#if !EN_STATIC_SCRIPT_MODULE
// same signature as in the script module lib
typedef void (*register_all_fn)();
static register_all_fn register_all;
static void* script_module_handle;
#endif

void ScriptSystem::LoadScriptModuleFirstTime() {
#if EN_STATIC_SCRIPT_MODULE
	RegisterAllScripts();
#else
	if (Project::GetActive()->GetConfig().script_module_path.empty()) {
		return;
	}
	auto sm_path = Project::GetAbsolutePath(Project::GetActive()->GetConfig().script_module_path);
	if (sm_path.empty()) {
		EN_CORE_ERROR("Script module not found: '{}'", Project::GetActive()->GetConfig().script_module_path.string().c_str());
		EN_CORE_ERROR("\tDid you forget to build?");
		auto buildsh = Project::GetActive()->GetProjectDirectory() / "build.sh";
		EN_CORE_ERROR("\tYou can build it by running the build script '{}'", buildsh.string().c_str());
		return;
	}

	s_Data.reload_pending = true;
	ReloadScriptModule();

	s_Data.file_watcher = CreateScope<filewatch::FileWatch<std::string>>(sm_path.string(), OnFileWatcherEvent);
#endif
}

void ScriptSystem::ReloadScriptModule() {
	if (s_Data.reload_pending) {
		auto new_script_module_path = CopyScriptModule();
		if (not new_script_module_path.empty()) {
			UnloadScriptModule();
			ScriptRegistry::ClearRegistry();

			LoadScriptModule(new_script_module_path);

			for (auto& function : s_Data.OnScriptModuleReloadEvents) {
				function();
			}

			s_Data.reload_pending = false;
		}
	}
}



void ScriptSystem::LoadScriptModule(const std::filesystem::path& script_module_path) {
#if !EN_STATIC_SCRIPT_MODULE

	if (script_module_path.empty() or not std::filesystem::exists(script_module_path)) {
		EN_CORE_ERROR("Error while opening script module: Invalid Path\n    '{0}'", script_module_path.string().c_str());
		return;
	}

	// open the library
#ifdef EN_PLATFORM_LINUX
	script_module_handle = dlopen(script_module_path.c_str(), RTLD_LAZY);

	const char* dl_error = dlerror();
	if (not script_module_handle or script_module_handle == nullptr or dl_error) {
		EN_CORE_ERROR("Error while opening script module:\n    {0}", dl_error);
		return;
	}
#elif defined(EN_PLATFORM_WINDOWS)
	script_module_handle = LoadLibraryA(script_module_path.string().c_str());
#endif

	// load the symbol
	void* symbol;
#ifdef EN_PLATFORM_LINUX
	symbol = dlsym(script_module_handle, "RegisterAllScripts");

	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		EN_CORE_ERROR("Error while loading symbol 'RegisterAllScripts':\n    {0}", dlsym_error);
		dlclose(script_module_handle);
		return;
	}
#elif defined(EN_PLATFORM_WINDOWS)
	symbol = reinterpret_cast<void*>(GetProcAddress((HMODULE)script_module_handle, "RegisterAllScripts"));
#endif

	if (symbol and symbol != nullptr) {
		register_all = reinterpret_cast<register_all_fn>(symbol);
		if (register_all and register_all != nullptr) {
			register_all();
		}
		s_Data.current_script_module_path = script_module_path;
		EN_CORE_INFO("Loaded script module '{0}'", script_module_path.string().c_str());
	}
#endif
}

void ScriptSystem::UnloadScriptModule() {
#if !EN_STATIC_SCRIPT_MODULE
	// close the library
	if (script_module_handle and script_module_handle != nullptr) {
#ifdef EN_PLATFORM_LINUX
		if (dlclose(script_module_handle) != 0) {
			const char* dlclose_error = dlerror();
			if (dlclose_error) {
				EN_CORE_ERROR("Error while unloading script module:\n    {0}", dlclose_error);
			}
		}
#elif defined(EN_PLATFORM_WINDOWS)
		FreeLibrary((HMODULE)script_module_handle);
#endif

		script_module_handle = nullptr;
		register_all = nullptr;

		if (not s_Data.current_script_module_path.empty() and
			std::filesystem::exists(s_Data.current_script_module_path)) {
			std::filesystem::remove(s_Data.current_script_module_path);
		}
	}
#endif
}

void ScriptSystem::ClearOnScriptModuleReloadEvents() {
    s_Data.OnScriptModuleReloadEvents.clear();
}

void ScriptSystem::CallOnScriptModuleReload(const std::function<void()>& function) {
    s_Data.OnScriptModuleReloadEvents.emplace_back(function);
}

void ScriptSystem::SetSceneContext(Scene* scene) {
	s_Data.scene_context = scene;
}
Scene* ScriptSystem::GetSceneContext() {
    return s_Data.scene_context;
}
Physics& ScriptSystem::GetPhysicsContext() {
	return GetSceneContext()->m_Physics;
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

	bool copy_result = std::filesystem::copy_file(
		Project::GetAbsolutePath(Project::GetActive()->GetConfig().script_module_path),
		new_path,
		error_code
	);

	if (copy_result) {
		return new_path;
	}

	EN_CORE_ERROR("Error while copying script module {0}", error_code.message());

    return std::filesystem::path();
}

}
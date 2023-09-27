#pragma once
#include <dlfcn.h>

#include "scene/scriptable_entity.h"
#include "script_system/script_registry.h"
#include "project/project.h"
#include "filewatch/FileWatch.hpp"
#include "scene/scene.h"

namespace Enik {

class ScriptSystem {
public:

	static void LoadScriptModuleFirstTime();
	static void ReloadScriptModule();
	static void UnloadScriptModule();

	static void ClearOnScriptModuleReloadEvents();
	static void CallOnScriptModuleReload(const std::function<void()>& function);

	static void SetSceneContext(Scene* scene);
	static Scene* GetSceneContext();

	struct ScriptSystemData {
		Scope<filewatch::FileWatch<std::string>> file_watcher;
		std::filesystem::path current_script_module_path;
		bool reload_pending = false;

		Scene* scene_context;

		std::vector<std::function<void ()>> OnScriptModuleReloadEvents;
	};

private:
	static void LoadScriptModule(const std::filesystem::path& script_module_path);

	static void OnFileWatcherEvent(const std::string& path, const filewatch::Event change_type);

	static std::filesystem::path CopyScriptModule();

};

}
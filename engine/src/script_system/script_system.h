#pragma once
#include <dlfcn.h>

#include "scene/scriptable_entity.h"
#include "script_system/script_registry.h"
#include "project/project.h"
#include "filewatch/FileWatch.hpp"

namespace Enik {

class ScriptSystem {
public:

	static void LoadScriptModuleFirstTime();
	static void ReloadScriptModule();
	static void UnloadScriptModule();

	struct ScriptSystemData {
		Scope<filewatch::FileWatch<std::string>> file_watcher;
		std::filesystem::path current_script_module_path;
		bool reload_pending = false;
	};

private:
	static void LoadScriptModule(const std::filesystem::path& script_module_path);

	static void OnFileWatcherEvent(const std::string& path, const filewatch::Event change_type);

	static std::filesystem::path CopyScriptModule();

};

}
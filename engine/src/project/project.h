#pragma once

#include <base.h>
#include <memory>
#include <string>
#include <filesystem>
#include "asset/asset_manager_base.h"
#include "asset/asset_manager_editor.h"
#include "core/asserter.h"

namespace Enik {

struct ProjectConfig {
	std::string project_name = "untitled project";
	std::filesystem::path start_scene;
	std::filesystem::path asset_registry_path;
	std::filesystem::path script_module_path;
	std::vector<std::filesystem::path> autoloads;
	std::vector<std::filesystem::path> open_assets;
};

class Project {
public:

	static const std::filesystem::path GetProjectDirectory() {
		if (not s_ActiveProject) {
			return std::filesystem::path();
		}
		return s_ActiveProject->m_ProjectDirectory;
	}

	static const std::filesystem::path GetEngineDirectory() {
		if (s_EngineSourcePath.empty()) FindEngineSourcePath();
		return s_EngineSourcePath;
	}

	static const std::filesystem::path GetAssetRegistryPath() {
		EN_CORE_ASSERT(s_ActiveProject);
		return GetProjectDirectory() / GetActive()->m_Config.asset_registry_path;
	}

	static const std::filesystem::path GetAbsolutePath(const std::filesystem::path& path) {
		try {
			std::filesystem::path connected = GetProjectDirectory() / path;
			if (std::filesystem::exists(connected)) {
				std::filesystem::path absolute_path = std::filesystem::canonical(connected);
				if (std::filesystem::exists(absolute_path)) {
					return absolute_path;
				}
			}
		}
		catch(const std::exception& e) {
			EN_CORE_ERROR("Could not GetAbsolutePath of {}:\n        {}", path.string(), e.what());
		}

		return std::filesystem::path();
	}

	static const std::filesystem::path GetRelativePath(const std::filesystem::path& path) {
		return GetRelativePath(path.string());
	}
	static const std::filesystem::path GetRelativePath(const std::string& path) {
		std::filesystem::path canonical = std::filesystem::absolute(path);
		std::filesystem::path base      = std::filesystem::absolute(GetProjectDirectory());

		return std::filesystem::relative(canonical, base);
	}

	ProjectConfig& GetConfig() { return m_Config; }

	static Ref<Project> GetActive() { return s_ActiveProject; }
	static Ref<AssetManagerBase>   GetAssetManager() { return GetActive()->m_AssetManager; }
	static Ref<AssetManagerEditor> GetAssetManagerEditor() { return std::static_pointer_cast<AssetManagerEditor>(GetActive()->m_AssetManager); }

	static Ref<Project> New();
	static Ref<Project> Load(const std::filesystem::path& path);
	static void         Save(const std::filesystem::path& path);


	static std::filesystem::path FindAssetPath(const std::filesystem::path& path);

	static void FindEngineSourcePath();
private:

	ProjectConfig m_Config;

	std::filesystem::path m_ProjectDirectory;

	Ref<AssetManagerBase> m_AssetManager;

	inline static Ref<Project> s_ActiveProject;

	static std::filesystem::path s_EngineSourcePath;
	static std::filesystem::path s_AssetsPath;
};

}
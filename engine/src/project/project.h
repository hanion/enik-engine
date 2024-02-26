#pragma once

#include <base.h>
#include <string>
#include <filesystem>

namespace Enik {

struct ProjectConfig {
	std::string project_name = "untitled project";
	std::filesystem::path start_scene;
	std::filesystem::path script_module_path;
};

class EN_API Project {
public:

	static const std::filesystem::path GetProjectDirectory() {
		if (not s_ActiveProject) {
			return std::filesystem::path();
		}
		return s_ActiveProject->m_ProjectDirectory;
	}

	static const std::filesystem::path GetAbsolutePath(const std::filesystem::path& path) {
		try {
			std::filesystem::path absolute_path = std::filesystem::canonical(GetProjectDirectory() / path);
			if (std::filesystem::exists(absolute_path)) {
				return absolute_path;
			}
		}
		catch(const std::exception& e) {
			EN_CORE_ERROR("Could not GetAbsolutePath of {0}:\n        {1}", path, e.what());
		}

		return std::filesystem::path();
	}

	ProjectConfig& GetConfig() { return m_Config; }

	static Ref<Project> GetActive() { return s_ActiveProject; }

	static Ref<Project> New();
	static Ref<Project> Load(const std::filesystem::path& path);
	static void         Save(const std::filesystem::path& path);

private:
	ProjectConfig m_Config;

	std::filesystem::path m_ProjectDirectory;

	inline static Ref<Project> s_ActiveProject;

};

}
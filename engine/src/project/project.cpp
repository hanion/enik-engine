#include "project.h"
#include "project/project_serializer.h"
#include <filesystem>

namespace Enik {

std::filesystem::path Project::s_EngineSourcePath = "";
std::filesystem::path Project::s_AssetsPath = "";

Ref<Project> Project::New() {
	FindEngineSourcePath();
	s_ActiveProject = CreateRef<Project>();
	return s_ActiveProject;
}

Ref<Project> Project::Load(const std::filesystem::path& path) {
	FindEngineSourcePath();
	Ref<Project> project = CreateRef<Project>();

	ProjectSerializer serializer = ProjectSerializer(project);
	if (serializer.Deserialize(path)) {
		project->m_ProjectDirectory = std::filesystem::canonical(path.parent_path());
		project->m_AssetManager = CreateRef<AssetManagerEditor>();
		s_ActiveProject = project;
		GetAssetManagerEditor()->DeserializeAssetRegistry();
		return s_ActiveProject;
	}

	return nullptr;
}
void Project::Save(const std::filesystem::path& path) {
	ProjectSerializer serializer = ProjectSerializer(s_ActiveProject);
	s_ActiveProject->m_ProjectDirectory = std::filesystem::canonical(path.parent_path());
	serializer.Serialize(path);
}

std::filesystem::path Project::FindAssetPath(const std::filesystem::path& path) {
	if (s_EngineSourcePath.empty()) FindEngineSourcePath();

	std::filesystem::path res = s_AssetsPath / path;
	if (std::filesystem::exists(res)) {
		return res;
	}

	EN_CORE_WARN("Asset could not be found. '{}'", res.string().c_str());
	return "";
}

void Project::FindEngineSourcePath() {
	if (!s_EngineSourcePath.empty()) return;

	if (std::filesystem::exists("./assets")) {
		s_EngineSourcePath = "./";
		s_AssetsPath = "assets";
		return;
	}

	std::string source = "./";
	const std::string assets = "editor/assets";
	for (size_t i = 0; i < 5; ++i) {
		if (std::filesystem::exists(source + assets)) {
			s_EngineSourcePath = source;
			s_AssetsPath = source + assets;
			return;
		}
		source = source + "../";
	}

	EN_CORE_ERROR("Engine source path not found. Please run the editor from its build directory!");
	exit(1); // not much we can do
}

}

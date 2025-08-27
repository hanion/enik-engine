#include <pch.h>
#include "project_serializer.h"


#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Enik {

ProjectSerializer::ProjectSerializer(Ref<Project>& project)
	: m_Project(project) {

}

void ProjectSerializer::Serialize(std::filesystem::path path) {
	const auto& config = m_Project->GetConfig();

	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Project" << YAML::Value << config.project_name;
	out << YAML::Key << "StartScene" << YAML::Value << config.start_scene.string();

	if (not config.script_module_path.empty()) {
		out << YAML::Key << "ScriptModule" << YAML::Value << config.script_module_path.string();
	}
	out << YAML::Key << "AssetRegistry" << YAML::Value << config.asset_registry_path.string();


	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();
	EN_CORE_INFO("Serialized project   '{}', in {}", config.project_name, std::filesystem::canonical(path));
}
bool ProjectSerializer::Deserialize(std::filesystem::path path) {
	path = std::filesystem::canonical(path);

	if (not path.has_extension() or path.extension() != ".enik") {
		EN_CORE_ERROR("Can not load file {}\n	{}", path.string(), "It needs to be a project.enik file!");
		return false;
	}

	YAML::Node data;
	try {
		data = YAML::LoadFile(path.string());
	}
	catch (const YAML::ParserException& e) {
		EN_CORE_ERROR("Failed to load project.enik file {}\n	{1}", path.string(), e.what());
		return false;
	}

	if (not data.IsMap() or not data["Project"] or not data["StartScene"]) {
		return false;
	}

	auto& config = m_Project->GetConfig();

	config.project_name = data["Project"].as<std::string>();
	config.start_scene  = data["StartScene"].as<std::string>();

	if (data["ScriptModule"]) {
		if (!data["ScriptModule"].IsNull() && data["ScriptModule"].IsDefined()) {
			config.script_module_path  = data["ScriptModule"].as<std::string>();
		}
	}

	if (data["AssetRegistry"]) {
		config.asset_registry_path  = data["AssetRegistry"].as<std::string>();
	}

	EN_CORE_INFO("Deserialized project '{}', in {}", config.project_name, path);

	return true;
}

}
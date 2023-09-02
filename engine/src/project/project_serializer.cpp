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


	EN_CORE_INFO("Serializing project   '{0}', in '{1}'", config.project_name, path);
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Project" << YAML::Value << config.project_name;
	out << YAML::Key << "StartScene" << YAML::Value << config.start_scene.string();

	out << YAML::EndMap;

	std::ofstream fout(path);
	fout << out.c_str();
}
bool ProjectSerializer::Deserialize(std::filesystem::path path) {
	YAML::Node data;
	try {
		data = YAML::LoadFile(path);
	}
	catch (const YAML::ParserException& e) {
		EN_CORE_ERROR("Failed to load project.enik file '{0}'\n	{1}", path, e.what());
		return false;
	}

	if (not data["Project"] or not data["StartScene"]) {
		return false;
	}

	auto& config = m_Project->GetConfig();

	config.project_name = data["Project"].as<std::string>();
	config.start_scene  = data["StartScene"].as<std::string>();

	EN_CORE_INFO("Deserializing project '{0}', in '{1}'", config.project_name, path);

	return true;
}

}
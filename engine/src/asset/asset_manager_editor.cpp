#include "asset_manager_editor.h"
#include "asset/asset.h"
#include "asset/asset_metadata.h"
#include "asset/importer/asset_importer.h"
#include "core/log.h"
#include "project/project.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <yaml-cpp/yaml.h>

namespace Enik {


static std::map<std::filesystem::path, AssetType> s_AssetToExtensionMap = {
	{ ".png", AssetType::Texture2D },
	{ ".jpg", AssetType::Texture2D },
	{ ".jpeg", AssetType::Texture2D },
	{ ".escn", AssetType::Scene },
	{ ".prefab", AssetType::Prefab },
	{ ".wav", AssetType::Audio },
	{ ".anim", AssetType::Animation },
	{ ".ttf", AssetType::Font },
};
static AssetType GetTypeFromExtension(const std::filesystem::path& ext) {
	if (s_AssetToExtensionMap.find(ext) != s_AssetToExtensionMap.end()) {
		return s_AssetToExtensionMap.at(ext);
	}
	EN_CORE_ERROR("Could not get asset type from extension '{}'", ext.string().c_str());
	return AssetType::None;
}

bool AssetManagerEditor::IsAssetHandleValid(AssetHandle handle) const {
	return handle!=0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
}

bool AssetManagerEditor::IsAssetLoaded(AssetHandle handle) const {
	return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
}

AssetType AssetManagerEditor::GetAssetType(AssetHandle handle) const {
	if (!IsAssetHandleValid(handle)) {
		return AssetType::None;
	}
	return m_AssetRegistry.at(handle).Type;
}

const AssetMetadata& AssetManagerEditor::GetMetadata(AssetHandle handle) const {
	auto it = m_AssetRegistry.find(handle);
	if (it == m_AssetRegistry.end()) {
		static AssetMetadata s_NullMetadata;
		return s_NullMetadata;
	}
	return it->second;
}

const std::filesystem::path& AssetManagerEditor::GetAssetPath(AssetHandle handle) const {
	return GetMetadata(handle).FilePath;
}


Ref<Asset> AssetManagerEditor::GetAsset(AssetHandle handle) {
	if (!IsAssetHandleValid(handle)) {
		return nullptr;
	}

	Ref<Asset> asset;
	if (IsAssetLoaded(handle)) {
		asset = m_LoadedAssets.at(handle);
	} else {
		const AssetMetadata& metadata = GetMetadata(handle);
		asset = AssetImporter::ImportAsset(handle, metadata);

		if (!asset) {
			EN_CORE_ERROR("Asset Import Failed! {}", metadata.FilePath.string().c_str());
		}

		m_LoadedAssets[handle] = asset;
	}

	return asset;
}



AssetHandle AssetManagerEditor::ImportAsset(const std::filesystem::path& path) {
	const std::filesystem::path absolute_path = std::filesystem::absolute(path);

	// NOTE: check if it already exists before creating new
	{
		for (const auto&[handle, metadata] : m_AssetRegistry) {
			if (metadata.FilePath == absolute_path) {
				return handle;
			}
		}
	}


	// generate new handle
	AssetHandle handle;
	AssetMetadata metadata;
	metadata.FilePath = absolute_path;

	if (path.has_extension()) {
		metadata.Type = GetTypeFromExtension(path.extension());
	}
	
	Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
	if (asset) {
		asset->Handle = handle;
		m_LoadedAssets[handle] = asset;
		m_AssetRegistry[handle] = metadata;
		return handle;
	}
	return 0;
}




void AssetManagerEditor::SerializeAssetRegistry() {
	std::filesystem::path path = Project::GetAssetRegistryPath();

	YAML::Emitter out;
	{
		out << YAML::BeginMap;
		out << YAML::Key << "AssetRegistry" << YAML::Value;
		out << YAML::BeginSeq;
		for (const auto&[handle, metadata] : m_AssetRegistry) {
			out << YAML::BeginMap;
			out << YAML::Key << "Handle" << YAML::Value << (uint64_t)handle;
			out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
			out << YAML::Key << "FilePath" << YAML::Value << Project::GetRelativePath(metadata.FilePath).generic_string();
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	std::ofstream fout(path);
	fout << out.c_str();
}

bool AssetManagerEditor::DeserializeAssetRegistry() {
	std::filesystem::path path = Project::GetAssetRegistryPath();
	if (not std::filesystem::exists(path)) {
		return false;
	}

	if (not path.has_extension() or path.extension() != ".registry") {
		EN_CORE_ERROR("Can not load file {}\n	{}", path.string(), "It needs to be a asset.registry file!");
		return false;
	}

	YAML::Node data;
	try {
		data = YAML::LoadFile(path.string());
	}
	catch (const YAML::ParserException& e) {
		EN_CORE_ERROR("Failed to load asset.registry file {}\n	{1}", path.string(), e.what());
		return false;
	}

	if (not data.IsMap() or not data["AssetRegistry"]) {
		return false;
	}

	auto root = data["AssetRegistry"];

	for (const auto& node : root) {
		AssetHandle handle = node["Handle"].as<uint64_t>();
		AssetMetadata& metadata =  m_AssetRegistry[handle];
		metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
		metadata.FilePath = Project::GetAbsolutePath(node["FilePath"].as<std::string>());
	}


	return true;
}

}

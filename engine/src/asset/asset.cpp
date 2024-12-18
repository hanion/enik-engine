#include "asset.h"
#include "core/log.h"

namespace Enik {

const char* AssetTypeToString(AssetType type)  {
	switch (type) {
		case AssetType::None:      return "None";
		case AssetType::Scene:     return "Scene";
		case AssetType::Prefab:    return "Prefab";
		case AssetType::Texture2D: return "Texture2D";
		case AssetType::Font:      return "Font";
		case AssetType::Audio:     return "Audio";
		case AssetType::Animation: return "Animation";
	}
	return "<Invalid>";
}

AssetType AssetTypeFromString(std::string type) {
	if (type == "None")      { return AssetType::None; }
	if (type == "Scene")     { return AssetType::Scene; }
	if (type == "Prefab")    { return AssetType::Prefab; }
	if (type == "Texture2D") { return AssetType::Texture2D; }
	if (type == "Font")      { return AssetType::Font; }
	if (type == "Audio")     { return AssetType::Audio; }
	if (type == "Animation") { return AssetType::Animation; }

	EN_CORE_ERROR("Invalid Asset Type");
	return AssetType::None;
}


}

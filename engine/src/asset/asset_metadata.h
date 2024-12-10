#pragma once
#include "asset/asset.h"

namespace Enik {

struct AssetMetadata {
	AssetType Type = AssetType::None;
	std::filesystem::path FilePath;
};

}


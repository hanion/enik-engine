#pragma once

#include "base.h"
#include "core/uuid.h"
#include <string_view>

namespace Enik {

using AssetHandle = UUID;

enum class AssetType : uint16_t {
	None = 0,
	Scene, Prefab,
	Texture2D,
	Font,
	Audio,
	Animation,
};

const char* AssetTypeToString(AssetType type);
AssetType AssetTypeFromString(std::string type);

class Asset {
public:
	virtual ~Asset() = default;

	virtual AssetType GetType() const = 0;

public:
	AssetHandle Handle;
};

}

#pragma once

#include "base.h"
#include "asset.h"
#include <map>

namespace Enik {

using AssetMap = std::map<AssetHandle, Ref<Asset>>;

class AssetManagerBase {
public:
	virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
	virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
	virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
	virtual AssetType GetAssetType(AssetHandle handle) const = 0;
	virtual const std::filesystem::path& GetAssetPath(AssetHandle handle) const = 0;

};

}

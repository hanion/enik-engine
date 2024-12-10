#pragma once
#include "asset/asset_metadata.h"

namespace Enik {

class AssetImporter {
public:
	static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
};

}

#pragma once
#include "asset/asset.h"
#include "asset/asset_metadata.h"
#include "renderer/font.h"

namespace Enik {

class FontImporter {
public:
	static Ref<FontAsset> ImportFont(AssetHandle handle, const AssetMetadata& metadata);
};

}

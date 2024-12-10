#pragma once
#include "asset/asset.h"
#include "asset/asset_metadata.h"
#include "asset_importer.h"
#include "renderer/texture.h"

namespace Enik {
class TextureImporter {
public:
	static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
	static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
};
}

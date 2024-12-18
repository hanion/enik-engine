#include "asset_importer.h"
#include "asset/importer/texture_importer.h"
#include "asset/importer/animation_importer.h"
#include "core/asserter.h"
#include <functional>
#include <map>

namespace Enik {

using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;
static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
	{ AssetType::Texture2D, TextureImporter::ImportTexture2D },
	{ AssetType::Animation, AnimationImporter::ImportAnimation },
};

Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata) {
	EN_CORE_ASSERT(s_AssetImportFunctions.find(metadata.Type) != s_AssetImportFunctions.end());
	return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
}


}

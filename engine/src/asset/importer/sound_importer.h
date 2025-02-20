#pragma once
#include "asset/asset.h"
#include "asset/asset_metadata.h"
#include "audio/sound.h"

namespace Enik {
namespace SoundImporter {

Ref<SoundAsset> ImportSound(AssetHandle handle, const AssetMetadata& metadata);

}
}

#pragma once
#include "asset/asset.h"
#include "asset/asset_metadata.h"
#include "scene/animation.h"

namespace Enik {
class AnimationImporter {
public:
	static Ref<Animation> ImportAnimation(AssetHandle handle, const AssetMetadata& metadata);
	static Ref<Animation> DeserializeAnimation(const std::filesystem::path& path);
	static void SerializeAnimation(const Ref<Animation>& animation, const std::filesystem::path& path);
};


}

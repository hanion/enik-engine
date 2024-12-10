#pragma once

#include "asset_manager_base.h"
#include "project/project.h"
#include <memory>

namespace Enik {
namespace AssetManager {

template <typename T>
Ref<T> GetAsset(AssetHandle handle) {
	Ref<Asset> asset = Project::GetAssetManager()->GetAsset(handle);
	return std::static_pointer_cast<T>(asset);
}

inline bool IsAssetHandleValid(AssetHandle handle) {
	return Project::GetAssetManager()->IsAssetHandleValid(handle);
}
inline bool IsAssetLoaded(AssetHandle handle) {
	return Project::GetAssetManager()->IsAssetLoaded(handle);
}
inline AssetType GetAssetType(AssetHandle handle) {
	return Project::GetAssetManager()->GetAssetType(handle);
}

inline const std::filesystem::path& GetAssetPath(AssetHandle handle) {
	return Project::GetAssetManager()->GetAssetPath(handle);
}

}
}

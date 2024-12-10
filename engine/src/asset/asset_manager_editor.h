#pragma once
#include <map>
#include "asset/asset_manager_base.h"
#include "asset/asset_metadata.h"

namespace Enik {

using AssetRegistry = std::map<AssetHandle, AssetMetadata>;


class AssetManagerEditor : public AssetManagerBase {
public:
	virtual bool IsAssetHandleValid(AssetHandle handle) const override;
	virtual bool IsAssetLoaded(AssetHandle handle) const override;
	virtual Ref<Asset> GetAsset(AssetHandle handle) override;
	virtual AssetType GetAssetType(AssetHandle handle) const override;
	const AssetMetadata& GetMetadata(AssetHandle handle) const;
	virtual const std::filesystem::path& GetAssetPath(AssetHandle handle) const override;

	// path should be relative to project
	AssetHandle ImportAsset(const std::filesystem::path& path);


	void SerializeAssetRegistry();
	bool DeserializeAssetRegistry();

private:
	AssetRegistry m_AssetRegistry;
	AssetMap m_LoadedAssets;

};

}

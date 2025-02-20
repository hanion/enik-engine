#pragma once
#include "asset/asset.h"

namespace Enik {

class SoundAsset : public Asset {
public:
	virtual ~SoundAsset();

	static AssetType GetStaticType() { return AssetType::Sound; }
	virtual AssetType GetType() const override { return GetStaticType(); }

public:
	void* sound = nullptr;
};


}

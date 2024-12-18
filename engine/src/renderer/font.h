#pragma once
#include "asset/asset.h"
#include "glm.hpp"
#include "renderer/texture.h"

namespace Enik {

struct Glyph {
	glm::vec2 tex_coords[4];
	glm::vec2 positions[4];
	glm::vec2 Size;
	uint32_t Advance;
};

class FontAsset : public Asset {
public:
	virtual ~FontAsset() = default;

	static AssetType GetStaticType() { return AssetType::Font; }
	virtual AssetType GetType() const override { return GetStaticType(); }

public:
	Ref<Texture2D> AtlasTexture;
	std::vector<Glyph> Glyphs;
	uint32_t TextHeight;
};

}

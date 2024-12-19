#pragma once

#include <glm/glm.hpp>
#include "renderer/texture.h"

namespace Enik {

class SubTexture2D {
public:
	SubTexture2D(const AssetHandle& texture, const glm::vec2& tile_size, const glm::vec2& tile_index, const glm::vec2& tile_sep = glm::vec2(0));
	static Ref<SubTexture2D> CreateFromTileIndex(const AssetHandle& texture, const glm::vec2& tile_size, const glm::vec2& tile_index, const glm::vec2& tile_sep = glm::vec2(0));

	void UpdateSubTexture2D(const Ref<Texture2D>& texture);
	const glm::vec2* GetTextureCoords() const { return m_TextureCoords; }

public:
	glm::vec2 TileSize;
	glm::vec2 TileIndex;
	glm::vec2 TileSeparation;

private:
	glm::vec2 m_TextureCoords[4];
};

}
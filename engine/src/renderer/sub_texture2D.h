#pragma once

#include <glm/glm.hpp>
#include "renderer/texture.h"

namespace Enik {

class SubTexture2D {
public:
	SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& tile_size, const glm::vec2& tile_index, const glm::vec2& tile_sep = glm::vec2(0));
	static Ref<SubTexture2D> CreateFromTileIndex(const Ref<Texture2D> texture, const glm::vec2& tile_size, const glm::vec2& tile_index, const glm::vec2& tile_sep = glm::vec2(0));

	void UpdateSubTexture2D();

	const Ref<Texture2D> GetTexture() const { return m_Texture; }
	const glm::vec2* GetTextureCoords() const { return m_TextureCoords; }

	glm::vec2& GetTileSize() { return m_TileSize; }
	glm::vec2& GetTileIndex() { return m_TileIndex; }
	glm::vec2& GetTileSeparation() { return m_TileSeparation; }


private:
	Ref<Texture2D> m_Texture;
	glm::vec2 m_TextureCoords[4];

	glm::vec2 m_TileSize;
	glm::vec2 m_TileIndex;
	glm::vec2 m_TileSeparation;
};

}
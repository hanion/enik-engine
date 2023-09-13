#include "sub_texture2D.h"
#include <pch.h>

namespace Enik {

SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& tile_size, const glm::vec2& tile_index,  const glm::vec2& tile_sep)
	: m_Texture(texture), m_TileSize(tile_size), m_TileIndex(tile_index), m_TileSeparation(tile_sep) {
	UpdateSubTexture2D();
}

Ref<SubTexture2D> SubTexture2D::CreateFromTileIndex(const Ref<Texture2D> texture, const glm::vec2& tile_size, const glm::vec2& tile_index, const glm::vec2& tile_sep) {
	return CreateRef<SubTexture2D>(texture, tile_size, tile_index, tile_sep);
}

void SubTexture2D::UpdateSubTexture2D() {
	glm::vec2 atlas_size = glm::vec2(m_Texture->GetWidth(), m_Texture->GetHeight());
	glm::vec2 sep = m_TileIndex * m_TileSeparation;

	m_TextureCoords[0] = {( m_TileIndex.x      * m_TileSize.x + sep.x) / atlas_size.x, ( m_TileIndex.y      * m_TileSize.y + sep.y) / atlas_size.y};
	m_TextureCoords[1] = {((m_TileIndex.x + 1) * m_TileSize.x + sep.x) / atlas_size.x, ( m_TileIndex.y      * m_TileSize.y + sep.y) / atlas_size.y};
	m_TextureCoords[2] = {((m_TileIndex.x + 1) * m_TileSize.x + sep.x) / atlas_size.x, ((m_TileIndex.y + 1) * m_TileSize.y + sep.y) / atlas_size.y};
	m_TextureCoords[3] = {( m_TileIndex.x      * m_TileSize.x + sep.x) / atlas_size.x, ((m_TileIndex.y + 1) * m_TileSize.y + sep.y) / atlas_size.y};

}

}
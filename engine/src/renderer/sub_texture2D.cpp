#include "sub_texture2D.h"
#include "asset/asset_manager.h"
#include <pch.h>

namespace Enik {

SubTexture2D::SubTexture2D(const AssetHandle& texture, const glm::vec2& tile_size, const glm::vec2& tile_index,  const glm::vec2& tile_sep)
	: TileSize(tile_size), TileIndex(tile_index), TileSeparation(tile_sep) {
	UpdateSubTexture2D(AssetManager::GetAsset<Texture2D>(texture));
}

Ref<SubTexture2D> SubTexture2D::CreateFromTileIndex(const AssetHandle& texture, const glm::vec2& tile_size, const glm::vec2& tile_index, const glm::vec2& tile_sep) {
	return CreateRef<SubTexture2D>(texture, tile_size, tile_index, tile_sep);
}

void SubTexture2D::UpdateSubTexture2D(const Ref<Texture2D>& texture) {
	glm::vec2 atlas_size = glm::vec2(texture->GetWidth(), texture->GetHeight());
	glm::vec2 sep = TileIndex * TileSeparation;

	m_TextureCoords[0] = {( TileIndex.x      * TileSize.x + sep.x) / atlas_size.x, ( TileIndex.y      * TileSize.y + sep.y) / atlas_size.y};
	m_TextureCoords[1] = {((TileIndex.x + 1) * TileSize.x + sep.x) / atlas_size.x, ( TileIndex.y      * TileSize.y + sep.y) / atlas_size.y};
	m_TextureCoords[2] = {((TileIndex.x + 1) * TileSize.x + sep.x) / atlas_size.x, ((TileIndex.y + 1) * TileSize.y + sep.y) / atlas_size.y};
	m_TextureCoords[3] = {( TileIndex.x      * TileSize.x + sep.x) / atlas_size.x, ((TileIndex.y + 1) * TileSize.y + sep.y) / atlas_size.y};

}

}
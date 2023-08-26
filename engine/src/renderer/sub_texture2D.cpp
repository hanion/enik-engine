#include "sub_texture2D.h"
#include <pch.h>

namespace Enik {

SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& cell_size, const glm::vec2& tile_index,  const glm::vec2& sprite_sep)
	: m_Texture(texture) {

	glm::vec2 atlasSize = glm::vec2(texture->GetWidth(), texture->GetHeight());
	glm::vec2 sep = tile_index * sprite_sep;

	m_TextureCoords[0] = {( tile_index.x      * cell_size.x + sep.x) / atlasSize.x, ( tile_index.y      * cell_size.y + sep.y) / atlasSize.y};
	m_TextureCoords[1] = {((tile_index.x + 1) * cell_size.x + sep.x) / atlasSize.x, ( tile_index.y      * cell_size.y + sep.y) / atlasSize.y};
	m_TextureCoords[2] = {((tile_index.x + 1) * cell_size.x + sep.x) / atlasSize.x, ((tile_index.y + 1) * cell_size.y + sep.y) / atlasSize.y};
	m_TextureCoords[3] = {( tile_index.x      * cell_size.x + sep.x) / atlasSize.x, ((tile_index.y + 1) * cell_size.y + sep.y) / atlasSize.y};

}

Ref<SubTexture2D> SubTexture2D::CreateFromTileIndex(const Ref<Texture2D> texture, const glm::vec2& cell_size, const glm::vec2& tile_index, const glm::vec2& sprite_sep) {
	return CreateRef<SubTexture2D>(texture, cell_size, tile_index, sprite_sep);
}
}
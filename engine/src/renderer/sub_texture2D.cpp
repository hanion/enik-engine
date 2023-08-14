#include "sub_texture2D.h"
#include <pch.h>

namespace Enik {

SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& cellSize, const glm::vec2& tileIndex,  const glm::vec2& spriteSep)
	: m_Texture(texture) {

	glm::vec2 atlasSize = glm::vec2(texture->GetWidth(), texture->GetHeight());
	glm::vec2 sep = tileIndex * spriteSep;

	m_TextureCoords[0] = {( tileIndex.x      * cellSize.x + sep.x) / atlasSize.x, ( tileIndex.y      * cellSize.y + sep.y) / atlasSize.y};
	m_TextureCoords[1] = {((tileIndex.x + 1) * cellSize.x + sep.x) / atlasSize.x, ( tileIndex.y      * cellSize.y + sep.y) / atlasSize.y};
	m_TextureCoords[2] = {((tileIndex.x + 1) * cellSize.x + sep.x) / atlasSize.x, ((tileIndex.y + 1) * cellSize.y + sep.y) / atlasSize.y};
	m_TextureCoords[3] = {( tileIndex.x      * cellSize.x + sep.x) / atlasSize.x, ((tileIndex.y + 1) * cellSize.y + sep.y) / atlasSize.y};
	
}


Ref<SubTexture2D> SubTexture2D::CreateFromTileIndex(const Ref<Texture2D> texture, const glm::vec2& cellSize, const glm::vec2& tileIndex, const glm::vec2& spriteSep) {
        return CreateRef<SubTexture2D>(texture, cellSize, tileIndex, spriteSep);
}
}
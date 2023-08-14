#pragma once

#include <glm/glm.hpp>
#include "renderer/texture.h"

namespace Enik {

class SubTexture2D  {
public:
	SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& cellSize, const glm::vec2& tileIndex, const glm::vec2& spriteSep = glm::vec2(0));
	static Ref<SubTexture2D> CreateFromTileIndex(const Ref<Texture2D> texture, const glm::vec2& cellSize, const glm::vec2& tileIndex, const glm::vec2& spriteSep = glm::vec2(0));

	const Ref<Texture2D> GetTexture() const { return m_Texture; }
	const glm::vec2* GetTextureCoords() const { return m_TextureCoords; }

private:
	Ref<Texture2D> m_Texture;
	glm::vec2 m_TextureCoords[4];
};

}
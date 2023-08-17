#pragma once

#include <glm.hpp>
#include "renderer/sub_texture2D.h"

namespace Enik {
namespace Component {


struct Tag {
	std::string Text;

	Tag() = default;
	Tag(const Tag&) = default;
	Tag(const std::string& text) 
		: Text(text) {}
	
	operator std::string& () { return Text; }
	operator const std::string& () const { return Text; }
};


struct Transform {
	glm::vec3 Position = glm::vec3(0.0f); 
	// glm::vec3 Rotation = glm::vec3(0.0f); // TODO use Quaternions
	float Rotation     = 0.0f;
	glm::vec2 Scale    = glm::vec2(1.0f);

	Transform() = default;
	Transform(const Transform&) = default;
	// Transform(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(0.0f)) 
	// 	: Position(position), Rotation(rotation), Scale(scale) {}
	Transform(const glm::vec3& position, const float rotation = 0.0f, const glm::vec3& scale = glm::vec3(0.0f)) 
		: Position(position), Rotation(rotation), Scale(scale) {}

};


struct SpriteRenderer {
	glm::vec4 Color = glm::vec4(1.0f);
	
	Ref<Texture2D>    Texture    = nullptr;
	Ref<SubTexture2D> SubTexture = nullptr;
	float TileScale = 1.0f;

	SpriteRenderer() = default;
	SpriteRenderer(const SpriteRenderer&) = default;
	SpriteRenderer(const glm::vec4& color) 
		: Color(color) {}
	
};


}

}
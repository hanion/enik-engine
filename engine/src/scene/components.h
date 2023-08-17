#pragma once

#include <glm.hpp>
#include "renderer/sub_texture2D.h"
#include "scene/scene_camera.h"
#include "scene/scriptable_entity.h"
#include <glm/gtc/matrix_transform.hpp>

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

	glm::mat4 GetTransform() {
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, Position)
			* glm::rotate(transform, Rotation, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(transform, glm::vec3(Scale.x, Scale.y, 1.0f));
		return transform;
	}
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

struct Camera {
	SceneCamera Cam;
	bool Primary = true;
	bool FixedAspectRatio = false;

	Camera() = default;
	Camera(const Camera&) = default;
	
};


struct NativeScript {
	ScriptableEntity* Instance = nullptr;

	ScriptableEntity*(*InstantiateScript)();
	void (*DestroyScript)(NativeScript*);

	template<typename T>
	void Bind() {
		
		InstantiateScript = []() {
			return static_cast<ScriptableEntity*>(new T());
		};

		DestroyScript = [](NativeScript* ns) {
			delete ns->Instance;
			ns->Instance = nullptr;
		};
	}
};

}

}
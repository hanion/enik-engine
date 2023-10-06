#pragma once

#include "renderer/sub_texture2D.h"
#include "scene/scene_camera.h"
#include "scene/scriptable_entity.h"
#include "core/uuid.h"

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Enik {
namespace Component {

struct ID {
	UUID uuid;

	ID() = default;
	ID(const ID&) = default;
	ID(const UUID& id)
		: uuid(id) {}

	operator uint64_t() { return (uint64_t)uuid; }
};

struct Tag {
	std::string Text;

	Tag() = default;
	Tag(const Tag&) = default;
	Tag(const std::string& text)
		: Text(text) {}

	operator std::string&() { return Text; }
	operator const std::string&() const { return Text; }
};

struct Transform {
	glm::vec3 Position = glm::vec3(0.0f);
	float Rotation = 0.0f;
	glm::vec2 Scale = glm::vec2(1.0f);

	Transform() = default;
	Transform(const Transform&) = default;
	Transform(const glm::vec3& position, const float rotation = 0.0f, const glm::vec3& scale = glm::vec3(0.0f))
		: Position(position), Rotation(rotation), Scale(scale) {}

	glm::mat4 GetTransform() const {
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, Position);
		transform = glm::rotate(transform, Rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(Scale.x, Scale.y, 1.0f));
		return transform;
	}
};

struct SpriteRenderer {
	glm::vec4 Color = glm::vec4(1.0f);

	Ref<Texture2D> Texture = nullptr;
	Ref<SubTexture2D> SubTexture = nullptr;
	float TileScale = 1.0f;

	bool mag_filter_linear = true;

	// ? relative path
	std::filesystem::path TexturePath;

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

	std::function<ScriptableEntity*()> InstantiateScript;
	std::function<void(NativeScript*)> DestroyScript;

	std::string ScriptName;

	std::map<std::string, NativeScriptField> NativeScriptFields;
	void ApplyNativeScriptFieldsToInstance();

	void Bind(const std::string& script_name, const std::function<ScriptableEntity*()>& inst) {
		ScriptName = script_name;

		InstantiateScript = inst;

		DestroyScript = [](NativeScript* ns) {
			delete ns->Instance;
			ns->Instance = nullptr;
		};
	}
};


struct RigidBody {
	glm::vec3 Velocity;
	glm::vec3 Force;
	float Mass = 1.0f;

	bool UseGravity = false;

	RigidBody() = default;
	RigidBody(const RigidBody&) = default;

	void ApplyForce(const glm::vec3& force) {
		Force += force;
	}

	void ApplyImpulse(const glm::vec3& impulse) {
		if (Mass != 0.0f) {
			Velocity += impulse / Mass;
		}
	}
};



enum ColliderShape {
	CIRCLE, PLANE, BOX
};
struct Collider {
	ColliderShape Shape = ColliderShape::CIRCLE;
	glm::vec3 Vector = glm::vec3(0,0,0);
	float Float = 0.5f;

	Collider() = default;
	Collider(const Collider&) = default;

	const std::string String() const {
		switch (Shape) {
			case ColliderShape::CIRCLE: return "Circle";
			case ColliderShape::PLANE:  return "Plane";
			case ColliderShape::BOX:    return "Box";
		}
		return std::string();
	}
};

}

}
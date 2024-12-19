#pragma once

#include "renderer/sub_texture2D.h"
#include "scene/scene_camera.h"
#include "core/uuid.h"
#include <map>
#include "scene/native_script_fields.h"
#include "scene/animation.h"

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Enik {

class ScriptableEntity;
class Entity;

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
	glm::vec3 LocalPosition = glm::vec3(0.0f);
	float     LocalRotation = 0.0f;
	glm::vec3 LocalScale    = glm::vec3(1.0f);

	glm::vec3 GlobalPosition = LocalPosition;
	float     GlobalRotation = LocalRotation;
	glm::vec3 GlobalScale    = LocalScale;

	Transform() = default;
	Transform(const Transform&) = default;
	Transform(const glm::vec3& position, float rotation = 0.0f, const glm::vec3& scale = glm::vec3(1.0f))
	: LocalPosition(position), LocalRotation(rotation), LocalScale(scale) {}

	glm::mat4 GetTransform() const {
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, GlobalPosition);
		transform = glm::rotate(transform, GlobalRotation, glm::vec3(0.0f, 0.0f, 1.0f));
		transform = glm::scale(transform, GlobalScale);
		return transform;
	}
};

struct SpriteRenderer {
	glm::vec4 Color = glm::vec4(1.0f);

	AssetHandle Handle = 0;
	float TileScale = 1.0f;

	Ref<SubTexture2D> SubTexture = nullptr;

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

	void Bind(const std::string& script_name, const std::function<ScriptableEntity*()>& inst);
};


struct RigidBody {
	glm::vec3 Velocity;
	glm::vec3 Force;
	float Mass = 1.0f;

	bool Awake = true;
	bool UseGravity = false;

	RigidBody() = default;
	RigidBody(const RigidBody&) = default;

	static constexpr float epsilon = 0.001f;

	void ApplyForce(const glm::vec3& force) {
		if (force.length() < epsilon) {
			return;
		}

		Awake = true;
		Force += force;
	}

	void ApplyImpulse(const glm::vec3& impulse) {
		if (impulse.length() < epsilon) {
			return;
		}

		if (Mass != 0.0f) {
			Awake = true;
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
	bool IsArea = false;

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



class Family {
public:
	Family() = default;
	Family(const Family&) = default;

	std::vector<Entity> Children;

	bool HasParent();
	Entity GetParent();

	void Reparent(Entity this_entity, Entity new_parent);

	void SetChildrenGlobalTransformRecursive(Component::Transform& transform);

	bool HasEntityAsChild(const Entity& entity);

private:
	Ref<Entity> Parent;

	void AddChild(const Entity& entity);

	void RemoveChild(const Entity& entity);

	void SetParent(const Entity& entity);
};

struct Prefab {
	bool RootPrefab = false;
	std::filesystem::path PrefabPath = std::filesystem::path();

	Prefab() = default;
	Prefab(const Prefab&) = default;
};


struct AudioSources {
	std::vector<std::filesystem::path> SourcePaths;

	AudioSources() = default;
	AudioSources(const AudioSources&) = default;

	void Play(const std::string& name);
	void Play(int index);
};


struct AnimationPlayer {
	Ref<Entity> BoundEntity = nullptr;
	std::map<std::string, AssetHandle> Animations = {};
	AssetHandle CurrentAnimation = 0;
	float CurrentTime = 0;
	bool Paused = false;

	void Start(const std::string& name);
	// fast forwards to the end
	void End();
	// stops the animation where it currently is
	void Kill();

	void Update(const Timestep& dt);
};


struct Text {
	std::string Data;
	AssetHandle Font = 0;
	glm::vec4 Color = glm::vec4(1.0f);
	float Scale = 10.0f;
	float Visible = 1.0f;
};

}

}
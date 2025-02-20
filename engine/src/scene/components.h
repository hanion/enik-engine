#pragma once

#include "renderer/sub_texture2D.h"
#include "scene/scene_camera.h"
#include "core/uuid.h"
#include <map>
#include "scene/native_script_fields.h"
#include "scene/animation.h"
#include "physics/physics_body.h"
#include <Jolt/Physics/PhysicsSystem.h>

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
	glm::quat LocalRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 LocalScale    = glm::vec3(1.0f);

	glm::vec3 GlobalPosition = LocalPosition;
	glm::quat GlobalRotation = LocalRotation;
	glm::vec3 GlobalScale    = LocalScale;

	Transform() = default;
	Transform(const Transform&) = default;
	Transform(const glm::vec3& position, const glm::quat& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), const glm::vec3& scale = glm::vec3(1.0f))
	: LocalPosition(position), LocalRotation(rotation), LocalScale(scale) {}

	glm::mat4 GetTransform() const {
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, GlobalPosition);
		transform *= glm::mat4_cast(GlobalRotation);
		transform = glm::scale(transform, GlobalScale);
		return transform;
	}

	glm::vec3  GetLocalRotationEuler() const { return glm::eulerAngles(LocalRotation); }
	glm::vec3 GetGlobalRotationEuler() const { return glm::eulerAngles(GlobalRotation); }
	glm::vec3  GetLocalRotationEulerDegrees() const { return glm::degrees(glm::eulerAngles(LocalRotation)); }
	glm::vec3 GetGlobalRotationEulerDegrees() const { return glm::degrees(glm::eulerAngles(GlobalRotation)); }
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
	bool Called_OnCreate = false;

	std::function<ScriptableEntity*()> InstantiateScript;
	std::function<void(NativeScript*)> DestroyScript;

	std::string ScriptName;

	std::map<std::string, NativeScriptField> NativeScriptFields;
	void ApplyNativeScriptFieldsToInstance();

	void Bind(const std::string& script_name, const std::function<ScriptableEntity*()>& inst);
};



struct PhysicsBodyBase {
	PhysicsBody* body = nullptr;
	uint16_t Layer = 1;
	JPH::EMotionType MotionType = JPH::EMotionType::Dynamic;

	PhysicsBodyBase() = default;
	PhysicsBodyBase(const PhysicsBodyBase&) = default;
	virtual ~PhysicsBodyBase() = default;

	bool IsStatic()    const { return MotionType == JPH::EMotionType::Static; }
	bool IsKinematic() const { return MotionType == JPH::EMotionType::Kinematic; }

	void SetStatic();
	void SetKinematic();

	glm::vec3 GetLinearVelocity() const;
	void      SetLinearVelocity(const glm::vec3& velocity);

	glm::quat GetRotation() const;

	glm::vec3 GetAngularVelocity() const;
	void      SetAngularVelocity(const glm::vec3& angular_velocity);

	void SetPosition(const glm::vec3& position);

	void  SetFriction(float friction);
	float GetFriction();

	void  SetRestitution(float restution);
	float GetRestitution();

protected:
	JPH::PhysicsSystem* GetPhysicsSystem() const;
	JPH::BodyInterface& GetBodyInterface() const;
};

struct RigidBody : PhysicsBodyBase {
	RigidBody() = default;
	RigidBody(const RigidBody&) = default;

	void AddForce(const glm::vec3& force);
	void AddImpulse(const glm::vec3& impulse);
	void AddTorque(const glm::vec3& torque);

	float GetMass() const { return Mass; }
	void  SetMass(float mass);

	float GetGravityFactor() const { return GravityFactor; }
	void  SetGravityFactor(float gravity_factor);

private:
	float Mass = 1.0f;
	float GravityFactor = 1.0f;
};

struct CollisionBody : PhysicsBodyBase {
	bool IsSensor = false;

	CollisionBody() { SetStatic(); }
	CollisionBody(const CollisionBody&) = default;

	void SetIsSensor(bool is_sensor);
};


struct CollisionShape {
	JPH::Ref<JPH::Shape> shape = nullptr;

	enum class Type {
		NONE, BOX, CIRCLE
	};
	Type Shape = Type::BOX;

	union {
		glm::vec3 Vector = glm::vec3(0.5f,0.5f,0.5f);
		glm::vec3 BoxScale;
		glm::vec3 CircleCenter;
	};
	union {
		float Float = 0.5f;
		float CircleRadius;
	};

	CollisionShape() = default;
	CollisionShape(const CollisionShape&) = default;

	const std::string ToString() const { return TypeToString(Shape); }

	static std::string TypeToString(Type shape) {
		switch (shape) {
			case Type::NONE:   return "NONE";
			case Type::BOX:    return "Box";
			case Type::CIRCLE: return "Circle";
		}
		return std::string();
	}

	static Type TypeFromString(const std::string& str) {
		if      (str.empty())     { return Type::NONE; }
		else if (str == "Box")    { return Type::BOX; }
		else if (str == "Circle") { return Type::CIRCLE; }
		return Type::NONE;
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

	void SetChildrenGlobalTransformRecursive(const Component::Transform& transform);

	bool HasEntityAsChild(const Entity& entity);

	void SetGlobalPositionRotation(Component::Transform& tr, const glm::vec3& global_pos, const glm::quat& global_rot);
	void SetGlobalPosition(Component::Transform& tr, const glm::vec3& global);
	void SetGlobalRotation(Component::Transform& tr, const glm::quat& global);

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
	std::map<std::string, AssetHandle> Sounds = {};

	AudioSources() = default;
	AudioSources(const AudioSources&) = default;

	void Play(AssetHandle sound_handle);
	void Play(const std::string& name);
};


struct AnimationPlayer {
	Ref<Entity> BoundEntity = nullptr;
	std::map<std::string, AssetHandle> Animations = {};
	AssetHandle CurrentAnimation = 0;
	float CurrentTime = 0;
	bool Paused = false;

	std::function<void(const std::string&)> OnEndCallback = nullptr;

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

	glm::vec2 GetBoundingBox();
	float GetWidth();
	float GetHeight();
};

struct SceneControl {
	bool Persistent = true;
};

}

}

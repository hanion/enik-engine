#pragma once

#ifndef EN_STATIC_SCRIPT_MODULE
	#define EN_STATIC_SCRIPT_MODULE 0
#endif

/*
 *         enik-engine single header file
 *   this is a stub header for the script module
 *
*/


extern "C" void RegisterAllScripts();
#define GLM_ENABLE_EXPERIMENTAL

#if EN_STATIC_SCRIPT_MODULE
	#include <Enik.h>
	#define CONSOLE_DEBUG(txt)             EN_TRACE(txt)
	#define CONSOLE_DEBUGS(txt, msg)       EN_TRACE(txt, msg)
	#define CONSOLE_DEBUG_ERROR(txt)       EN_ERROR(txt)
	#define CONSOLE_DEBUGS_ERROR(txt, msg) EN_ERROR(txt, msg)
#else


#include <cstring>
#include <map>

#include <memory>
#include <algorithm>
#include <functional>
#include <stdio.h>

#include <iostream>
#include <filesystem>

#include <vector>
#include <sstream>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <cstdint>
#include <variant>

#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"



#ifdef EN_DEBUG
	#if defined(EN_PLATFORM_WINDOWS)
		#define EN_DEBUGBREAK() __debugbreak()
	#elif defined(EN_PLATFORM_LINUX)
		#include <signal.h>
		#define EN_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define EN_ENABLE_ASSERTS
#else
	#define EN_DEBUGBREAK()
#endif


#define EN_EXPAND_MACRO(x) x
#define EN_STRINGIFY_MACRO(x) #x

#define EN_BIND_EVENT_FN(fn) std::bind(&Enik::fn, this, std::placeholders::_1)

// path relative to ./build/sandbox
#define FULL_PATH(x) (((std::string)"../../sandbox/") + ((std::string)x))
#define FULL_PATH_EDITOR(x) (((std::string)"../../editor/") + ((std::string)x))

namespace Enik {

constexpr double PHYSICS_UPDATE_RATE = 1.0/144.0;

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}


namespace Enik {
class Console {
public:

// ? we can't extern "C" templates so ...

static void Trace(const std::string& msg);
static void Trace(const std::string& msg, void* ptr);
static void Trace(const std::string& msg, const std::string& txt);

static void Trace(const std::string& msg, float x);
static void Trace(const std::string& msg, float x, float y);
static void Trace(const std::string& msg, float x, float y, float z);

static void Info (const std::string& msg);
static void Info (const std::string& msg, const std::string& txt);

static void Warn (const std::string& msg);
static void Warn (const std::string& msg, const std::string& txt);

static void Error(const std::string& msg);
static void Error(const std::string& msg, const std::string& txt);
};

}



namespace Enik {

class Timestep {
public:
	Timestep(float time = 0.0f)
		: m_Time(time) {
	}

	operator float() const { return m_Time; }

	inline float GetSeconds() const { return m_Time; }
	inline float GetMilliseconds() const { return m_Time * 1000.0f; }

private:
	float m_Time;
};

}



namespace Enik {

class UUID {
public:
	UUID();
	UUID(uint64_t uuid);
	UUID(const UUID&) = default;

	operator uint64_t() const { return m_UUID; }

private:
	uint64_t m_UUID;
};

}

namespace std {
template <>
struct hash<Enik::UUID> {
	size_t operator()(const Enik::UUID& uuid) const {
		return (uint64_t)uuid;
	}
};
}






namespace Enik {

class Tween {
public:
	static void Create(float* value, float start_value, float end_value, float duration);
	static void Create(float* value, float end_value, float duration);
	static void Create(float* value, float end_value, float duration, const std::function<void()>* call_on_end);

	static void StepAll(Timestep ts);
	static void ResetData();
};

}














namespace Enik {

class Application {
public:
	static Application& Get();
	void Close();
};
}




namespace Enik {
// buffer.h
struct Buffer {
	uint8_t *Data = nullptr;
	uint64_t Size = 0;

	Buffer() = default;

	Buffer(uint64_t size) { Allocate(size); }
	Buffer(const void* data, uint64_t size) : Data((uint8_t*)data), Size(size) {}

	Buffer(const Buffer &) = default;

	static Buffer Copy(Buffer other) {
		Buffer result(other.Size);
		memcpy(result.Data, other.Data, other.Size);
		return result;
	}

	void Allocate(uint64_t size) {
		Release();

		Data = (uint8_t*)malloc(size);
		Size = size;
	}

	void Release() {
		free(Data);
		Data = nullptr;
		Size = 0;
	}

	template <typename T> T *As() { return (T *)Data; }

	operator bool() const { return (bool)Data; }
};

struct ScopedBuffer {
	ScopedBuffer(Buffer buffer) : m_Buffer(buffer) {}

	ScopedBuffer(uint64_t size) : m_Buffer(size) {}

	~ScopedBuffer() { m_Buffer.Release(); }

	uint8_t *Data() { return m_Buffer.Data; }
	uint64_t Size() { return m_Buffer.Size; }

	template <typename T> T *As() { return m_Buffer.As<T>(); }

	operator bool() const { return m_Buffer; }

private:
	Buffer m_Buffer;
};
}


// asset.h
namespace Enik {

using AssetHandle = UUID;

enum class AssetType : uint16_t {
	None = 0,
	Scene, Prefab,
	Texture2D,
	Font,
	Audio,
	Animation,
};

const char* AssetTypeToString(AssetType type);
AssetType AssetTypeFromString(std::string type);

class Asset {
public:
	virtual ~Asset() = default;

	virtual AssetType GetType() const = 0;

public:
	AssetHandle Handle;
};


enum class ImageFormat {
	None = 0,
	R8, RGB8, RGBA8, RGBA32F
};

struct TextureSpecification {
	uint32_t Width = 1;
	uint32_t Height = 1;
	ImageFormat Format = ImageFormat::RGBA8;
	bool MagFilterLinear = true;
};

class Texture : public Asset {
   public:
	virtual ~Texture() = default;

	virtual const TextureSpecification& GetSpecification() const = 0;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;
	virtual uint32_t GetRendererID() const = 0;

	virtual void SetData(Buffer data) = 0;

	virtual void Bind(uint32_t slot = 0) const = 0;

	virtual bool operator==(const Texture& other) const = 0;
	virtual bool equals(const Texture& other) const = 0;
};

class Texture2D : public Texture {
public:
	static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer data = Buffer());

	static AssetType GetStaticType() { return AssetType::Texture2D; }
	virtual AssetType GetType() const override { return GetStaticType(); }
};

class SubTexture2D {
public:
	SubTexture2D(const AssetHandle& texture, const glm::vec2& tile_size, const glm::vec2& tile_index, const glm::vec2& tile_sep = glm::vec2(0));
	static Ref<SubTexture2D> CreateFromTileIndex(const AssetHandle& texture, const glm::vec2& tile_size, const glm::vec2& tile_index, const glm::vec2& tile_sep = glm::vec2(0));

	void UpdateSubTexture2D(const Ref<Texture2D>& texture);
	const glm::vec2* GetTextureCoords() const { return m_TextureCoords; }

public:
	glm::vec2 TileSize;
	glm::vec2 TileIndex;
	glm::vec2 TileSeparation;

private:
	glm::vec2 m_TextureCoords[4];
};


class Entity;

enum class EASE {
	NONE = 0,
};
enum class TrackProperty {
	NONE,
	Position, Rotation, Scale, Color,

	PositionX, PositionY, PositionZ,
	ScaleX, ScaleY,
	ColorR, ColorG, ColorB, ColorA,
};
extern const std::map<std::string, TrackProperty> StringToTrackProperty;
using AnimationKeyValue = std::variant<float, int, bool, std::string, glm::vec2, glm::vec3, glm::vec4>;




struct Keyframe {
	float Time = 0.0f;
	AnimationKeyValue Value = 0.0f;

	void InitializeZero(TrackProperty property);
};

struct Track {
	TrackProperty Property;
	std::vector<Keyframe> Keyframes;

	void SortKeyframes() {
		std::sort(Keyframes.begin(), Keyframes.end(), [](const Keyframe& a, const Keyframe& b) {
			return a.Time < b.Time;
		});
	}
};

// NOTE: does not hold state, just asset data, shared
class Animation : public Asset {
public:
	static AssetType GetStaticType() { return AssetType::Animation; }
	virtual AssetType GetType() const override { return GetStaticType(); }
public:
	std::vector<Track> Tracks = {};
	std::string Name = "";
	float Duration = 0;
 	bool Looping = false;

public:
	void SortKeyframes() {
		for (Track& track : Tracks) {
			track.SortKeyframes();
		}
	}
	void Update(const Entity& entity, float time) const;
	static float ease(float t, EASE easing_function = EASE::NONE);
	static AnimationKeyValue lerp(AnimationKeyValue a, AnimationKeyValue b, float t);

	
	static const std::string TrackPropertyToString(TrackProperty property);
	static TrackProperty TrackPropertyFromString(const std::string& str);

private:
	static bool HasProperty(const Entity& entity, const TrackProperty& property);
	static void ApplyPropertyValue(const Entity& entity, const TrackProperty& property, const AnimationKeyValue& value);


};


}




namespace Enik {

class Entity;
struct Raycast;
struct RaycastResult;


class Physics {
public:
	bool m_is_initialized = false;

	RaycastResult CastRay(const Raycast& ray);

};


class Scene {
public:
	Scene();
	~Scene();

	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
	void DestroyEntity(Entity entity);

	Entity InstantiatePrefab(const std::filesystem::path& path, UUID instance_uuid = UUID());

	void OnUpdateRuntime(Timestep ts);
	void OnViewportResize(uint32_t width, uint32_t height);

	const std::string& GetName() const { return m_SceneName; }
	const void SetName(const std::string& name) { m_SceneName = name; }

	bool IsPaused() const { return m_IsPaused; }
	void SetPaused(bool is_paused) { m_IsPaused = is_paused; }

	void Step(int frames = 1) { m_StepFrames = frames; }

	void CloseApplication();
	void ChangeScene(const std::string& path);

private:
	Physics m_Physics;

	uint32_t m_ViewportWidth;
	uint32_t m_ViewportHeight;

	std::string m_SceneName = "untitled";

	bool m_IsPaused = false;
	int m_StepFrames = 0;

	std::string m_deferred_scene_path = "";

	friend class Entity;
	friend class SceneTreePanel;
	friend class InspectorPanel;
	friend class SceneSerializer;
	friend class ScriptSystem;
	friend class ScriptableEntity;
};

}

namespace Enik {


class Camera {
public:
	Camera() = default;
	Camera(const glm::mat4& projection)
		: m_Projection(projection) {}

	virtual ~Camera() = default;

	const glm::mat4& GetProjection() const { return m_Projection; }

protected:
	glm::mat4 m_Projection = glm::mat4(1.0f);
};

class SceneCamera : public Camera {
public:
	SceneCamera();
	virtual ~SceneCamera();

	void SetOrthographic(float size, float near, float far);

	void SetViewportSize(uint32_t width, uint32_t height);

	float GetSize       () { return m_OrthographicSize; }
	float GetFar        () { return m_OrthographicFar ; }
	float GetNear       () { return m_OrthographicNear; }
	float GetAspectRatio() { return m_AspectRatio     ; }

	void SetSize       (float size ) { m_OrthographicSize = size ; RecalculateProjection(); }
	void SetFar        (float far  ) { m_OrthographicFar  = far  ; RecalculateProjection(); }
	void SetNear       (float near ) { m_OrthographicNear = near ; RecalculateProjection(); }
	void SetAspectRatio(float ratio) { m_AspectRatio      = ratio; RecalculateProjection(); }

	glm::vec2 GetWorldPosition(const float x, const float y) const;

	glm::vec2 GetWorldPosition(const std::pair<float, float>& position) const {
		return GetWorldPosition(position.first , position.second);
	}

private:
	void RecalculateProjection();

private:
	float m_OrthographicSize =  10.0f;
	float m_OrthographicFar  =  1.0f;
	float m_OrthographicNear = -1.0f;

	float m_AspectRatio = 1.0f;

};
}






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

	glm::vec3 GetLocalRotationEuler () const { return glm::eulerAngles(LocalRotation); }
	glm::vec3 GetGlobalRotationEuler() const { return glm::eulerAngles(GlobalRotation); }
	glm::vec3 GetLocalRotationEulerDegrees () const { return glm::degrees(glm::eulerAngles(LocalRotation)); }
	glm::vec3 GetGlobalRotationEulerDegrees() const { return glm::degrees(glm::eulerAngles(GlobalRotation)); }
};


struct SpriteRenderer {
	glm::vec4 Color = glm::vec4(1.0f);

	AssetHandle Handle = 0;
	float TileScale = 1.0f;

	Ref<SubTexture2D> SubTexture = nullptr;
	void UpdateSubTexture();

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
	bool Called_OnAwake = false;

	std::function<ScriptableEntity*()> InstantiateScript;
	std::function<void(NativeScript*)> DestroyScript;

	std::string ScriptName;

	void Bind(const std::string& script_name, const std::function<ScriptableEntity*()>& inst);
};




namespace JPH {
enum class EMotionType : uint8_t {
	Static,						///< Non movable
	Kinematic,					///< Movable using velocities only, does not respond to forces
	Dynamic,					///< Responds to forces as a normal physics object
};
}

struct PhysicsBodyBase {
	class PhysicsBody* body = nullptr;
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

	void ActivateBody();

protected:
	void* GetPhysicsSystem() const;
	void*& GetBodyInterface() const;
};

struct RigidBody : PhysicsBodyBase {
	RigidBody() = default;
	RigidBody(const RigidBody&) = default;

	void SetKinematic(bool is_kinematic);

	void AddForce(const glm::vec3& force);
	void AddImpulse(const glm::vec3& impulse);
	/// Add torque (unit: N m) for the next time step, will be reset after the next call to PhysicsSystem::Update.
	/// If you want the body to wake up when it is sleeping, use BodyInterface::AddTorque instead.
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

	CollisionBody() { MotionType = JPH::EMotionType::Kinematic; }
	CollisionBody(const CollisionBody&) = default;

	void SetIsSensor(bool is_sensor);
};


struct CollisionShape {
	void* shape = nullptr;

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
	Prefab() = default;
	Prefab(const Prefab&) = default;

	bool RootPrefab = false;
	std::filesystem::path PrefabPath = std::filesystem::path();
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
};

struct SceneControl {
	bool Persistent = true;
};

}






class Audio {
public:
	static void Init();

	static void Play(const char* filepath);
	static void Play(const std::filesystem::path& filepath);

	~Audio();

private:
	static Scope<Audio> s_Instance;
};



class Entity {
public:
	Entity() = default;
	Entity(uint64_t handle, Scene* scene);


	template <typename T, typename... Args>
	T& Add(Args&&... args);


	// ?????????????
	/*
	 * not defining this might have fixed not unloading problem
	 * YES SSSSSSSSSSSSSSSSSSSSSSSSSS
	 *	template <typename T>
	 *	T& Get() {
	 *		static T does_not_matter;
	 *		return does_not_matter;
	 *	}
	 *
	 * turns out it does matter and some inlining might be happening inside script_module
	 *
	*/

	template <typename T>
	__attribute__((__noinline__))
	T& Get() const;

	template <typename T>
	__attribute__((__noinline__))
	bool Has() const;

	template <typename T>
	void Remove();

	template <typename T>
	T& GetOrAdd() {
		if (Has<T>()) {
			return Get<T>();
		}
		return Add<T>();
	}

	// operator bool() const;
	operator bool() const;

	operator uint64_t() const { return m_Handle; }
	// operator uint32_t() const { return (uint32_t)m_Handle; }

	bool operator==(Entity other) {
		return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
	}
	bool operator!=(Entity other) {
		return !(*this == other);
	}


	inline UUID& GetID() const {
		return Get<Component::ID>().uuid;
	}

	inline std::string& GetTag() const {
		return Get<Component::Tag>().Text;
	}

	inline ScriptableEntity* GetScriptInstance() const {
		return Get<Component::NativeScript>().Instance;
	}



	inline bool HasFamily() const {
		return Has<Component::Family>();
	}
	Component::Family& GetOrAddFamily() {
		return GetOrAdd<Component::Family>();
	}

	bool HasParent() {
		if (not HasFamily()) {
			return false;
		}
		return GetOrAddFamily().HasParent();
	}
	Entity GetParent() {
		return GetOrAddFamily().GetParent();
	}
	void Reparent(Entity new_parent) {
		GetOrAddFamily().Reparent(*this, new_parent);
	}

	std::vector<Entity>& GetChildren() {
		return GetOrAddFamily().Children;
	}


	Entity CreateEntity(const std::string& name = std::string()) {
		return m_Scene->CreateEntity(name);
	}

	void DestroyEntity(Entity entity) {
		m_Scene->DestroyEntity(entity);
	}

	Entity InstantiatePrefab(const std::filesystem::path& path) {
		return m_Scene->InstantiatePrefab(path);
	}

	void CloseApplication() {
		m_Scene->CloseApplication();
	}

	void ChangeScene(const std::string& path) {
		m_Scene->ChangeScene(path);
	}

	uint64_t m_Handle{};
	Scene* m_Scene = nullptr;
private:

	friend class ScriptableEntity;
};



struct Raycast {
	glm::vec3 origin;
	glm::vec3 dir;
	uint16_t layer;
};

struct RaycastResult {
	Entity entity;
	glm::vec3 point;
};



}




namespace Enik {


enum class FieldType {
	NONE,
	BOOL, INT, FLOAT, DOUBLE,
	VEC2, VEC3, VEC4,
	PREFAB,
	STRING,
	ENTITY
};
struct NativeScriptField {
	std::string Name;
	FieldType Type;
	void* Value;

	NativeScriptField()
		: Type(FieldType::NONE), Value(nullptr) {}

	NativeScriptField(const std::string& name, FieldType type, void* value)
		: Name(name), Type(type), Value(value) {}

};











}




namespace Enik {

namespace ScriptRegistry {
	extern "C" void RegisterScriptClass(const std::string& class_name, ScriptableEntity* (*create_function)());
	std::unordered_map<std::string, ScriptableEntity* (*)()>& GetRegistry();
}

}



namespace Enik {

class Input {
public:
	static bool IsKeyPressed(int keycode);
	static bool IsMouseButtonPressed(int button);
	static std::pair<float, float> GetMousePosition();

private:
	static Scope<Input> s_Instance;
};

}


// ??????????????????????????
// ????????? EVENTS ?????????
// ??????????????????????????
#define BIT(x) (1 << x)
namespace Enik {

enum class EventType {
	None = 0,
	WindowClose,
	WindowResize,
	WindowFocus,
	WindowLostFocus,
	WindowMoved,
	AppTick,
	AppUpdate,
	AppRender,
	KeyPressed,
	KeyReleased,
	KeyTyped,
	MouseButtonPressed,
	MouseButtonReleased,
	MouseMoved,
	MouseScrolled
};

enum EventCategory {
	None = 0,
	EventCategoryApplication = BIT(0),
	EventCategoryInput = BIT(1),
	EventCategoryKeyboard = BIT(2),
	EventCategoryMouse = BIT(3),
	EventCategoryMouseButton = BIT(4)
};

#define EVENT_CLASS_TYPE(type)                                                  \
	static EventType GetStaticType() { return EventType::type; }                \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return category; }



class Event {
public:
	virtual ~Event() = default;

	bool Handled = false;

	virtual EventType GetEventType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetCategoryFlags() const = 0;
	virtual std::string ToString() const { return GetName(); }

	bool IsInCategory(EventCategory category) {
		return GetCategoryFlags() & category;
	}
};



class EventDispatcher {
public:
	EventDispatcher(Event& event)
		: m_Event(event) {
	}

	// F will be deduced by the compiler
	template <typename T, typename F>
	bool Dispatch(const F& func) {
		if (m_Event.GetEventType() == T::GetStaticType()) {
			m_Event.Handled |= func(static_cast<T&>(m_Event));
			return true;
		}
		return false;
	}

private:
	Event& m_Event;
};



inline std::ostream& operator<<(std::ostream& os, const Event& e) {
	return os << e.ToString();
}

}

namespace Enik {
	using KeyCode = uint16_t;

	namespace Key
	{
		enum : KeyCode
		{
			// From glfw3.h
			Space               = 32,
			Apostrophe          = 39, /* ' */
			Comma               = 44, /* , */
			Minus               = 45, /* - */
			Period              = 46, /* . */
			Slash               = 47, /* / */

			D0                  = 48, /* 0 */
			D1                  = 49, /* 1 */
			D2                  = 50, /* 2 */
			D3                  = 51, /* 3 */
			D4                  = 52, /* 4 */
			D5                  = 53, /* 5 */
			D6                  = 54, /* 6 */
			D7                  = 55, /* 7 */
			D8                  = 56, /* 8 */
			D9                  = 57, /* 9 */

			Semicolon           = 59, /* ; */
			Equal               = 61, /* = */

			A                   = 65,
			B                   = 66,
			C                   = 67,
			D                   = 68,
			E                   = 69,
			F                   = 70,
			G                   = 71,
			H                   = 72,
			I                   = 73,
			J                   = 74,
			K                   = 75,
			L                   = 76,
			M                   = 77,
			N                   = 78,
			O                   = 79,
			P                   = 80,
			Q                   = 81,
			R                   = 82,
			S                   = 83,
			T                   = 84,
			U                   = 85,
			V                   = 86,
			W                   = 87,
			X                   = 88,
			Y                   = 89,
			Z                   = 90,

			LeftBracket         = 91,  /* [ */
			Backslash           = 92,  /* \ */
			RightBracket        = 93,  /* ] */
			GraveAccent         = 96,  /* ` */

			World1              = 161, /* non-US #1 */
			World2              = 162, /* non-US #2 */

			/* Function keys */
			Escape              = 256,
			Enter               = 257,
			Tab                 = 258,
			Backspace           = 259,
			Insert              = 260,
			Delete              = 261,
			Right               = 262,
			Left                = 263,
			Down                = 264,
			Up                  = 265,
			PageUp              = 266,
			PageDown            = 267,
			Home                = 268,
			End                 = 269,
			CapsLock            = 280,
			ScrollLock          = 281,
			NumLock             = 282,
			PrintScreen         = 283,
			Pause               = 284,
			F1                  = 290,
			F2                  = 291,
			F3                  = 292,
			F4                  = 293,
			F5                  = 294,
			F6                  = 295,
			F7                  = 296,
			F8                  = 297,
			F9                  = 298,
			F10                 = 299,
			F11                 = 300,
			F12                 = 301,
			F13                 = 302,
			F14                 = 303,
			F15                 = 304,
			F16                 = 305,
			F17                 = 306,
			F18                 = 307,
			F19                 = 308,
			F20                 = 309,
			F21                 = 310,
			F22                 = 311,
			F23                 = 312,
			F24                 = 313,
			F25                 = 314,

			/* Keypad */
			KP0                 = 320,
			KP1                 = 321,
			KP2                 = 322,
			KP3                 = 323,
			KP4                 = 324,
			KP5                 = 325,
			KP6                 = 326,
			KP7                 = 327,
			KP8                 = 328,
			KP9                 = 329,
			KPDecimal           = 330,
			KPDivide            = 331,
			KPMultiply          = 332,
			KPSubtract          = 333,
			KPAdd               = 334,
			KPEnter             = 335,
			KPEqual             = 336,

			LeftShift           = 340,
			LeftControl         = 341,
			LeftAlt             = 342,
			LeftSuper           = 343,
			RightShift          = 344,
			RightControl        = 345,
			RightAlt            = 346,
			RightSuper          = 347,
			Menu                = 348
		};
	}
}

namespace Enik {

class KeyEvent : public Event {
public:
	KeyCode GetKeyCode() const { return m_KeyCode; }

	EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
protected:
	KeyEvent(const KeyCode keycode)
		: m_KeyCode(keycode) {}

	KeyCode m_KeyCode;
};



class KeyPressedEvent : public KeyEvent {
public:
	KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
		: KeyEvent(keycode), m_IsRepeat(isRepeat) {}

	bool IsRepeat() const { return m_IsRepeat; }

	std::string ToString() const override {
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyPressed)
private:
	bool m_IsRepeat;
};



class KeyReleasedEvent : public KeyEvent {
public:
	KeyReleasedEvent(const KeyCode keycode)
		: KeyEvent(keycode) {}

	std::string ToString() const override {
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_KeyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyReleased)
};



class KeyTypedEvent : public KeyEvent {
public:
	KeyTypedEvent(const KeyCode keycode)
		: KeyEvent(keycode) {}

	std::string ToString() const override {
		std::stringstream ss;
		ss << "KeyTypedEvent: " << m_KeyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyTyped)
};
}

namespace Enik {
	using MouseCode = uint16_t;

	namespace Mouse
	{
		enum : MouseCode
		{
			// From glfw3.h
			Button0                = 0,
			Button1                = 1,
			Button2                = 2,
			Button3                = 3,
			Button4                = 4,
			Button5                = 5,
			Button6                = 6,
			Button7                = 7,

			ButtonLast             = Button7,
			ButtonLeft             = Button0,
			ButtonRight            = Button1,
			ButtonMiddle           = Button2
		};
	}
}

namespace Enik {

class MouseMovedEvent : public Event {
   public:
	MouseMovedEvent(const float x, const float y)
		: m_MouseX(x), m_MouseY(y) {}

	float GetX() const { return m_MouseX; }
	float GetY() const { return m_MouseY; }

	std::string ToString() const override {
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseMoved)
	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
   private:
	float m_MouseX, m_MouseY;
};



class MouseScrolledEvent : public Event {
   public:
	MouseScrolledEvent(const float xOffset, const float yOffset)
		: m_XOffset(xOffset), m_YOffset(yOffset) {}

	float GetXOffset() const { return m_XOffset; }
	float GetYOffset() const { return m_YOffset; }

	std::string ToString() const override {
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseScrolled)
	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
   private:
	float m_XOffset, m_YOffset;
};



class MouseButtonEvent : public Event {
   public:
	MouseCode GetMouseButton() const { return m_Button; }

	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
   protected:
	MouseButtonEvent(const MouseCode button)
		: m_Button(button) {}

	MouseCode m_Button;
};



class MouseButtonPressedEvent : public MouseButtonEvent {
   public:
	MouseButtonPressedEvent(const MouseCode button)
		: MouseButtonEvent(button) {}

	std::string ToString() const override {
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonPressed)
};



class MouseButtonReleasedEvent : public MouseButtonEvent {
   public:
	MouseButtonReleasedEvent(const MouseCode button)
		: MouseButtonEvent(button) {}

	std::string ToString() const override {
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseButtonReleased)
};

}


namespace Enik {

class WindowResizeEvent : public Event {
public:
	WindowResizeEvent(unsigned int width, unsigned int height)
		: m_Width(width), m_Height(height) {}

	unsigned int GetWidth() const { return m_Width; }
	unsigned int GetHeight() const { return m_Height; }

	std::string ToString() const override {
		std::stringstream ss;
		ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
		return ss.str();
	}

	EVENT_CLASS_TYPE(WindowResize)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
private:
	unsigned int m_Width, m_Height;
};



class WindowCloseEvent : public Event {
public:
	WindowCloseEvent() = default;

	EVENT_CLASS_TYPE(WindowClose)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};



class AppTickEvent : public Event {
public:
	AppTickEvent() = default;

	EVENT_CLASS_TYPE(AppTick)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};



class AppUpdateEvent : public Event {
public:
	AppUpdateEvent() = default;

	EVENT_CLASS_TYPE(AppUpdate)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};



class AppRenderEvent : public Event {
public:
	AppRenderEvent() = default;

	EVENT_CLASS_TYPE(AppRender)
	EVENT_CLASS_CATEGORY(EventCategoryApplication)
};
}
// ??????????????????????????
// ????????? /EVENTS ????????
// ??????????????????????????


namespace Enik {

class ScriptableEntity {
public:
	virtual ~ScriptableEntity() {}

	template <typename T>
	__attribute__((noinline))
	T& Get() const {
		return m_Entity.Get<T>();
	}

	template <typename T>
	__attribute__((noinline))
	bool Has() const {
		return m_Entity.Has<T>();
	}

	template <typename T>
	T& GetOrAdd() {
		return m_Entity.GetOrAdd<T>();
	}

	virtual std::vector<NativeScriptField> OnEditorGetFields() {
		return std::vector<NativeScriptField>{};
	}

protected:
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnUpdate(Timestep ts) {}
	virtual void OnFixedUpdate() {}

	virtual void OnCollisionEnter(Entity& other) {}
	virtual void OnCollisionExit (Entity& other) {}
	virtual void OnSensorEnter(Entity& other) {}
	virtual void OnSensorExit (Entity& other) {}

	virtual void OnKeyPressed (const KeyPressedEvent&  event) { }
	virtual void OnKeyReleased(const KeyReleasedEvent& event) { }
	virtual void OnMouseButtonPressed (const MouseButtonPressedEvent&  event) { }
	virtual void OnMouseButtonReleased(const MouseButtonReleasedEvent& event) { }
	virtual void OnMouseScrolled(const MouseScrolledEvent& event) { }


protected:
	const std::string& GetTag() { return m_Entity.GetTag(); }
	const UUID&         GetID() { return m_Entity.GetID(); }

	bool                    HasFamily() { return m_Entity.HasFamily(); }
	Component::Family& GetOrAddFamily() { return m_Entity.GetOrAddFamily(); }
	bool                    HasParent() { return m_Entity.HasParent(); }
	Entity                  GetParent() { return m_Entity.GetParent(); }

	void   Reparent(Entity& new_parent) {        m_Entity.Reparent(new_parent); }
	std::vector<Entity>&  GetChildren() { return m_Entity.GetChildren(); }

	Entity CreateEntity(const std::string& name = "") { return m_Entity.CreateEntity(name); }

	void DestroyEntity(Entity entity) { m_Entity.DestroyEntity(entity); }

	Entity InstantiatePrefab(const std::filesystem::path& path) {
		return m_Entity.InstantiatePrefab(path);
	}

	void CloseApplication() {
		m_Entity.CloseApplication();
	}

	void ChangeScene(const std::string& path) { m_Entity.ChangeScene(path); }


protected:
	RaycastResult CastRay(Raycast ray);
// 		{ return m_Entity.m_Scene->m_Physics.CastRay(ray); }


	Entity m_Entity;
	friend class Scene;
};
}


namespace Enik {
}










//********************************************
//**************extern_functions**************
//********************************************
// ? only the header is included here

namespace Enik {

extern "C" Entity FindEntityByUUID(UUID uuid);

extern "C" Entity FindEntityByName(const std::string& name);

template <typename T>
T* GetScriptInstance(Entity entity) {
	return (T*)entity.Get<Component::NativeScript>().Instance;
}

}

//********************************************
//*****************DEBUG LOGS*****************
//********************************************
namespace Enik {

#if !EN_STATIC_SCRIPT_MODULE
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CONSOLE_PRINT_LINE_INFO std::cout << \
	"                    \033[0;40;90min: " + std::string(std::filesystem::path(__FILE__).filename()) + \
	"    at line: " + std::string(TOSTRING(__LINE__)) + "\033[0m\n";

#define CONSOLE_DEBUG(msg)      Enik::Console::Trace(msg);      CONSOLE_PRINT_LINE_INFO
#define CONSOLE_DEBUGS(msg,txt) Enik::Console::Trace(msg, txt); CONSOLE_PRINT_LINE_INFO
#define CONSOLE_DEBUG_ERROR(msg)      Enik::Console::Error(msg);      CONSOLE_PRINT_LINE_INFO
#define CONSOLE_DEBUGS_ERROR(msg,txt) Enik::Console::Error(msg, txt); CONSOLE_PRINT_LINE_INFO

#endif

}

#endif // EN_STATIC_SCRIPT_MODULE
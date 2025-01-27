#include "physics.h"

#include "core/log.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "script_system/extern_functions.h"

#include "Jolt/Math/MathTypes.h"
#include "Jolt/Math/Real.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "Jolt/Physics/EActivation.h"
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/RegisterTypes.h>

namespace Enik {
using namespace JPH;

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers
{
	static constexpr ObjectLayer NON_MOVING = 0;
	static constexpr ObjectLayer MOVING = 1;
	static constexpr ObjectLayer NUM_LAYERS = 2;
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
	virtual bool	ShouldCollide(ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::NON_MOVING:
			return inObject2 == Layers::MOVING; // Non moving only collides with moving
		case Layers::MOVING:
			return true; // Moving collides with everything
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
	static constexpr BroadPhaseLayer NON_MOVING(0);
	static constexpr BroadPhaseLayer MOVING(1);
	static constexpr uint NUM_LAYERS(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
									BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual BroadPhaseLayer			GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char *			GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
	{
		switch ((BroadPhaseLayer::Type)inLayer)
		{
		case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
			return inLayer2 == BroadPhaseLayers::MOVING;
		case Layers::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}
};

class MyContactListener : public ContactListener
{
public:
	virtual void OnContactAdded(const Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
	{
// 		Entity e1 = FindEntityByUUID(inBody1.GetUserData());
// 		Entity e2 = FindEntityByUUID(inBody2.GetUserData());
// 		EN_CORE_INFO("Collision {}, {}", e1.GetTag(), e2.GetTag());
	}

// 	virtual void OnContactPersisted(const Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
// 	{
// // 		std::cout << "A contact was persisted" << std::endl;
// 	}

	virtual void OnContactRemoved(const SubShapeIDPair &inSubShapePair) override
	{
	}
};









void Physics::Initialize(entt::registry& Registry, class Scene* scene, float delta_time) {
	m_delta_time = delta_time;
	m_Registry = &Registry;
	m_Scene = scene;

	RegisterDefaultAllocator();
	Factory::sInstance = new Factory();
	RegisterTypes();

	m_broad_phase_layer_interface = new BPLayerInterfaceImpl();

	// Create class that filters object vs broadphase layers
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	// Also have a look at ObjectVsBroadPhaseLayerFilterTable or ObjectVsBroadPhaseLayerFilterMask for a simpler interface.
	m_object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();

	// Create class that filters object vs object layers
	// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
	// Also have a look at ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler interface.
	m_object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();

	m_contact_listener = new MyContactListener();

	// Now we can create the actual physics system.
	m_PhysicsSystem = new PhysicsSystem();
	m_PhysicsSystem->Init(
		m_max_bodies, m_num_body_mutexes, m_max_body_pairs, m_max_contact_constraints,
		*m_broad_phase_layer_interface, *m_object_vs_broadphase_layer_filter, *m_object_vs_object_layer_filter
	);

	m_PhysicsSystem->SetContactListener(m_contact_listener);

	m_job_system = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

	m_is_initialized = true;
}

void Physics::Uninitialize() {
	if (!m_is_initialized) {
		return;
	}
	m_is_initialized = false;

	UnregisterTypes();

	delete Factory::sInstance;
	Factory::sInstance = nullptr;

	delete m_broad_phase_layer_interface;
	delete m_object_vs_broadphase_layer_filter;
	delete m_object_vs_object_layer_filter;
	delete m_contact_listener;


	m_Registry->each([&](entt::entity entity) {
		if (m_Registry->any_of<Component::RigidBody>(entity)) {
			auto& body = m_Registry->get<Component::RigidBody>(entity);
			body.body = nullptr;
		}
		if (m_Registry->any_of<Component::CollisionBody>(entity)) {
			auto& body = m_Registry->get<Component::CollisionBody>(entity);
			body.body = nullptr;
		}
		if (m_Registry->any_of<Component::CollisionShape>(entity)) {
			auto& cs = m_Registry->get<Component::CollisionShape>(entity);
			if (cs.shape != nullptr) {
				cs.shape->Release();
				cs.shape = nullptr;
			}
		}
	});
}



void Physics::UpdatePhysics() {
	if (!m_is_initialized) {
		return;
	}

	m_PhysicsSystem->Update(m_delta_time, 2, &m_temp_allocator, m_job_system);

	SyncTransforms<Component::RigidBody>();
	SyncTransforms<Component::CollisionBody>();
}

template<typename T>
void Physics::SyncTransforms() {
	auto group = m_Registry->group<T>(entt::get<Component::Transform>);
	for (auto entity : group) {
		auto& tr = group.template get<Component::Transform>(entity);

		T& body = group.template get<T>(entity);

		if (body.body != nullptr) {
			const Vec3& pos = body.body->GetPosition();
			const Quat& rot = body.body->GetRotation();

			const glm::vec3 global_pos = glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
			const glm::quat global_rot = glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ());

			if (m_Registry->all_of<Component::Family>(entity)) {
				Component::Family& family = m_Registry->get<Component::Family>(entity);
				family.SetGlobalPositionRotation(tr, global_pos, global_rot);
			} else {
				tr.LocalPosition = global_pos;
				tr.LocalRotation = global_rot;
			}
		}
	}
}


void Physics::CreatePhysicsWorld() {
	{
		auto group = m_Registry->group<Component::RigidBody>(entt::get<Component::Transform>);
		for (auto entity : group) {
			Component::Transform& tr = group.get<Component::Transform>(entity);
			Component::RigidBody& rb = group.get<Component::RigidBody>(entity);
			if (rb.body == nullptr) {
				CreatePhysicsBody(Entity(entity, m_Scene), tr, rb);
			}
		}
	}
	{
		auto group = m_Registry->group<Component::CollisionBody>(entt::get<Component::Transform>);
		for (auto entity : group) {
			Component::Transform& tr = group.get<Component::Transform>(entity);
			Component::CollisionBody& cb = group.get<Component::CollisionBody>(entity);
			if (cb.body == nullptr) {
				CreatePhysicsBody(Entity(entity, m_Scene), tr, cb);
			}
		}
	}
	m_PhysicsSystem->OptimizeBroadPhase();
}


void Physics::CreatePhysicsBody(Entity entity, const Component::Transform& tr, Component::RigidBody& body) {
	Shape* shape = CreateShapeForBody(entity);
	if (shape == nullptr) {
		return;
	}

	BodyCreationSettings bcs{
		shape,
		RVec3Arg(tr.GlobalPosition.x, tr.GlobalPosition.y, tr.GlobalPosition.z),
		QuatArg(tr.GlobalRotation.x, tr.GlobalRotation.y, tr.GlobalRotation.z, tr.GlobalRotation.w),
		body.MotionType,
		static_cast<ObjectLayer>(body.Layer)
	};
	bcs.mAllowedDOFs = EAllowedDOFs::Plane2D;
	bcs.mGravityFactor = body.GetGravityFactor();

	Body* new_body = m_PhysicsSystem->GetBodyInterface().CreateBody(bcs);
	new_body->GetMotionProperties()->ScaleToMass(body.GetMass());
	new_body->SetUserData(entity.GetID());
	m_PhysicsSystem->GetBodyInterface().AddBody(new_body->GetID(), EActivation::Activate);

	body.body = new_body;
}
void Physics::CreatePhysicsBody(Entity entity, const Component::Transform& tr, Component::CollisionBody& body) {
	Shape* shape = CreateShapeForBody(entity);
	if (shape == nullptr) {
		return;
	}

	BodyCreationSettings bcs{
		shape,
		RVec3Arg(tr.GlobalPosition.x, tr.GlobalPosition.y, tr.GlobalPosition.z),
		QuatArg(tr.GlobalRotation.x, tr.GlobalRotation.y, tr.GlobalRotation.z, tr.GlobalRotation.w),
		body.MotionType,
		static_cast<ObjectLayer>(body.Layer)
	};
	bcs.mAllowedDOFs = EAllowedDOFs::Plane2D;
	bcs.mIsSensor = body.IsSensor;

	Body* new_body = m_PhysicsSystem->GetBodyInterface().CreateBody(bcs);
	new_body->SetUserData(entity.GetID());
	m_PhysicsSystem->GetBodyInterface().AddBody(new_body->GetID(), EActivation::Activate);

	body.body = new_body;
}


JPH::Ref<JPH::Shape> Physics::CreateShapeForBody(Entity entity) {
	// TODO: composite shapes

	if (entity.Has<Component::CollisionShape>()) {
		Component::CollisionShape& cs = entity.Get<Component::CollisionShape>();
		Component::Transform& tr = entity.Get<Component::Transform>();

		switch (cs.Shape) {
			case Component::CollisionShape::Type::BOX: {
				auto scale = cs.BoxScale * tr.GlobalScale;
				BoxShapeSettings shape(Vec3(scale.x, scale.y, scale.z));
				cs.shape = shape.Create().Get();
				cs.shape->AddRef();
				return cs.shape;
			}
			case Component::CollisionShape::Type::CIRCLE: {
				SphereShapeSettings shape(cs.CircleRadius);
				cs.shape = shape.Create().Get();
				cs.shape->AddRef();
				return cs.shape;
			}
			case Component::CollisionShape::Type::NONE: break;
		}
	}

	BoxShapeSettings shape(Vec3(0.5f, 0.5f, 0.5f));
	auto s = shape.Create().Get();
	s->AddRef();
	return s;
}


}

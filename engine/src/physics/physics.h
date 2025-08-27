#pragma once
#include "base.h"
#include "scene/components.h"
#include <entt/entt.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>


namespace Enik {

constexpr float PHYSICS_UPDATE_RATE = 1.0/60.0;


struct RaycastResult;
struct Raycast;


class Physics {
public:
	bool m_is_initialized = false;
	void Initialize(entt::registry& Registry, class Scene* scene);
	void Uninitialize();

	void UpdatePhysics();
	void CreatePhysicsWorld();

	void RemovePhysicsBody(JPH::BodyID bodyID);
	void RemovePhysicsBody(JPH::Body* body);

	JPH::PhysicsSystem* GetPhysicsSystem() const { return m_PhysicsSystem; }

	void DeferOnExit(JPH::BodyID a, JPH::BodyID b);
	void DeferOnEnter(const JPH::Body& a, const JPH::Body& b);

	RaycastResult CastRay(const Raycast& ray);

	template<typename T>
	void SyncTransforms();
private:
	void CreatePhysicsBody(Entity entity, const Component::Transform& tr, Component::RigidBody& body);
	void CreatePhysicsBody(Entity entity, const Component::Transform& tr, Component::CollisionBody& body);
	JPH::Ref<JPH::Shape> CreateShapeForBody(Entity entity);

	void ProcessDeferredOnExitSignals();
	void ProcessDeferredOnEnterSignals();

private:
	JPH::PhysicsSystem* m_PhysicsSystem = nullptr;
	JPH::JobSystemThreadPool* m_job_system;
	JPH::TempAllocator * m_temp_allocator = nullptr;

	entt::registry* m_Registry = nullptr;
	class Scene* m_Scene = nullptr;


	const int m_max_bodies = 1024;
	const int m_num_body_mutexes = 0;
	const int m_max_body_pairs = 4096;
	const int m_max_contact_constraints = 4096;

	JPH::BroadPhaseLayerInterface*      m_broad_phase_layer_interface;
	JPH::ObjectVsBroadPhaseLayerFilter* m_object_vs_broadphase_layer_filter;
	JPH::ObjectLayerPairFilter*         m_object_vs_object_layer_filter;
	JPH::ContactListener*               m_contact_listener;

	std::vector<std::pair<JPH::BodyID, JPH::BodyID>> m_DeferredOnExits = {};
	std::vector<std::pair<const JPH::Body*, const JPH::Body*>> m_DeferredOnEnters = {};
	std::mutex m_DeferredOnExitMutex;
	std::mutex m_DeferredOnEnterMutex;

};


}

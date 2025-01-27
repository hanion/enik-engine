#pragma once
#include "base.h"
#include "scene/components.h"
#include <entt/entt.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>


namespace Enik {

constexpr double PHYSICS_UPDATE_RATE = 1.0/144.0;


class Physics {
public:
	bool m_is_initialized = false;
	void Initialize(entt::registry& Registry, class Scene* scene, float delta_time = PHYSICS_UPDATE_RATE);
	void Uninitialize();

	void UpdatePhysics();
	void CreatePhysicsWorld();

	void RemovePhysicsBody(JPH::BodyID bodyID);
	void RemovePhysicsBody(JPH::Body* body);

	JPH::PhysicsSystem* GetPhysicsSystem() const { return m_PhysicsSystem; }

private:
	template<typename T>
	void SyncTransforms();
	void CreatePhysicsBody(Entity entity, const Component::Transform& tr, Component::RigidBody& body);
	void CreatePhysicsBody(Entity entity, const Component::Transform& tr, Component::CollisionBody& body);
	JPH::Ref<JPH::Shape> CreateShapeForBody(Entity entity);

private:
	JPH::PhysicsSystem* m_PhysicsSystem = nullptr;
	JPH::JobSystemThreadPool* m_job_system;
	JPH::TempAllocatorMalloc m_temp_allocator;
// 	JPH::TempAllocatorImpl m_temp_allocator;

	entt::registry* m_Registry = nullptr;
	class Scene* m_Scene = nullptr;

	float m_delta_time = PHYSICS_UPDATE_RATE;


	const int m_max_bodies = 1024;
	const int m_num_body_mutexes = 0;
	const int m_max_body_pairs = 1024;
	const int m_max_contact_constraints = 1024;

	JPH::BroadPhaseLayerInterface*      m_broad_phase_layer_interface;
	JPH::ObjectVsBroadPhaseLayerFilter* m_object_vs_broadphase_layer_filter;
	JPH::ObjectLayerPairFilter*         m_object_vs_object_layer_filter;
	JPH::ContactListener*               m_contact_listener;
};


}

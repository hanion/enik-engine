#include "physics_world.h"
#include "scene/components.h"
#include "collision.h"
#include "script_system/script_system.h"

namespace Enik {

static PhysicsWorld::Data s_Data;

void PhysicsWorld::InitPhysicsWorld(entt::registry* registry) {
	s_Data.Registry = registry;
}

void PhysicsWorld::Step() {
	if (s_Data.Registry == nullptr) {
		return;
	}
	ResolveCollisions();

	auto group = s_Data.Registry->group<Component::RigidBody>(entt::get<Component::Transform>);
	for (auto entity : group) {
		Component::Transform& transform  = group.get<Component::Transform>(entity);
		Component::RigidBody& rigid_body = group.get<Component::RigidBody>(entity);

		// gravity
		if (rigid_body.UseGravity) {
			rigid_body.Force.y += rigid_body.Mass * s_Data.Gravity.y;
		}

		if (rigid_body.Awake) {
			auto half_acceleration = (rigid_body.Force / rigid_body.Mass) * GetFixedUpdateRate() * 0.5f;
			rigid_body.Velocity += half_acceleration;
			transform.LocalPosition += rigid_body.Velocity * GetFixedUpdateRate();
			rigid_body.Velocity += half_acceleration;
		}

		// reset force
		rigid_body.Force = glm::vec3(0);

	}
}

void PhysicsWorld::ResolveCollisions() {
	std::vector<Collision> collisions;

	auto group = s_Data.Registry->group<Component::Collider>(entt::get<Component::Transform>);
	for (auto a : group) {
		for (auto b : group) {
			if (a == b) {
				break;
			}

			Component::Transform* a_transform  = &group.get<Component::Transform>(a);
			Component::Collider*  a_collider   = &group.get<Component::Collider> (a);

			Component::Transform* b_transform  = &group.get<Component::Transform>(b);
			Component::Collider*  b_collider   = &group.get<Component::Collider> (b);

			CollisionPoints points = TestCollision(
				a_collider, a_transform,
				b_collider, b_transform
			);

			if (points.HasCollision) {
				Collision col;
				col.EntityA = Entity(a, ScriptSystem::GetSceneContext()),
				col.EntityB = Entity(b, ScriptSystem::GetSceneContext()),
				col.Points = points;
				collisions.emplace_back(col);
			}
		}
	}

	for (auto& collision : collisions) {
		if (not collision.Points.HasCollision) {
			continue;
		}

		Entity& a = collision.EntityA;
		Entity& b = collision.EntityB;
		CollisionPoints& points = collision.Points;


		if (not a or not b) { continue; }
		if (a.Has<Component::NativeScript>()) {
			if (a.GetScriptInstance() != nullptr) {
				a.GetScriptInstance()->OnCollision(b);
			}
		}

		if (not a or not b) { continue; }
		if (b.Has<Component::NativeScript>()) {
			if (b.GetScriptInstance() != nullptr) {
				b.GetScriptInstance()->OnCollision(a);
			}
		}

		if (not a or not b) { continue; }
		if (not a.Has<Component::Collider>() or
			not b.Has<Component::Collider>()) {
			continue;
		}

		if (a.Get<Component::Collider>().IsArea or
			b.Get<Component::Collider>().IsArea) {
			continue;
		}


		// Calculate the separation vector
		glm::vec3 separation = points.Normal * points.Depth;
		// NOTE: lock all bodies in 2d
		separation.z = 0.0f;
		// FIXME: ! adding energy to the system ???
		static constexpr float bounce = 200.0f;

		if (a.Has<Component::RigidBody>()) {
			a.Get<Component::Transform>().LocalPosition -= separation;
			auto rb = &a.Get<Component::RigidBody>();
			rb->Velocity *= 0.9999f;
			rb->ApplyImpulse(-separation * bounce);
		}
		if (b.Has<Component::RigidBody>()) {
			b.Get<Component::Transform>().LocalPosition += separation;
			auto rb = &b.Get<Component::RigidBody>();
			rb->Velocity *= 0.9999f;
			rb->ApplyImpulse(+separation * bounce);
		}
	}
}

}
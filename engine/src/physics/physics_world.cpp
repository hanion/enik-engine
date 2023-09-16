#include "physics_world.h"
#include "scene/components.h"
#include "collision.h"
#include "script_system/script_system.h"

namespace Enik {

void PhysicsWorld::Step(Timestep ts, entt::registry& reg) {

	ResolveCollisions(ts, reg);

	auto group = reg.group<Component::RigidBody>(entt::get<Component::Transform>);
	for (auto entity : group) {
		Component::Transform& transform  = group.get<Component::Transform>(entity);
		Component::RigidBody& rigid_body = group.get<Component::RigidBody>(entity);

		// gravity
		rigid_body.Force += rigid_body.Mass * m_Gravity;

		rigid_body.Velocity += rigid_body.Force / rigid_body.Mass * ts.GetSeconds();
		transform.Position += rigid_body.Velocity * ts.GetSeconds();

		// reset force
		rigid_body.Force = glm::vec3(0);

		// TODO damping
	}
}

void PhysicsWorld::ResolveCollisions(Timestep ts, entt::registry& reg) {
	std::vector<Collision> collisions;

	auto group = reg.group<Component::Collider>(entt::get<Component::Transform>);
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

		Entity a = collision.EntityA;
		Entity b = collision.EntityB;
		CollisionPoints& points = collision.Points;


		// ? temp debug
		auto& a_text = a.Get<Component::Tag>().Text;
		if (a_text != "platform") {
			EN_CORE_WARN("collision {0} {1}", a_text, b.Get<Component::Tag>().Text);
		}

		// Calculate the separation vector
		glm::vec3 separation = points.Normal * points.Depth;

		// ! FIXME balls slowly go inside the plane
		// Apply forces or impulses to simulate the collision response
		if (a.Has<Component::RigidBody>()) {
			a.Get<Component::Transform>().Position -= separation;
			a.Get<Component::RigidBody>().Velocity *= 0.99f;
			a.Get<Component::RigidBody>().Velocity -= separation;
		}
		if (b.Has<Component::RigidBody>()) {
			b.Get<Component::Transform>().Position += separation;
			b.Get<Component::RigidBody>().Velocity *= 0.99f;
			b.Get<Component::RigidBody>().Velocity += separation;
		}
	}
}

}
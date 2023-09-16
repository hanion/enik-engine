#pragma once
#include <glm/glm.hpp>
#include "scene/components.h"


namespace Enik {

struct CollisionPoints {
	glm::vec3 A; // Furthest point of A into B
	glm::vec3 B; // Furthest point of B into A
	glm::vec3 Normal; // B – A normalized
	float Depth;    // Length of B – A
	bool HasCollision = false;
};

struct Collision {
	Entity EntityA;
	Entity EntityB;
	CollisionPoints Points;
};

CollisionPoints TestCollision(
	const Component::Collider* a, const Component::Transform* at,
	const Component::Collider* b, const Component::Transform* bt);



CollisionPoints Test_Sphere_Sphere(
	const Component::Collider* a, const Component::Transform* ta,
	const Component::Collider* b, const Component::Transform* tb);

CollisionPoints Test_Sphere_Plane(
	const Component::Collider* a, const Component::Transform* ta,
	const Component::Collider* b, const Component::Transform* tb);



}
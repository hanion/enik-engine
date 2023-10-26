#pragma once
#include <glm/glm.hpp>
#include "scene/components.h"
#include "scene/entity.h"


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
	const Component::Collider* a, const Component::Transform* a_transform,
	const Component::Collider* b, const Component::Transform* b_transform);



CollisionPoints TestCircleCircle(
	const Component::Collider* a, const Component::Transform* a_transform,
	const Component::Collider* b, const Component::Transform* b_transform);

CollisionPoints TestCirclePlane(
	const Component::Collider* circle, const Component::Transform* circle_transform,
	const Component::Collider* plane, const Component::Transform* plane_transform);



CollisionPoints TestCircleBox(
	const Component::Collider* circle, const Component::Transform* circle_transform,
	const Component::Collider* box, const Component::Transform* box_transform);

CollisionPoints TestPlaneBox(
	const Component::Collider* plane, const Component::Transform* plane_transform,
	const Component::Collider* box, const Component::Transform* box_transform);


CollisionPoints TestBoxBox(
	const Component::Collider* a, const Component::Transform* a_transform,
	const Component::Collider* b, const Component::Transform* b_transform);



}
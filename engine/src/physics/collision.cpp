#include "collision.h"

namespace Enik {

using FindContactFunc = CollisionPoints (*)(
	const Component::Collider*, const Component::Transform*,
	const Component::Collider*, const Component::Transform*);

CollisionPoints TestCollision(
	const Component::Collider* a, const Component::Transform* at,
	const Component::Collider* b, const Component::Transform* bt) {
	static const FindContactFunc tests[2][2] = {
		// Sphere             Plane
		{Test_Sphere_Sphere, Test_Sphere_Plane},  // Sphere
		{nullptr, nullptr}                        // Plane
	};
	// If we are passed a Plane vs Sphere, swap the
	// colliders so it's a Sphere vs Plane
	bool swap = b->Shape < a->Shape;

	if (swap) {
		std::swap(a, b);
		std::swap(at, bt);
	}
	// now we can dispatch the correct function
	CollisionPoints points = tests[a->Shape][b->Shape](a, at, b, bt);

	// if we swapped the order of the colliders, to keep the
	// results consistent, we need to swap the points
	if (swap) {
		std::swap(points.A, points.B);
		points.Normal = -points.Normal;
	}

	return points;
}



CollisionPoints Test_Sphere_Sphere(
	const Component::Collider* a, const Component::Transform* ta,
	const Component::Collider* b, const Component::Transform* tb) {
	CollisionPoints point;

	glm::vec3 a_center = (ta->Position + a->vector);
	glm::vec3 b_center = (tb->Position + b->vector);

	glm::vec3 diff = b_center - a_center;
	auto distance = glm::length(diff);
	// auto distance = (float)diff.length();

	float a_radius = a->flat;
	float b_radius = b->flat;

	if (distance < a_radius + b_radius) {
		// Spheres are colliding

		// Calculate collision normal, depth, and collision points
		glm::vec3 normal = glm::normalize(diff);
		float depth = a_radius + b_radius - distance;
		glm::vec3 a_point = a_center + a_radius * normal;
		glm::vec3 b_point = b_center - b_radius * normal;

		// Fill in the CollisionPoints structure
		point.A = a_point;
		point.B = b_point;
		point.Normal = normal;
		point.Depth = depth;
		point.HasCollision = true;
	}

	return point;
}


CollisionPoints Test_Sphere_Plane(
	const Component::Collider* a, const Component::Transform* ta,
	const Component::Collider* b, const Component::Transform* tb) {
	CollisionPoints result;

	// Extract relevant information from the sphere and plane colliders and transforms
	glm::vec3 sphere_center = ta->Position + a->vector;
	float sphere_radius = a->flat;

	glm::vec3 plane_normal = b->vector;
	float plane_d = tb->Position.y + b->flat;

	// Calculate the signed distance from the sphere center to the plane
	float distance = glm::dot(sphere_center, plane_normal) - plane_d;

	// Check for collision
	if (distance < sphere_radius) {
		// Calculate collision normal and depth
		result.Normal = -plane_normal;  // Normal points away from the plane
		result.Depth = sphere_radius - distance;

		// Calculate collision points
		result.A = sphere_center - sphere_radius * plane_normal;  // Point on sphere's surface
		result.B = sphere_center - distance * plane_normal;       // Point on the plane

		result.HasCollision = true;
	}

	return result;
}

}
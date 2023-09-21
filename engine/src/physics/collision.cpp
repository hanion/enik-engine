#include "collision.h"

namespace Enik {

using FindContactFunc = CollisionPoints (*)(
	const Component::Collider*, const Component::Transform*,
	const Component::Collider*, const Component::Transform*);

CollisionPoints TestCollision(
	const Component::Collider* a, const Component::Transform* a_transform,
	const Component::Collider* b, const Component::Transform* b_transform) {
	static const FindContactFunc tests[3][3] = {
		// Circle           Plane            Box
		{ TestCircleCircle, TestCirclePlane, TestCircleBox }, // Circle
		{ nullptr,          nullptr,         TestPlaneBox  }, // Plane
		{ nullptr,          nullptr,         TestBoxBox    }, // Box
	};

	if (b->Shape == a->Shape and b->Shape == Component::ColliderShape::PLANE) {
		return CollisionPoints();
	}


	// If we are passed a Plane vs Sphere, swap the
	// colliders so it's a Sphere vs Plane
	bool swap = a->Shape > b->Shape;

	if (swap) {
		std::swap(a, b);
		std::swap(a_transform, b_transform);
	}
	// now we can dispatch the correct function
	CollisionPoints points = tests[a->Shape][b->Shape](
		a, a_transform,
		b, b_transform);

	// if we swapped the order of the colliders, to keep the
	// results consistent, we need to swap the points
	if (swap) {
		std::swap(points.A, points.B);
		points.Normal = -points.Normal;
	}

	return points;
}



CollisionPoints TestCircleCircle(
	const Component::Collider* a, const Component::Transform* a_transform,
	const Component::Collider* b, const Component::Transform* b_transform) {

	CollisionPoints result;

	if (a->Shape != Component::ColliderShape::CIRCLE)  { return result; }
	if (b->Shape != Component::ColliderShape::CIRCLE)  { return result; }


	glm::vec3 a_center = (a_transform->Position + a->Vector);
	glm::vec3 b_center = (b_transform->Position + b->Vector);

	glm::vec3 diff = b_center - a_center;
	auto distance = glm::length(diff);
	// auto distance = (float)diff.length();

	float a_radius = a->Float;
	float b_radius = b->Float;

	if (distance < a_radius + b_radius) {
		// Spheres are colliding

		// Calculate collision normal, depth, and collision points
		glm::vec3 normal = glm::normalize(diff);
		float depth = a_radius + b_radius - distance;
		glm::vec3 a_point = a_center + a_radius * normal;
		glm::vec3 b_point = b_center - b_radius * normal;

		// Fill in the CollisionPoints structure
		result.A = a_point;
		result.B = b_point;
		result.Normal = normal;
		result.Depth = depth;
		result.HasCollision = true;
	}

	return result;
}


CollisionPoints TestCirclePlane(
	const Component::Collider* circle, const Component::Transform* circle_transform,
	const Component::Collider* plane,  const Component::Transform* plane_transform) {

	CollisionPoints result;

	if (circle->Shape != Component::ColliderShape::CIRCLE)  { return result; }
	if (plane->Shape  != Component::ColliderShape::PLANE )  { return result; }

	glm::vec3 circle_center = circle_transform->Position + circle->Vector;
	float circle_radius = circle->Float;

	glm::vec3 plane_normal = plane->Vector;
	float plane_d = plane_transform->Position.y + plane->Float;

	float distance = glm::dot(circle_center, plane_normal) - plane_d;

	if (distance < circle_radius) {
		result.Normal = -plane_normal;
		result.Depth = circle_radius - distance;

		result.A = circle_center - circle_radius * plane_normal;  // Point on circle's surface
		result.B = circle_center - distance * plane_normal;       // Point on the plane

		result.HasCollision = true;
	}

	return result;
}

CollisionPoints TestCircleBox(
	const Component::Collider* circle, const Component::Transform* circle_transform,
	const Component::Collider* box,    const Component::Transform* box_transform) {

	CollisionPoints result;

	if (circle->Shape != Component::ColliderShape::CIRCLE)  { return result; }
	if (box->Shape    != Component::ColliderShape::BOX   )  { return result; }

	return result;
}

CollisionPoints TestPlaneBox(
	const Component::Collider* plane, const Component::Transform* plane_transform,
	const Component::Collider* box,   const Component::Transform* box_transform) {

	CollisionPoints result;

	if (plane->Shape != Component::ColliderShape::PLANE)  { return result; }
	if (box->Shape   != Component::ColliderShape::BOX  )  { return result; }



	return result;
}


CollisionPoints TestBoxBox(
	const Component::Collider* a, const Component::Transform* a_transform,
	const Component::Collider* b, const Component::Transform* b_transform) {

	CollisionPoints result;

	if (a->Shape != Component::ColliderShape::BOX)  { return result; }
	if (b->Shape != Component::ColliderShape::BOX)  { return result; }


	return result;
}
}
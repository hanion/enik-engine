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

	float a_radius = a->Float * a_transform->Scale.x;
	float b_radius = b->Float * b_transform->Scale.x;

	if (distance < a_radius + b_radius) {
		glm::vec3 normal = glm::normalize(diff);
		float depth = a_radius + b_radius - distance;
		glm::vec3 a_point = a_center + a_radius * normal;
		glm::vec3 b_point = b_center - b_radius * normal;

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
	float circle_radius = circle->Float * circle_transform->Scale.x;

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

	glm::vec3 box_scale = glm::vec3(box_transform->Scale.x * 0.5f, box_transform->Scale.y * 0.5f, 0);

	glm::vec3 closest_point_on_box = glm::clamp(
		circle_transform->Position,
		box_transform->Position - box_scale,
		box_transform->Position + box_scale
	);

	glm::vec3 circle_to_closest_point = closest_point_on_box - circle_transform->Position;

	float distance = glm::length(circle_to_closest_point);

	float circle_radius = circle->Float * circle_transform->Scale.x;

	if (distance <= circle_radius) {
		result.Depth = circle_radius - distance;
		result.Normal = glm::normalize(circle_to_closest_point);

		result.HasCollision = true;
	}

	return result;
}

CollisionPoints TestPlaneBox(
	const Component::Collider* plane, const Component::Transform* plane_transform,
	const Component::Collider* box,   const Component::Transform* box_transform) {

	CollisionPoints result;

	if (plane->Shape != Component::ColliderShape::PLANE)  { return result; }
	if (box->Shape   != Component::ColliderShape::BOX  )  { return result; }


	float distance = glm::dot(box_transform->Position - plane_transform->Position, plane->Vector);

	if (distance <= box_transform->Scale.y) {
		result.Depth = box_transform->Scale.y - distance;
		result.Normal = plane->Vector;
		result.HasCollision = true;
	}

	return result;
}


void CalculateBoxVertices(const glm::vec3& position, const glm::vec2& half_extents,
	float rotation_angle_radians, glm::vec2 out_vertices[4]) {

	glm::mat2 rotation_matrix(
		cos(rotation_angle_radians), -sin(rotation_angle_radians),
		sin(rotation_angle_radians),  cos(rotation_angle_radians)
	);

	glm::vec2 local_vertices[4] = {
		glm::vec2(-half_extents.x, -half_extents.y),
		glm::vec2( half_extents.x, -half_extents.y),
		glm::vec2( half_extents.x,  half_extents.y),
		glm::vec2(-half_extents.x,  half_extents.y)
	};

	for (int i = 0; i < 4; ++i) {
		out_vertices[i] = glm::vec2(position.x, position.y) + rotation_matrix * local_vertices[i];
	}
}

void ProjectVerticesOntoAxis(const glm::vec2 vertices[], const glm::vec2& axis,
	float& out_min_projection, float& out_max_projection) {

	out_min_projection = out_max_projection = glm::dot(vertices[0], axis);

	for (int i = 1; i < 4; ++i) {
		float projection = glm::dot(vertices[i], axis);
		if (projection < out_min_projection) {
			out_min_projection = projection;
		}
		if (projection > out_max_projection) {
			out_max_projection = projection;
		}
	}
}


CollisionPoints TestBoxBox(
	const Component::Collider* a, const Component::Transform* a_transform,
	const Component::Collider* b, const Component::Transform* b_transform) {

	CollisionPoints result;

	if (a->Shape != Component::ColliderShape::BOX)  { return result; }
	if (b->Shape != Component::ColliderShape::BOX)  { return result; }


	const glm::vec3& a_position = a_transform->Position;
	const glm::vec3& b_position = b_transform->Position;

	glm::vec2 a_half_extents = a_transform->Scale * 0.5f;
	glm::vec2 b_half_extents = b_transform->Scale * 0.5f;

	glm::vec2 a_vertices[4];
	glm::vec2 b_vertices[4];

	CalculateBoxVertices(a_position, a_half_extents, glm::radians(a_transform->Rotation), a_vertices);
	CalculateBoxVertices(b_position, b_half_extents, glm::radians(b_transform->Rotation), b_vertices);

	bool overlap_on_all_axes = true;

	for (int i = 0; i < 4; ++i) {
		glm::vec2 axis = a_vertices[(i + 1) % 4] - a_vertices[i];
		axis = glm::normalize(glm::vec2(-axis.y, axis.x));

		float a_min, a_max;
		float b_min, b_max;
		ProjectVerticesOntoAxis(a_vertices, axis, a_min, a_max);
		ProjectVerticesOntoAxis(b_vertices, axis, b_min, b_max);

		if (not (a_max >= b_min && b_max >= a_min)) {
			overlap_on_all_axes = false;
			break; // No overlap on this axis, exit early
		}
	}

	if (overlap_on_all_axes) {

		float depth_x = a_half_extents.x + b_half_extents.x - glm::abs(a_position.x - b_position.x);
		float depth_y = a_half_extents.y + b_half_extents.y - glm::abs(a_position.y - b_position.y);

		if (depth_x < depth_y) {
			result.Depth = depth_x;
			if (a_position.x < b_position.x) {
				result.Normal = glm::vec3( 1.0f, 0.0f, 0.0f); // Collision from left
			} else {
				result.Normal = glm::vec3(-1.0f, 0.0f, 0.0f); // Collision from right
			}
		} else {
			result.Depth = depth_y;
			if (a_position.y < b_position.y) {
				result.Normal = glm::vec3(0.0f,  1.0f, 0.0f); // Collision from below
			} else {
				result.Normal = glm::vec3(0.0f, -1.0f, 0.0f); // Collision from above
			}
		}

		result.HasCollision = true;
	}

	return result;
}
}
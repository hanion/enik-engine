#pragma once
#include <vector>
#include "core/timestep.h"
#include "glm/glm.hpp"
#include <entt/entt.hpp>


namespace Enik {

static const double physics_update_rate = (1.0/288.0);
static const float  physics_update_rate_float = (float)physics_update_rate;

class PhysicsWorld {
public:
	static void InitPhysicsWorld(entt::registry* registry);

	static void Step();
	static void ResolveCollisions();

	static const float GetFixedUpdateRate() { return physics_update_rate_float; }

	struct Data {
		entt::registry* Registry;
		glm::vec3 Gravity = glm::vec3(0.0f, -9.81f, 0.0f);
	};
};

}
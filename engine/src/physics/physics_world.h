#pragma once
#include <vector>
#include "core/timestep.h"
#include "glm/glm.hpp"
#include <entt/entt.hpp>


namespace Enik {
class Entity;

class PhysicsWorld {
public:
	void Step(Timestep ts, entt::registry& reg);
	void ResolveCollisions(Timestep ts, entt::registry& reg);

private:
	glm::vec3 m_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

};

}
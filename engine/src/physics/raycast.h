#pragma once

#include "scene/entity.h"
#include <glm/glm.hpp>

namespace Enik {

struct Raycast {
	glm::vec3 origin;
	glm::vec3 dir;
	uint16_t layer;
};
struct RaycastResult {
	Entity entity;
	glm::vec3 point;
};

}

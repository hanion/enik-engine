#include <pch.h>
#include "entity.h"

namespace Enik {

Entity::Entity(entt::entity handle, Scene* scene)
	: m_Handle(handle), m_Scene(scene) {
}

}
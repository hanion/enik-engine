#include "scriptable_entity.h"

namespace Enik {


RaycastResult ScriptableEntity::CastRay(Raycast ray) {
	return m_Entity.m_Scene->m_Physics.CastRay(ray);
}
}

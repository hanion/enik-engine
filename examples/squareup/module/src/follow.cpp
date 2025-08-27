#include "follow.h"
#include "player.h"

namespace Enik {

void Follow::OnCreate() {
	Entity re = FindEntityByUUID(m_Rect);
	if (re) {
		auto& trans = re.Get<Component::Transform>();
		m_base_pos = trans.GlobalPosition;
		m_dead_zone = glm::vec2(trans.GlobalScale.x * 0.5f, trans.GlobalScale.y * 0.5f);
	}
}

float F_lerp(float p0, float p1, float t) {
	return p0 + t * (p1 - p0);
}

glm::vec3 lerp_vec3(const glm::vec3& vec, const glm::vec3& target, float step) {
	return glm::vec3(F_lerp(vec.x, target.x, step), F_lerp(vec.y, target.y, step), F_lerp(vec.z, target.z, step));
}

void Follow::OnFixedUpdate() {
	Entity target = FindEntityByUUID(m_TargetID);
	if (!target) {
		target = FindEntityByName("player");
		if (target) {
			m_TargetID = target.GetID();
		}
		return;
	}

	auto& transform = Get<Component::Transform>();
	auto& target_transform = target.Get<Component::Transform>();

	glm::vec3 cam_pos = transform.LocalPosition;

	glm::vec2 diff = glm::vec2(
		target_transform.GlobalPosition.x - m_base_pos.x,
		target_transform.GlobalPosition.y - m_base_pos.y
	);

	glm::vec2 nudge = glm::vec2(0.0f);

	if (diff.x > m_dead_zone.x)        nudge.x = diff.x - m_dead_zone.x;
	else if (diff.x < -m_dead_zone.x)  nudge.x = diff.x + m_dead_zone.x;

	if (diff.y > m_dead_zone.y)        nudge.y = diff.y - m_dead_zone.y;
	else if (diff.y < -m_dead_zone.y)  nudge.y = diff.y + m_dead_zone.y;

	glm::vec3 target_cam_pos = m_base_pos + glm::vec3(nudge, 0.0f);

	float step = glm::clamp((float)(m_Speed * PHYSICS_UPDATE_RATE), 0.0f, 1.0f);
	transform.LocalPosition = lerp_vec3(cam_pos, target_cam_pos, step);

	transform.LocalPosition.z = 0.0f;

	if (m_CamSize != m_OldCamSize) {
		Get<Component::Camera>().Cam.SetSize(m_CamSize);
		m_OldCamSize = m_CamSize;
	}
}

}
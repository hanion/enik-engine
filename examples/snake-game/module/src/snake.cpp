#include "snake.h"


namespace Enik {


void Snake::ResetState() {
	length = 4;

	head_x = MAP_WIDTH  / 2;
	head_y = MAP_HEIGHT / 2;

	m_direction = DIRECTION::NONE;
	m_color_index = 0;

	for (auto& parte : m_free_parts) {
		if (Entity e = FindEntityByUUID(parte.val)) {
			DestroyEntity(e);
		}
	}
	m_free_parts.clear();

	for (auto& body_part : m_body) {
		if (Entity e = FindEntityByUUID(body_part.part.val)) {
			DestroyEntity(e);
		}
	}
	m_body.clear();

	if (Entity head_sprite = FindEntityByUUID(m_head_sprite)) {
		auto& transform = head_sprite.Get<Component::Transform>();
		transform.LocalRotation.z = 0.0f;
	}

	CreateStartingBody();
}


PartEntity Snake::CreatePart() {
	Entity prefab = InstantiatePrefab(m_part_prefab_path);
	prefab.Get<Component::Transform>().LocalPosition = { 100, 100, 2 };

	prefab.GetOrAdd<Component::SpriteRenderer>().Color = m_colors[m_color_index];
	if (++m_color_index == COLORS_COUNT){
		m_color_index = 0;
	}

	prefab.GetOrAdd<Component::Family>().Reparent(prefab, m_Entity);
	return { prefab.GetID() };
}

void Snake::CreateStartingBody() {
	m_body.push_back({ 10, 10, CreatePart() });
	for (int i = 0; i < length-1; i++) {
		m_body.push_back({ 10, 10, CreatePart() });
	}
}

// call before updating head_x head_y
void Snake::MoveBody(int x, int y, DIRECTION direction, float animation_duration) {
	m_anim_duration = animation_duration / 1000.0f;

	PartEntity part;
	if (m_free_parts.size() > 0) {
		part = m_free_parts.back();
		// body.push_front({x, y, free_parts.back()});
		m_free_parts.pop_back();
	} else {
		part = CreatePart();
		// body.push_front({x, y, CreatePart()});
	}
	if (Entity ent = FindEntityByUUID(part.val)) {
		ent.Get<Component::Transform>().LocalPosition = {
			head_x - MAP_WIDTH /2.0f,
			head_y - MAP_HEIGHT/2.0f,
			0.1f
		};
	}
	m_body.push_front({x, y, {part.val}});

	// free part
	if (m_body.size() > 1) {
		BodyPart p = m_body.back();
		m_free_parts.push_back(p.part);
		m_body.pop_back();
	}


	m_direction = direction;

	if (Entity head_sprite = FindEntityByUUID(m_head_sprite)) {
		auto& transform = head_sprite.Get<Component::Transform>();

		switch (m_direction) {
			case DIRECTION::DOWN:
				transform.LocalRotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
				break;
			case DIRECTION::UP:
				transform.LocalRotation = glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(180.0f)));
				break;
			case DIRECTION::LEFT:
				transform.LocalRotation = glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(-90.0f)));
				break;
			case DIRECTION::RIGHT:
				transform.LocalRotation = glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(90.0f)));
				break;
			default:
				break;
		}
	}
}

void Snake::GrowBody(size_t amount) {
	length += amount;
	for (size_t i = 0; i < amount; i++) {
		auto back = m_body.back();
		if (m_free_parts.size() > 0) {
			m_body.push_back({back.x, back.y, m_free_parts.back()});
			m_free_parts.pop_back();
		} else {
			m_body.push_back({back.x, back.y, CreatePart()});
		}
		// CONSOLE_DEBUG("yum");
		// Get<Component::AudioSource>().Play();

		auto& sources = Get<Component::AudioSources>();

		std::srand(std::time(nullptr));
		int random_number = std::rand() % 2;
		if (random_number == 0) {
			sources.Play("positive_0");
		} else if (random_number == 1) {
			sources.Play("positive_1");
		} else {
			sources.Play("positive_2");
		}
	}
}

void Snake::TeleportHead(int x, int y) {
	if (Entity head = FindEntityByUUID(m_head_sprite)) {
		Component::Transform& tr = head.Get<Component::Transform>();
		tr.LocalPosition = {
			x - MAP_WIDTH /2.0f,
			y - MAP_HEIGHT/2.0f,
			0.5f
		};
	}

}

void Snake::Die() {
	Get<Component::AudioSources>().Play("hit");
}




float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

float EaseInOutCubic(float t) {
	if (t < 0.5f)
		return 4 * t * t * t;
	else {
		float f = 2 * t - 2;
		return 0.5f * f * f * f + 1;
	}
}
float EaseOutQuad(float t) {
	return -t * (t - 2);
}



// void Snake::move_body_lerp(Timestep ts) {
// 	static float elapsed_time = 0.0f;
// 	elapsed_time += ts;

// 	float t = std::min(elapsed_time / m_anim_duration, 1.0f);

// 	for (auto it = m_body.begin(); it != m_body.end(); ++it) {
// 		BodyPart body_part = *it;

// 		if (Entity ent = FindEntityByUUID(body_part.part.val)) {
// 			Component::Transform& tr = ent.Get<Component::Transform>();
// 			float new_x = lerp(tr.Position.x, body_part.x - MAP_WIDTH /2.0f, (t));
// 			float new_y = lerp(tr.Position.y, body_part.y - MAP_HEIGHT/2.0f, (t));
// 			tr.Position = { new_x, new_y, 0.1f };
// 		}
// 	}

// 	// Reset elapsed time if the lerp is finished
// 	if (t >= 1.0f) {
// 		elapsed_time = 0.0f;
// 	}
// }

void Snake::move_head_lerp(Timestep ts) {
	static float elapsed_time2 = 0.0f;
	elapsed_time2 += ts;

	float t = std::min(elapsed_time2 / m_anim_duration, 1.0f);

	if (m_body.size() > 0) {
		BodyPart body_part = m_body.front();

		if (Entity head = FindEntityByUUID(m_head_sprite)) {
			Component::Transform& tr = head.Get<Component::Transform>();
			float new_x = lerp(tr.LocalPosition.x, body_part.x - MAP_WIDTH /2.0f, EaseInOutCubic(t));
			float new_y = lerp(tr.LocalPosition.y, body_part.y - MAP_HEIGHT/2.0f, EaseInOutCubic(t));
			tr.LocalPosition = { new_x, new_y, 0.5f };
		}
	}

	// reset elapsed time if the lerp is finished
	if (t >= 1.0f) {
		elapsed_time2 = 0.0f;

		// move body
		for (auto it = m_body.begin(); it != m_body.end(); ++it) {
			BodyPart body_part = *it;

			if (Entity ent = FindEntityByUUID(body_part.part.val)) {
				Component::Transform& tr = ent.Get<Component::Transform>();
				tr.LocalPosition.x = body_part.x - MAP_WIDTH/2.0f;
				tr.LocalPosition.y = body_part.y - MAP_HEIGHT/2.0f;
				tr.LocalPosition.z = -0.1f;
			}
		}
	}
}


void Snake::OnUpdate(Timestep ts) {
	// move_body_lerp(ts);
	move_head_lerp(ts);
}



}
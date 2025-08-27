#include "game_manager.h"

namespace Enik {


void GameManager::ResetState() {
	while (not m_direction_queue.empty()) {
		m_direction_queue.pop();
	}

	m_is_playing = false;
	m_changed_direction = false;
	m_snake_direction = DIRECTION::NONE;

	m_time_accumulated = 0.0f;
	m_update_wait_time = 100.0f;

	std::fill(&map[0][0], &map[0][0] + MAP_WIDTH * MAP_HEIGHT, 0);

	if (Entity se = FindEntityByUUID(m_snake_entity)) {
		snake = (Snake*)se.GetScriptInstance();
		if (snake) {
			snake->ResetState();
		}
	}

	SetScore();

	GenerateFood();

	GetOrAdd<Component::Prefab>();
	GetOrAdd<Component::AudioSources>();
	GetOrAdd<Component::AnimationPlayer>();
}


void GameManager::OnUpdate(Timestep ts) {
	if (not m_is_playing) {
		return;
	}

	m_time_accumulated += ts.GetMilliseconds();
	if (m_time_accumulated >= m_update_wait_time) {
		m_time_accumulated = 0;

		if (not m_direction_queue.empty()) {
			if (m_direction_queue.size() > 1) {
				m_direction_queue.pop();
			}
			Move(m_direction_queue.front());
		}
	}
	m_queue_count_debug = m_direction_queue.size();
}

void GameManager::add_move(DIRECTION direction) {
	if (not m_direction_queue.empty() and m_direction_queue.back() == direction) {
		return;
	}
	m_direction_queue.push(direction);
	m_snake_direction = direction;
}


void GameManager::SetScore() {
	Entity se = FindEntityByUUID(m_snake_entity);
	if (not se) {
		CONSOLE_DEBUG_ERROR("GameManager::SetScore can't find snake!");
		return;
	}

	snake = (Snake*)se.GetScriptInstance();
	if (not snake) {
		CONSOLE_DEBUG_ERROR("GameManager::SetScore can't find snake script!");
		return;
	}

	if (Entity nd = FindEntityByUUID(m_number_display)) {
		NumberDisplay* script = (NumberDisplay*)nd.GetScriptInstance();
		if (script) {
			script->SetNumber(snake->length);
		}
	}
}



void GameManager::OnKeyPressed(const KeyPressedEvent& event) {
	if (not m_is_playing) {
		return;
	}

	switch (event.GetKeyCode()) {
		case Key::Up:
		case Key::W:
			if (m_snake_direction != DIRECTION::DOWN) {
				add_move(DIRECTION::UP);
			}
			break;
		case Key::Down:
		case Key::S:
			if (m_snake_direction != DIRECTION::UP) {
				add_move(DIRECTION::DOWN);
			}
			break;
		case Key::Right:
		case Key::D:
			if (m_snake_direction != DIRECTION::LEFT) {
				add_move(DIRECTION::RIGHT);
			}
			break;
		case Key::Left:
		case Key::A:
			if (m_snake_direction != DIRECTION::RIGHT) {
				add_move(DIRECTION::LEFT);
			}
			break;
#if 0
		case Key::E:
			snake->GrowBody(1);
			break;
		case Key::P:
			snake->GrowBody(10);
			break;
		case Key::Y: {
			static int display_debug = 0;
			if (Entity nd = FindEntityByUUID(m_number_display)) {
				NumberDisplay* script = (NumberDisplay*)nd.GetScriptInstance();
				if (script) {
					script->SetNumber(display_debug);
					display_debug++;
				}
			}
		}
			break;
#endif
        default:
            break;
    }
}




void GameManager::GenerateFood() {
	int x = 0;
	int y = 0;
	do {
		x = rand() % (MAP_WIDTH  - 2) + 1;
		y = rand() % (MAP_HEIGHT - 2) + 1;
	} while (map[x][y] != MAP::EMPTY);

	map[x][y] = MAP::FOOD;

	if (Entity food = FindEntityByUUID(m_food_entity)) {
		food.Get<Component::Transform>().LocalPosition = { x - MAP_WIDTH / 2.0f, y - MAP_HEIGHT / 2.0f, 0.1f };
	}
}

void GameManager::Move(DIRECTION direction) {
	if (not m_is_playing) {
		return;
	}

	// ! MUST
	// it will fail on sm reload, if pointer not set properly
	if (Entity se = FindEntityByUUID(m_snake_entity)) {
		snake = (Snake*)se.GetScriptInstance();
	}
	else {
		CONSOLE_DEBUG_ERROR("cant find snake entity");
	}

	int new_x = snake->head_x;
	int new_y = snake->head_y;

	switch (direction) {
		case DIRECTION::UP:
			new_y += 1;
			break;
		case DIRECTION::DOWN:
			new_y -= 1;
			break;
		case DIRECTION::LEFT:
			new_x -= 1;
			break;
		case DIRECTION::RIGHT:
			new_x += 1;
			break;
		case DIRECTION::NONE:
			return;
	}

	if (new_x < 0 or new_x >= MAP_WIDTH or
		new_y < 0 or new_y >= MAP_HEIGHT) {


		// if walls kill
		if (m_walls_kill) {
			m_snake_direction = DIRECTION::NONE;
			while (m_direction_queue.size() > 0) {
				m_direction_queue.pop();
			}
			add_move(DIRECTION::NONE);
			snake->Die();
			m_is_playing = false;

			return;
		}
		// if walls teleport
		else {
			if (new_x < 0) {
				new_x = MAP_WIDTH - 1;
				snake->TeleportHead(new_x + 1, new_y);
			}
			else if (new_x >= MAP_WIDTH) {
				new_x = 0;
				snake->TeleportHead(new_x - 1, new_y);
			}

			if (new_y < 0) {
				new_y = MAP_HEIGHT - 1;
				snake->TeleportHead(new_x, new_y + 1);
			}
			else if (new_y >= MAP_HEIGHT) {
				new_y = 0;
				snake->TeleportHead(new_x, new_y - 1);
			}
		}


	}


	int new_place_value = map[new_x][new_y];

	if (new_place_value == MAP::FOOD) {
		snake->GrowBody(1);
		SetScore();
		GenerateFood();
		if (m_update_wait_time > 30.0f) {
			m_update_wait_time -= 0.5f;
		}
	}
	else if (new_place_value != MAP::EMPTY) {
		// CONSOLE_DEBUG("DEAD! by snake");
		// todo should return, do something
		m_snake_direction = DIRECTION::NONE;
		while (m_direction_queue.size() > 0) {
			m_direction_queue.pop();
		}
		add_move(DIRECTION::NONE);
		snake->Die();
		m_is_playing = false;
		return;
	}

	snake->MoveBody(new_x, new_y, direction, m_update_wait_time);
	snake->head_x = new_x;
	snake->head_y = new_y;
	map[new_x][new_y] = snake->length + 1;

	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			if (map[x][y] > 0) {
				map[x][y]--;
			}
		}
	}
}


}
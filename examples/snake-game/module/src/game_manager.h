#pragma once
#include <queue>

#include "../api/enik.h"
#include "snake.h"
#include "number_display.h"


namespace Enik {

class GameManager : public ScriptableEntity {
public:
	void ResetState();

	void SetPlaying(bool playing) {
		m_is_playing = playing;
	}

private:
	virtual void OnUpdate(Timestep ts) override final;

	virtual void OnKeyPressed (const KeyPressedEvent&  event) override final;


	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "wait time",  FieldType::FLOAT,  &m_update_wait_time },
			{ "snake",      FieldType::ENTITY, &m_snake_entity },
			{ "walls kill", FieldType::BOOL,   &m_walls_kill },
			{ "food",       FieldType::ENTITY, &m_food_entity },
			{ "score",      FieldType::ENTITY, &m_number_display },
			{ "queue",      FieldType::INT,    &m_queue_count_debug }
		};
	}


	void add_move(DIRECTION direction);


	void SetScore();


public:
	UUID m_snake_entity;
	UUID m_food_entity;
	UUID m_number_display;

	bool m_is_playing = false;

	bool m_changed_direction = false;
	DIRECTION m_snake_direction = DIRECTION::NONE;

	std::queue<DIRECTION> m_direction_queue;

	float m_time_accumulated = 0.0f;
	float m_update_wait_time = 100.0f;

	bool m_walls_kill = false;


	int map[MAP_WIDTH][MAP_HEIGHT];
	Snake* snake;

	void GenerateFood();
	void Move(DIRECTION direction);

	int m_queue_count_debug = 0;
};

}
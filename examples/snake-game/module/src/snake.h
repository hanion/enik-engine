#pragma once
#include "../api/enik.h"
#include <list>

#define MAP_WIDTH  20
#define MAP_HEIGHT 20


namespace Enik {


enum MAP {
	EMPTY = 0,
	// WALL = -1,
	FOOD = -2
};

enum class DIRECTION {
	NONE = 0,
	RIGHT, LEFT, UP, DOWN
};


struct PartEntity {
	Enik::UUID val;
};

struct BodyPart {
	int x = -1;
	int y = -1;
	PartEntity part;
};

class Snake : public ScriptableEntity {
public:
	void ResetState();

	void MoveBody(int x, int y, DIRECTION direction, float animation_duration);

	void GrowBody(size_t amount = 1);

	void TeleportHead(int x, int y);

	void Die();

private:
	virtual void OnUpdate(Timestep ts) override final;

	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "part",   FieldType::PREFAB,  &m_part_prefab_path },
			{ "length", FieldType::INT,     &length },
			{ "color",  FieldType::INT,     &m_color_index },
			{ "head",   FieldType::ENTITY,  &m_head_sprite },
			{ "direction",FieldType::INT,  &m_direction }
		};
	}

	PartEntity CreatePart();

	void CreateStartingBody();

	void move_body_lerp(Timestep ts);
	void move_head_lerp(Timestep ts);

public:
	int length = 4;

	int head_x = MAP_WIDTH  / 2;
	int head_y = MAP_HEIGHT / 2;

	float m_anim_duration = 0.1f;

private:
	std::vector<PartEntity> m_free_parts;
	std::list<BodyPart> m_body;

	DIRECTION m_direction = DIRECTION::NONE;

	UUID m_head_sprite;
	// UUID m_head_sprite2;

	std::string m_part_prefab_path = "";

	#define COLORS_COUNT 21
	int m_color_index = 0;
	glm::vec4 m_colors[COLORS_COUNT] = {
		{ 0.1f, 0.550f, 0.1f, 1.0f },
		{ 0.1f, 0.545f, 0.1f, 1.0f },
		{ 0.1f, 0.540f, 0.1f, 1.0f },
		{ 0.1f, 0.535f, 0.1f, 1.0f },
		{ 0.1f, 0.530f, 0.1f, 1.0f },
		{ 0.1f, 0.525f, 0.1f, 1.0f },
		{ 0.1f, 0.520f, 0.1f, 1.0f },
		{ 0.1f, 0.515f, 0.1f, 1.0f },
		{ 0.1f, 0.510f, 0.1f, 1.0f },
		{ 0.1f, 0.505f, 0.1f, 1.0f },
		{ 0.1f, 0.500f, 0.1f, 1.0f },
		{ 0.1f, 0.495f, 0.1f, 1.0f },
		{ 0.1f, 0.490f, 0.1f, 1.0f },
		{ 0.1f, 0.485f, 0.1f, 1.0f },
		{ 0.1f, 0.480f, 0.1f, 1.0f },
		{ 0.1f, 0.475f, 0.1f, 1.0f },
		{ 0.1f, 0.470f, 0.1f, 1.0f },
		{ 0.1f, 0.465f, 0.1f, 1.0f },
		{ 0.1f, 0.460f, 0.1f, 1.0f },
		{ 0.1f, 0.455f, 0.1f, 1.0f },
		{ 0.1f, 0.450f, 0.1f, 1.0f }
    };

};

}
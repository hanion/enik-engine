#pragma once
#include "../api/enik.h"



namespace Enik {


class Follow : public ScriptableEntity {
private:
	virtual void OnCreate() override final;

	virtual void OnFixedUpdate() override final;

	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "Target",   FieldType::ENTITY, &m_TargetID },
			{ "Speed",    FieldType::FLOAT,  &m_Speed },
			{ "Rect",     FieldType::ENTITY,  &m_Rect },
		};
	}

public:
	float m_CamSize = 10.0f;
	float m_OldCamSize = m_CamSize;

	float m_Speed = 1.8f;

	UUID m_TargetID;
	UUID m_Rect;

	glm::vec3 m_base_pos = glm::vec3(0,3,0);
	glm::vec2 m_dead_zone = glm::vec2(5.5f, 3.0f);
};

}
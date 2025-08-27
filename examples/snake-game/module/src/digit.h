#pragma once
#include "../api/enik.h"

namespace Enik {

class Digit : public ScriptableEntity {
public:
	void SetDigit(int value);

private:
	void UpdateDigit();


	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "value", FieldType::INT,    &m_value },
			{ "0",     FieldType::ENTITY, &m_entities[0] },
			{ "1",     FieldType::ENTITY, &m_entities[1] },
			{ "2",     FieldType::ENTITY, &m_entities[2] },
			{ "3",     FieldType::ENTITY, &m_entities[3] },
			{ "4",     FieldType::ENTITY, &m_entities[4] },
			{ "5",     FieldType::ENTITY, &m_entities[5] },
			{ "6",     FieldType::ENTITY, &m_entities[6] },
			{ "7",     FieldType::ENTITY, &m_entities[7] },
			{ "8",     FieldType::ENTITY, &m_entities[8] },
			{ "9",     FieldType::ENTITY, &m_entities[9] }
		};
	}

private:
	int m_value = 10;

	UUID m_entities[10];


};

}
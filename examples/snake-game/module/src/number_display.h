#pragma once
#include "../api/enik.h"
#include "digit.h"

namespace Enik {

class NumberDisplay : public ScriptableEntity {
public:
	void SetNumber(int value);

	virtual void OnCreate() override final;

private:
	void UpdateNumber();

	void CreateNewDigit();

	virtual std::vector<NativeScriptField> OnEditorGetFields() override final {
		return std::vector<NativeScriptField>{
			{ "value",  FieldType::INT,    &m_value },
			{ "offset", FieldType::FLOAT,  &m_Offset },
			{ "digit",  FieldType::PREFAB, &m_DigitPrefab }
		};
	}

private:
	int m_value = -1;

	float m_Offset = 1.0f;

	std::string m_DigitPrefab;

	std::vector<UUID> m_Digits;

};

}
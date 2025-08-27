#pragma once
#include "../api/enik.h"
#include "common.h"

namespace Enik {

class GUI : public ScriptableEntity {
public:
	virtual void OnCreate() override;
	virtual void OnUpdate(Timestep t) override;


public:
	void SetHealth(float hp);
	void SetScore(int score);
	void SetPrimary(Primary primary);
	void SetSecondary(Secondary secondary);




	virtual std::vector<NativeScriptField> OnEditorGetFields() override {
		return {
			{ "Heart 0",        FieldType::ENTITY,  &m_Hearts[0] },
			{ "Heart 1",        FieldType::ENTITY,  &m_Hearts[1] },
			{ "Heart 2",        FieldType::ENTITY,  &m_Hearts[2] },
			{ "Primary",        FieldType::ENTITY,  &m_Primary },
			{ "Secondary",      FieldType::ENTITY,  &m_Secondary },
			{ "Score",          FieldType::ENTITY,  &m_Score },
		};
	}

private:
	UUID m_Hearts[3] = {0,0,0};
	UUID m_Primary = 0;
	UUID m_Secondary = 0;
	UUID m_Score = 0;
};


inline GUI* GetGUI() {
	Entity g = Enik::FindEntityByName("GUI");
	if (g) { return (GUI*)(g.GetScriptInstance()); }
	return nullptr;
}

}


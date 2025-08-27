#include "../api/enik.h"

namespace Enik {

class EnemySpawner : public ScriptableEntity {
public:
	virtual void OnCreate() override {
		srand((unsigned int)time(nullptr));
		m_SpawnTimer = 1.0f;
		Entity sa = FindEntityByUUID(m_SpawnArea);
		if (sa) {
			auto& trans = sa.Get<Component::Transform>();
			m_SpawnRect.x = trans.GlobalPosition.x - trans.GlobalScale.x * 0.5f;
			m_SpawnRect.y = trans.GlobalPosition.y - trans.GlobalScale.y * 0.5f;
			m_SpawnRect.z = trans.GlobalPosition.x + trans.GlobalScale.x * 0.5f;
			m_SpawnRect.w = trans.GlobalPosition.y + trans.GlobalScale.y * 0.5f;
		}
	}

	virtual void OnUpdate(Timestep t) override {
		if (!m_SpawnEnabled || m_EnemyPrefabPath.empty()) {
			return;
		}

		m_SpawnTimer -= t;
		if (m_SpawnTimer <= 0.0f) {
			SpawnEnemy();
			m_SpawnInterval = std::max(m_SpawnInterval * 0.95f, 0.5f);
			m_SpawnTimer = m_SpawnInterval;
		}
	}

	void SpawnEnemy() {
		Entity enemy = InstantiatePrefab(m_EnemyPrefabPath);
		if (!enemy) {
			return;
		}
	
		enemy.Reparent(Enik::FindEntityByName("ENTITIES"));

		glm::vec3 spawn_pos = glm::vec3(
			RandomFloat(m_SpawnRect.x, m_SpawnRect.z),
			RandomFloat(m_SpawnRect.y, m_SpawnRect.w),
			0.0f
		);
		enemy.Get<Component::Transform>().LocalPosition = spawn_pos;
		Component::RigidBody& rb = enemy.GetOrAdd<Component::RigidBody>();
		rb.SetPosition(spawn_pos);
	}

	float RandomFloat(float min, float max) {
		return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
	}

	virtual std::vector<NativeScriptField> OnEditorGetFields() override {
		return {
			{ "Enabled",        FieldType::BOOL,  &m_SpawnEnabled },
			{ "Prefab",         FieldType::PREFAB,&m_EnemyPrefabPath },
			{ "Spawn Interval", FieldType::FLOAT, &m_SpawnInterval },
			{ "Spawn Area",     FieldType::ENTITY,&m_SpawnArea }
		};
	}

private:
	std::string m_EnemyPrefabPath = "";
	UUID m_SpawnArea = 0;
	glm::vec4 m_SpawnRect = {0,0,0,0};

	bool m_SpawnEnabled = true;

	float m_SpawnInterval = 3.0f;
	float m_SpawnTimer = 3.0f;
};

}

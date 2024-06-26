#pragma once
#include <base.h>
#include <yaml-cpp/yaml.h>

#include "scene/scene.h"
#include "renderer/texture.h"
#include "scene/components.h"

namespace Enik {

class SceneSerializer {
public:
	SceneSerializer(Scene* scene);
	SceneSerializer(const Ref<Scene>& scene);

	void Serialize(const std::string& filepath);
	void SerializeRuntime(const std::string& filepath);

	bool Deserialize(const std::string& filepath);
	bool DeserializeRuntime(const std::string& filepath);

	const UUID DuplicateEntity(const std::string& filepath, UUID uuid);

	void ReloadNativeScriptFields(const std::string& filepath);

	void CreatePrefab(const std::string& filepath, Entity entity_to_prefab);
	Entity InstantiatePrefab(const std::string& filepath, UUID instance_uuid = UUID());

private:
	void SerializeEntity(YAML::Emitter& out, Entity& entity);
	void DeserializeEntity(YAML::Node& data, uint64_t uuid, std::string& name);

	void DeserializeNativeScript(YAML::Node& node, Entity& entity);
	void SerializeNativeScriptFields(YAML::Emitter& out, Component::NativeScript& script);
	void DeserializeNativeScriptFields(YAML::Node& node, Entity& entity);

	void UpdateUUIDs(YAML::Node& entities, std::unordered_map<uint64_t, uint64_t>& uuid_map, UUID new_root_uuid);

private:
	Scene* m_Scene;

	Ref<Texture2D> m_ErrorTexture;
};

}
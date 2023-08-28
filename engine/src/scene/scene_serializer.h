#pragma once
#include <base.h>
#include <yaml-cpp/yaml.h>

#include "scene/scene.h"
#include "renderer/texture.h"

namespace Enik {

class SceneSerializer {
public:
	SceneSerializer(const Ref<Scene>& scene);

	void Serialize(const std::string& filepath);
	void SerializeRuntime(const std::string& filepath);

	bool Deserialize(const std::string& filepath);
	bool DeserializeRuntime(const std::string& filepath);

	void DuplicateEntity(const std::string& filepath, UUID uuid);

private:
	void SerializeEntity(YAML::Emitter& out, Entity& entity);
	void DeserializeEntity(YAML::Node& data, uint64_t uuid, std::string& name);

private:
	Ref<Scene> m_Scene;

	Ref<Texture2D> m_ErrorTexture;
};

}
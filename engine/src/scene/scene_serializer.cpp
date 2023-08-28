#include "scene_serializer.h"

#include <pch.h>

#include "scene/components.h"

namespace YAML {
template <>
struct convert<glm::vec2> {
	static Node encode(const glm::vec2& vec) {
		Node node;
		node.push_back(vec.x);
		node.push_back(vec.y);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	};

	static bool decode(const Node& node, glm::vec2& vec) {
		if (not node.IsSequence() or node.size() != 2) {
			return false;
		}
		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		return true;
	}
};

template <>
struct convert<glm::vec3> {
	static Node encode(const glm::vec3& vec) {
		Node node;
		node.push_back(vec.x);
		node.push_back(vec.y);
		node.push_back(vec.z);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	};

	static bool decode(const Node& node, glm::vec3& vec) {
		if (not node.IsSequence() or node.size() != 3) {
			return false;
		}
		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		vec.z = node[2].as<float>();
		return true;
	}
};

template <>
struct convert<glm::vec4> {
	static Node encode(const glm::vec4& vec) {
		Node node;
		node.push_back(vec.x);
		node.push_back(vec.y);
		node.push_back(vec.z);
		node.push_back(vec.w);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	};

	static bool decode(const Node& node, glm::vec4& vec) {
		if (not node.IsSequence() or node.size() != 4) {
			return false;
		}
		vec.x = node[0].as<float>();
		vec.y = node[1].as<float>();
		vec.z = node[2].as<float>();
		vec.w = node[3].as<float>();
		return true;
	}
};

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec) {
	out << YAML::Flow;
	out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
	return out;
}
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec) {
	out << YAML::Flow;
	out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
	return out;
}
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec) {
	out << YAML::Flow;
	out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
	return out;
}
}





namespace Enik {

SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	: m_Scene(scene) {
	m_ErrorTexture = Texture2D::Create(FULL_PATH_EDITOR("assets/textures/error.png"), false);
}

void SceneSerializer::Serialize(const std::string& filepath) {
	EN_CORE_INFO("Serializing scene   '{0}', in '{1}'", m_Scene->GetName(), filepath);
	YAML::Emitter out;
	out << YAML::BeginMap;

	out << YAML::Key << "Scene" << YAML::Value << m_Scene->GetName();

	out << YAML::Key << "Entities";
	out << YAML::Value << YAML::BeginSeq;

	m_Scene->m_Registry.each([&](auto entityID) {
		Entity entity = Entity(entityID, m_Scene.get());
		if (not entity) {
			return;
		}
		SerializeEntity(out, entity);
	});

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

void SceneSerializer::SerializeRuntime(const std::string& filepath) {
	EN_CORE_ASSERT(false);
}

bool SceneSerializer::Deserialize(const std::string& filepath) {
	YAML::Node data = YAML::LoadFile(filepath);
	if (not data["Scene"]) {
		return false;
	}

	std::string scene_name = data["Scene"].as<std::string>();
	m_Scene->SetName(scene_name);
	EN_CORE_INFO("Deserializing scene '{0}', in '{1}'", scene_name, filepath);


	auto entities = data["Entities"];
	if (entities) {
		// iterate reversed to not change entity indexes
		for (std::size_t i = entities.size(); i > 0; --i) {
			auto entity = entities[i - 1];

			uint64_t uuid = entity["Entity"].as<uint64_t>();
			std::string name;
			auto tag = entity["Component::Tag"];
			if (tag) {
				name = tag["Text"].as<std::string>();
			}

			DeserializeEntity(entity, uuid, name);
			// EN_CORE_TRACE("Deserialized entity {0}, with name '{1}'", uuid, name);
		}
	}

	return true;
}

bool SceneSerializer::DeserializeRuntime(const std::string& filepath) {
	EN_CORE_ASSERT(false);
	return false;
}

void SceneSerializer::DuplicateEntity(const std::string& filepath, UUID uuid) {
	YAML::Node data = YAML::LoadFile(filepath);
	if (not data["Scene"]) {
		return;
	}

	auto entities = data["Entities"];
	if (entities) {
		for (auto entity : entities) {
			uint64_t data_uuid = entity["Entity"].as<uint64_t>();
			if (data_uuid == uuid) {

				std::string name;
				auto tag = entity["Component::Tag"];
				if (tag) {
					name = tag["Text"].as<std::string>();
				}
				name += " duplicate";

				DeserializeEntity(entity, UUID(), name);
				return;
			}

		}
	}
}

void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity& entity) {
	out << YAML::BeginMap; // Entity

	out << YAML::Key << "Entity" << YAML::Value << (uint64_t)entity.Get<Component::ID>();

	if (entity.Has<Component::Tag>()) {
		out << YAML::Key << "Component::Tag";
		out << YAML::BeginMap;

		auto& text = entity.Get<Component::Tag>().Text;
		out << YAML::Key << "Text" << YAML::Value << text;

		out << YAML::EndMap;
	}

	if (entity.Has<Component::Transform>()) {
		out << YAML::Key << "Component::Transform";
		out << YAML::BeginMap;

		auto& transform = entity.Get<Component::Transform>();
		out << YAML::Key << "Position" << YAML::Value << transform.Position;
		out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << transform.Scale;

		out << YAML::EndMap;
	}

	if (entity.Has<Component::Camera>()) {
		out << YAML::Key << "Component::Camera";
		out << YAML::BeginMap;

		auto& cam = entity.Get<Component::Camera>();

		out << YAML::Key << "Camera" << YAML::Value;
		out << YAML::BeginMap; // Camera
		out << YAML::Key << "Size" << YAML::Value << cam.Cam.GetSize();
		out << YAML::Key << "Near" << YAML::Value << cam.Cam.GetNear();
		out << YAML::Key << "Far"  << YAML::Value << cam.Cam.GetFar();
		out << YAML::EndMap; // Camera

		out << YAML::Key << "Primary" << YAML::Value << cam.Primary;
		out << YAML::Key << "FixedAspectRatio" << YAML::Value << cam.FixedAspectRatio;

		out << YAML::EndMap;
	}

	if (entity.Has<Component::SpriteRenderer>()) {
		out << YAML::Key << "Component::SpriteRenderer";
		out << YAML::BeginMap;

		auto& sprite = entity.Get<Component::SpriteRenderer>();
		out << YAML::Key << "Color" << YAML::Value << sprite.Color;
		out << YAML::Key << "TileScale" << YAML::Value << sprite.TileScale;
		out << YAML::Key << "TexturePath" << YAML::Value << sprite.TexturePath.string(); // TODO asset manager

		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

void SceneSerializer::DeserializeEntity(YAML::Node& entity, uint64_t uuid, std::string& name) {
	Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

	auto transform = entity["Component::Transform"];
	if (transform) {
		auto& trans = deserializedEntity.Get<Component::Transform>();
		trans.Position = transform["Position"].as<glm::vec3>();
		trans.Rotation = transform["Rotation"].as<float>();
		trans.Scale = transform["Scale"].as<glm::vec2>();
	}

	auto spriteRenderer = entity["Component::SpriteRenderer"];
	if (spriteRenderer) {
		auto& sprite = deserializedEntity.Add<Component::SpriteRenderer>();
		sprite.Color = spriteRenderer["Color"].as<glm::vec4>();
		sprite.TileScale = spriteRenderer["TileScale"].as<float>();

		auto path = std::filesystem::path(spriteRenderer["TexturePath"].as<std::string>());
		if (not path.empty()) {
			sprite.TexturePath = path;
		}

		if (not sprite.TexturePath.empty()) {
			// TODO do not deal with paths like this ...
			if (std::filesystem::exists(FULL_PATH_EDITOR(sprite.TexturePath))) {
				sprite.Texture = Texture2D::Create(FULL_PATH_EDITOR(sprite.TexturePath));
			}
			else {
				sprite.Texture = m_ErrorTexture;
			}
		}

	}

	auto camera = entity["Component::Camera"];
	if (camera) {
		auto& cam = deserializedEntity.Add<Component::Camera>();

		cam.Cam.SetSize(camera["Camera"]["Size"].as<float>());
		cam.Cam.SetNear(camera["Camera"]["Near"].as<float>());
		cam.Cam.SetFar (camera["Camera"]["Far" ].as<float>());

		cam.Primary = camera["Primary"].as<bool>();
		cam.FixedAspectRatio = camera["FixedAspectRatio"].as<bool>();
	}

}

}
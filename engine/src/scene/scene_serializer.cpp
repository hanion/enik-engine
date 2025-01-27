#include "scene_serializer.h"

#include <pch.h>
#include <stack>

#include "core/log.h"
#include "gtc/type_ptr.hpp"
#include "project/project.h"
#include "scene/components.h"
#include "script_system/script_registry.h"


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

template <>
struct convert<glm::quat> {
	static Node encode(const glm::quat& quat) {
		Node node;
		node.push_back(quat.x);
		node.push_back(quat.y);
		node.push_back(quat.z);
		node.push_back(quat.w);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	};

	static bool decode(const Node& node, glm::quat& quat) {
		if (not node.IsSequence() or node.size() != 4) {
			return false;
		}
		quat.x = node[0].as<float>();
		quat.y = node[1].as<float>();
		quat.z = node[2].as<float>();
		quat.w = node[3].as<float>();
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
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& quat) {
	out << YAML::Flow;
	out << YAML::BeginSeq << quat.x << quat.y << quat.z << quat.w << YAML::EndSeq;
	return out;
}
}





namespace Enik {

bool IsChildOfPrefab(Entity entity) {
	if (!entity.HasParent()) {
		return false;
	}

	std::stack<Entity> parents;
	parents.push(entity.GetParent());

	while (!parents.empty()) {
		Entity p = parents.top();
		parents.pop();

		if (p.Has<Component::Prefab>()) {
			return true;
		}

		if (p.HasParent()) {
			parents.push(p.GetParent());
		}
	}
	return false;
}

void SceneSerializer::Serialize(const std::string& filepath) {
	// EN_CORE_INFO("Serializing scene   '{0}', in '{1}'", m_Scene->GetName(), filepath);
	YAML::Emitter out;
	out << YAML::BeginMap;

	out << YAML::Key << "Scene" << YAML::Value << m_Scene->GetName();

	out << YAML::Key << "Entities";
	out << YAML::Value << YAML::BeginSeq;

	m_Scene->m_Registry.each([&](auto entityID) {
		Entity entity = Entity(entityID, m_Scene);
		if (not entity) {
			return;
		}
		if (entity.Has<Component::Prefab>()) {
			if (IsChildOfPrefab(entity)) {
				return;
			}
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
	YAML::Node data;
	try {
		data = YAML::LoadFile(filepath);
	}
	catch (const YAML::ParserException& e) {
		EN_CORE_ERROR("Failed to load .escn file '{0}'\n	{1}", filepath, e.what());
		return false;
	}

	if (not data.IsMap() or not data["Scene"]) {
		return false;
	}

	std::string scene_name = data["Scene"].as<std::string>();
	m_Scene->SetName(scene_name);
	// EN_CORE_INFO("Deserializing scene '{0}', in '{1}'", scene_name, filepath);


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

const UUID SceneSerializer::DuplicateEntity(const std::string& filepath, UUID uuid) {
	YAML::Node data = YAML::LoadFile(filepath);
	if (not data.IsMap() or not (data["Scene"] or data["Prefab"])) {
		return -1;
	}

	auto entities = data["Entities"];
	if (not entities) {
		return -1;
	}

	Entity parent_of_root;
	Entity root_entity = m_Scene->FindEntityByUUID(uuid);
	if (root_entity.HasParent()) {
		parent_of_root = root_entity.GetParent();
	}

	// strip unrelated entities from yaml node
	YAML::Node related_entities;
	Component::Family& root_family = root_entity.GetOrAddFamily();
	for (const auto& entity : entities) {
		uint64_t data_uuid = entity["Entity"].as<uint64_t>();
		Entity test_entity = m_Scene->FindEntityByUUID(data_uuid);
		EN_CORE_ASSERT(test_entity);

		if (data_uuid == uuid) {
			related_entities.push_back(entity);
		}
		else if (root_family.HasEntityAsChild(test_entity)) {
			related_entities.push_back(entity);
		}
	}
	entities = related_entities;

	std::unordered_map<uint64_t, uint64_t> uuid_map;
	UUID new_root_uuid = UUID();
	uuid_map[uuid] = new_root_uuid;
	UpdateUUIDs(entities, uuid_map, new_root_uuid);
	
	// iterate reversed to not change entity indexes
	for (size_t i = entities.size(); i > 0; --i) {
		auto relative_entity = entities[i - 1];
		uint64_t relative_uuid = relative_entity["Entity"].as<uint64_t>();

		std::string name;
		if (auto tag = relative_entity["Component::Tag"]) {
			name = tag["Text"].as<std::string>();
		}

		DeserializeEntity(relative_entity, relative_uuid, name);
	}

	if (Entity root_entity = m_Scene->FindEntityByUUID(new_root_uuid)) {
		root_entity.Reparent(parent_of_root);
	}

	return new_root_uuid;
}


void SceneSerializer::ReloadNativeScriptFields(const std::string& filepath) {
	YAML::Node data = YAML::LoadFile(filepath);
	if (not data.IsMap() or not data["Scene"]) {
		return;
	}

	auto entities = data["Entities"];
	if (not entities) {
		return;
	}

	for (auto entity_node : entities) {
		if (not entity_node["Component::NativeScript"]) {
			continue;
		}

		uint64_t entity_id = entity_node["Entity"].as<uint64_t>();
		Entity entity = m_Scene->FindEntityByUUID(entity_id);
		if (entity and entity.Has<Component::NativeScript>()) {

			// destroy old script
			auto& script = entity.Get<Component::NativeScript>();
			script.DestroyScript(&script);
			entity.Remove<Component::NativeScript>();

			DeserializeNativeScript(entity_node, entity);
		}
	}
}

void SceneSerializer::CreatePrefab(const std::string& filepath, Entity entity_to_prefab) {
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Prefab" << YAML::Value << entity_to_prefab.GetTag();
	out << YAML::Key << "Root"   << YAML::Value << entity_to_prefab.Get<Component::ID>().uuid;

	out << YAML::Key << "Entities";
	out << YAML::Value << YAML::BeginSeq;

	SerializeEntity(out, entity_to_prefab);

	Component::Family entity_family = entity_to_prefab.GetOrAddFamily();
	if (entity_family.Children.size() > 0) {
		m_Scene->m_Registry.each([&](auto entityID) {
			Entity entity = Entity(entityID, m_Scene);
			if (not entity) {
				return;
			}
			if (entity_family.HasEntityAsChild(entity)) {
				SerializeEntity(out, entity);
			}
		});
	}

	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
	fout.close();

	// remove Component::Prefab and roots parent
	YAML::Node data;
	try {
		data = YAML::LoadFile(filepath);
	}
	catch (const YAML::ParserException& e) {
		EN_CORE_ERROR("Failed to load prefab file '{0}'\n	{1}", filepath, e.what());
		return;
	}
	auto entities = data["Entities"];
	if (not entities or not entities.IsSequence()) {
		return;
	}
	for (auto entity : entities) {
		if (entity["Entity"].as<std::uint64_t>() == entity_to_prefab.GetID()) {
			if (entity["Component::Prefab"]) {
				entity.remove("Component::Prefab");
			}
			if (auto fam = entity["Component::Family"]) {
				if (fam["Parent"]) {
					fam.remove("Parent");
				}
				if (not fam["Children"]) {
					entity.remove("Component::Family");
				}
			}
			break;
		}
	}

	// save it back
	try {
		std::ofstream fout(filepath);
		fout << data;
		fout.close();
	}
	catch (const std::exception& e) {
		EN_CORE_ERROR("Failed to save prefab file '{0}'\n    {1}", filepath, e.what());
	}
}


void SceneSerializer::UpdateUUIDs(YAML::Node& entities, std::unordered_map<uint64_t, uint64_t>& uuid_map, UUID new_root_uuid) {
	for (auto entity : entities) {
		uint64_t old_uuid = entity["Entity"].as<uint64_t>();
		UUID new_uuid;

		auto iter = uuid_map.find(old_uuid);
		if (iter != uuid_map.end()) {
			new_uuid = UUID(iter->second);
		}
		else {
			uuid_map[old_uuid] = new_uuid;
		}

		entity["Entity"] = (uint64_t)new_uuid;


		YAML::Node family = entity["Component::Family"];
		if (family) {
			if (family["Parent"]) {
				// do not change parent of the root entity
				if (new_uuid != new_root_uuid) {
					UUID old_parent_uuid = family["Parent"].as<uint64_t>();
					UUID parent_new_uuid;
					auto iter = uuid_map.find(old_parent_uuid);
					if (iter != uuid_map.end()) {
						parent_new_uuid = UUID(iter->second);
					}
					else {
						uuid_map[old_parent_uuid] = parent_new_uuid;
					}
					family["Parent"] = (uint64_t)parent_new_uuid;
				}
			}
			if (family["Children"]) {
				for (std::size_t j = 0; j < family["Children"].size(); ++j) {
					auto child_old_uuid = family["Children"][j].as<uint64_t>();
					UUID child_new_uuid;

					auto iter = uuid_map.find(child_old_uuid);
					if (iter != uuid_map.end()) {
						child_new_uuid = UUID(iter->second);
					}
					else {
						uuid_map[child_old_uuid] = child_new_uuid;
					}

					family["Children"][j] = (uint64_t)child_new_uuid;
				}
			}
		}


		YAML::Node native_script = entity["Component::NativeScript"];
		if (native_script and native_script["ScriptFields"] and native_script["ScriptFields"].IsMap()) {
			for (auto field : native_script["ScriptFields"]) {
				YAML::Node& field_node = field.second;

				if (not field_node["Type"] or not field_node["Value"]) {
					continue;
				}

				const std::string& field_type_string = field_node["Type"].as<std::string>();
				FieldType field_type = NativeScriptField::NameType(field_type_string);

				if (field_type == FieldType::ENTITY) {
					uint64_t field_uuid = field_node["Value"].as<uint64_t>();

					auto iter = uuid_map.find(field_uuid);
					if (iter != uuid_map.end()) {
						field_node["Value"] = iter->second;
					}
				}
			}
		}

	}
}

Entity SceneSerializer::InstantiatePrefab(const std::string& filepath, UUID instance_uuid, bool no_uuid_update) {
	YAML::Node data;
	try {
		data = YAML::LoadFile(filepath);
	}
	catch (const YAML::ParserException& e) {
		EN_CORE_ERROR("Failed to load prefab file '{0}'\n	{1}", filepath, e.what());
		return {};
	}

	if (not data.IsMap() or not data["Prefab"] or not data["Root"]) {
		return {};
	}

	auto entities = data["Entities"];
	if (not entities) {
		return {};
	}

	std::unordered_map<uint64_t, uint64_t> uuid_map;

	UUID old_root_uuid = data["Root"].as<uint64_t>();
	if (instance_uuid) {
		uuid_map[old_root_uuid] = instance_uuid;
	} else {
		instance_uuid = old_root_uuid;
	}

	if (!no_uuid_update) {
		UpdateUUIDs(entities, uuid_map, instance_uuid);

	}

	// iterate reversed to not change entity indexes
	for (std::size_t i = entities.size(); i > 0; --i) {
		auto entity = entities[i - 1];
		uint64_t uuid = entity["Entity"].as<uint64_t>();
		std::string name;
		auto tag = entity["Component::Tag"];
		if (tag) {
			name = tag["Text"].as<std::string>();
		}

		// remove prefab from node, so it doesn't instantiate again forever
		if (auto pref = entity["Component::Prefab"]) {
			if (pref["RootPrefab"].as<bool>()) {
				if (uuid == instance_uuid) {
					entity.remove("Component::Prefab");
				}
			}
		}

		DeserializeEntity(entity, uuid, name);
	}

	Entity root_entity = m_Scene->FindEntityByUUID(instance_uuid);

	Component::Prefab& component = root_entity.GetOrAdd<Component::Prefab>();
	component.RootPrefab = true;
	component.PrefabPath = Project::GetRelativePath(filepath);

	// flag children as prefab iterative
	std::stack<Entity> all_entities;
	all_entities.push(root_entity);

	while (not all_entities.empty()) {
		Entity current_entity = all_entities.top();
		all_entities.pop();

		if (current_entity.HasFamily()) {
			for (Entity child : current_entity.GetChildren()) {
				child.GetOrAdd<Component::Prefab>();
				all_entities.push(child);
			}
		}
	}


	// NOTE: reason: we need to be able to get script instance
	// right after doing InstantiateScript
	m_Scene->m_Registry.view<Component::NativeScript>().each([=](auto e, auto& ns) {
		if (not ns.Instance or ns.Instance == nullptr) {
			if (ns.InstantiateScript and ns.InstantiateScript != nullptr) {
				ns.Instance = ns.InstantiateScript();
				ns.Instance->m_Entity = Entity(e, m_Scene);
				ns.ApplyNativeScriptFieldsToInstance();
			}
		}
	});

	return root_entity;
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
		out << YAML::Key << "Position" << YAML::Value << transform.LocalPosition;
		out << YAML::Key << "Rotation" << YAML::Value << transform.LocalRotation;
		out << YAML::Key << "Scale"    << YAML::Value << transform.LocalScale;

		out << YAML::EndMap;
	}

	if (entity.Has<Component::NativeScript>()) {
		out << YAML::Key << "Component::NativeScript";
		out << YAML::BeginMap;

		auto& script = entity.Get<Component::NativeScript>();
		out << YAML::Key << "ScriptName" << YAML::Value << script.ScriptName;

		SerializeNativeScriptFields(out, script);

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
		out << YAML::Key << "TextureHandle" << YAML::Value << sprite.Handle;

		if (sprite.SubTexture) {
			out << YAML::Key << "SubTexture";
			out << YAML::BeginMap;
			out << YAML::Key << "TileSize" << YAML::Value << sprite.SubTexture->TileSize;
			out << YAML::Key << "TileIndex" << YAML::Value << sprite.SubTexture->TileIndex;
			out << YAML::Key << "TileSeparation" << YAML::Value << sprite.SubTexture->TileSeparation;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}


	if (entity.Has<Component::RigidBody>()) {
		out << YAML::Key << "Component::RigidBody";
		out << YAML::BeginMap;

		auto& body = entity.Get<Component::RigidBody>();
		out << YAML::Key << "Layer" << YAML::Value << body.Layer;
		out << YAML::Key << "IsKinematic" << YAML::Value << (body.MotionType == JPH::EMotionType::Kinematic);
		out << YAML::Key << "GravityFactor" << YAML::Value << body.GetGravityFactor();
		out << YAML::Key << "Mass" << YAML::Value << body.GetMass();

		out << YAML::EndMap;
	}

	if (entity.Has<Component::CollisionBody>()) {
		out << YAML::Key << "Component::CollisionBody";
		out << YAML::BeginMap;

 		auto& body = entity.Get<Component::CollisionBody>();
		out << YAML::Key << "Layer" << YAML::Value << body.Layer;
		out << YAML::Key << "IsStatic" << YAML::Value << (body.MotionType == JPH::EMotionType::Static);
		out << YAML::Key << "IsSensor" << YAML::Value << body.IsSensor;

		out << YAML::EndMap;
	}

	if (entity.Has<Component::CollisionShape>()) {
		out << YAML::Key << "Component::CollisionShape";
		out << YAML::BeginMap;

		auto& cs = entity.Get<Component::CollisionShape>();
		out << YAML::Key << "Shape"  << YAML::Value << cs.ToString();
		out << YAML::Key << "Float"  << YAML::Value << cs.Float;
		out << YAML::Key << "Vector" << YAML::Value << cs.Vector;
		out << YAML::Key << "IsArea" << YAML::Value << cs.IsArea;

		out << YAML::EndMap;
	}

	if (entity.HasFamily()) {
		if (entity.HasParent() or entity.GetChildren().size() > 0) {
			out << YAML::Key << "Component::Family";
			out << YAML::BeginMap;

			if (entity.HasParent() and entity.GetParent().Has<Component::ID>()) {
				out << YAML::Key << "Parent" << YAML::Value << entity.GetParent().Get<Component::ID>();
			}
			if (entity.GetChildren().size() > 0) {
				out << YAML::Key << "Children";
				out << YAML::Value << YAML::BeginSeq;
				for (auto& child : entity.GetChildren()) {
					if (child and child.Has<Component::ID>()) {
						out << YAML::Value << child.Get<Component::ID>();
					}
				}
				out << YAML::EndSeq;
			}
			out << YAML::EndMap;
		}
	}

	if (entity.Has<Component::Prefab>()) {
		out << YAML::Key << "Component::Prefab";
		out << YAML::BeginMap;

		auto& component = entity.Get<Component::Prefab>();
		out << YAML::Key << "RootPrefab" << YAML::Value << component.RootPrefab;
		if (component.RootPrefab) {
			out << YAML::Key << "PrefabPath" << YAML::Value << component.PrefabPath.string();
		}

		out << YAML::EndMap;
	}

	if (entity.Has<Component::AudioSources>()) {
		out << YAML::Key << "Component::AudioSources";
		out << YAML::BeginMap;

		auto& sources = entity.Get<Component::AudioSources>();

		out << YAML::Key << "SourcePaths";
		out << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < sources.SourcePaths.size(); i++) {
			if (not sources.SourcePaths[i].empty()) {
				out << YAML::Value << sources.SourcePaths[i].string();
			}
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
	}

	if (entity.Has<Component::AnimationPlayer>()) {
		out << YAML::Key << "Component::AnimationPlayer";
		out << YAML::BeginMap;
		auto& component = entity.Get<Component::AnimationPlayer>();
		out << YAML::Key << "CurrentAnimation" << YAML::Value << component.CurrentAnimation;

		out << YAML::Key << "Animations" << YAML::Value << YAML::BeginMap;
		for (auto const& [key, val] : component.Animations) {
			out << YAML::Key << key;
			out << YAML::Value << val;
		}
		out << YAML::EndMap;

		out << YAML::EndMap;
	}

	if (entity.Has<Component::Text>()) {
		out << YAML::Key << "Component::Text";
		out << YAML::BeginMap;

		auto& text = entity.Get<Component::Text>();
		out << YAML::Key << "Font" << YAML::Value << text.Font;
		out << YAML::Key << "Color" << YAML::Value << text.Color;
		out << YAML::Key << "Scale" << YAML::Value << text.Scale;
		out << YAML::Key << "Visible" << YAML::Value << text.Visible;
		if (!text.Data.empty()) {
			out << YAML::Key << "Data" << YAML::Value << text.Data;
		}

		out << YAML::EndMap;
	}

	if (entity.Has<Component::SceneControl>()) {
		out << YAML::Key << "Component::SceneControl";
		out << YAML::BeginMap;
		out << YAML::Key << "Persistent" << YAML::Value << entity.Get<Component::SceneControl>().Persistent;
		out << YAML::EndMap;
	}

	out << YAML::EndMap;
}

void SceneSerializer::DeserializeEntity(YAML::Node& entity, uint64_t uuid, std::string& name) {
	Entity deserialized_entity;
	if (Entity found_entity = m_Scene->FindEntityByUUID(uuid)) {
		deserialized_entity = found_entity;
		deserialized_entity.Get<Component::Tag>().Text = name;
	}
	else {
		deserialized_entity = m_Scene->CreateEntityWithUUID(uuid, name);
	}

	// do not deserialize persistent entity twice
	if (deserialized_entity.Has<Component::SceneControl>()) {
		if (deserialized_entity.Get<Component::SceneControl>().Persistent) {
			return;
		}
	}


	auto prefab = entity["Component::Prefab"];
	if (prefab) {
		m_Scene->DestroyEntityImmediatelyInternal(deserialized_entity);
		if (prefab["RootPrefab"].as<bool>()) {
			if (prefab["PrefabPath"]) {
				std::filesystem::path prefab_path = prefab["PrefabPath"].as<std::string>();

				if (prefab_path.empty()) {
					EN_CORE_ERROR("DeserializeEntity PrefabPath is empty! {} {}", uuid, name);
					return;
				}
				Entity instantiated_root = m_Scene->InstantiatePrefab(prefab_path, uuid);
				if (not instantiated_root) {
					return;
				}

				auto& trans = instantiated_root.Get<Component::Transform>();

				auto transform = entity["Component::Transform"];
				if (transform) {
					trans.LocalPosition = transform["Position"].as<glm::vec3>();
					if (transform["Rotation"].size() != 4) {
						trans.LocalRotation = glm::quat(glm::vec3(0,0,transform["Rotation"].as<float>()));
					} else {
						trans.LocalRotation = transform["Rotation"].as<glm::quat>();
					}

					if (transform["Scale"].size() == 2) {
						glm::vec2 scale = transform["Scale"].as<glm::vec2>();
						trans.LocalScale.x = scale.x;
						trans.LocalScale.y = scale.y;
					} else if (transform["Scale"].size() == 3) {
						trans.LocalScale = transform["Scale"].as<glm::vec3>();
					}
				}

				auto family = entity["Component::Family"];
				if (family and family["Parent"]) {
					auto parent_id = family["Parent"].as<uint64_t>();
					Entity parent = m_Scene->FindEntityByUUID(parent_id);
					if (parent) {
						instantiated_root.Reparent(parent);
					}
				}
			}
		}
		return;
	}

	auto transform = entity["Component::Transform"];
	if (transform) {
		auto& trans = deserialized_entity.Get<Component::Transform>();
		trans.LocalPosition = transform["Position"].as<glm::vec3>();

		if (transform["Rotation"].size() != 4) {
			trans.LocalRotation = glm::quat(glm::vec3(0,0,transform["Rotation"].as<float>()));
		} else {
			trans.LocalRotation = transform["Rotation"].as<glm::quat>();
		}

		if (transform["Scale"].size() == 2) {
			glm::vec2 scale = transform["Scale"].as<glm::vec2>();
			trans.LocalScale.x = scale.x;
			trans.LocalScale.y = scale.y;
		} else if (transform["Scale"].size() == 3) {
			trans.LocalScale = transform["Scale"].as<glm::vec3>();
		}
	}

	auto spriteRenderer = entity["Component::SpriteRenderer"];
	if (spriteRenderer) {
		auto& sprite = deserialized_entity.Add<Component::SpriteRenderer>();
		sprite.Color = spriteRenderer["Color"].as<glm::vec4>();
		sprite.TileScale = spriteRenderer["TileScale"].as<float>();

		if (spriteRenderer["TextureHandle"]) {
			sprite.Handle = spriteRenderer["TextureHandle"].as<uint64_t>();
		}

		if (spriteRenderer["SubTexture"]) {
			glm::vec2 tile_size = spriteRenderer["SubTexture"]["TileSize"].as<glm::vec2>();
			glm::vec2 tile_index = spriteRenderer["SubTexture"]["TileIndex"].as<glm::vec2>();
			glm::vec2 tile_separation = spriteRenderer["SubTexture"]["TileSeparation"].as<glm::vec2>();

			sprite.SubTexture = SubTexture2D::CreateFromTileIndex(
				sprite.Handle, tile_size, tile_index, tile_separation);
		}
	}

	auto camera = entity["Component::Camera"];
	if (camera) {
		auto& cam = deserialized_entity.Add<Component::Camera>();

		cam.Cam.SetSize(camera["Camera"]["Size"].as<float>());
		cam.Cam.SetNear(camera["Camera"]["Near"].as<float>());
		cam.Cam.SetFar (camera["Camera"]["Far" ].as<float>());

		cam.Primary = camera["Primary"].as<bool>();
		cam.FixedAspectRatio = camera["FixedAspectRatio"].as<bool>();
	}

	DeserializeNativeScript(entity, deserialized_entity);

	if (auto node = entity["Component::RigidBody"]) {
		auto& body = deserialized_entity.Add<Component::RigidBody>();
		body.Layer = node["Layer"].as<int>();
		body.MotionType = (node["IsKinematic"].as<bool>() ? JPH::EMotionType::Kinematic : JPH::EMotionType::Dynamic);
		body.SetGravityFactor(node["GravityFactor"].as<float>());
		body.SetMass(node["Mass"].as<float>());
	}

	if (auto node = entity["Component::CollisionBody"]) {
		auto& body = deserialized_entity.Add<Component::CollisionBody>();
		body.Layer = node["Layer"].as<int>();
		body.MotionType = (node["IsStatic"].as<bool>() ? JPH::EMotionType::Static : JPH::EMotionType::Kinematic);
		body.IsSensor = node["IsSensor"].as<bool>();
	}

	if (auto node = entity["Component::CollisionShape"]) {
		auto& cs = deserialized_entity.Add<Component::CollisionShape>();
		cs.Shape  = Component::CollisionShape::TypeFromString(node["Shape"].as<std::string>());
		cs.Float  = node["Float"].as<float>();
		cs.Vector = node["Vector"].as<glm::vec3>();
		cs.IsArea = node["IsArea"].as<bool>();
	}


	auto family = entity["Component::Family"];
	if (family) {

		if (family["Parent"]) {
			auto parent_id = family["Parent"].as<uint64_t>();
			Entity parent = m_Scene->FindEntityByUUID(parent_id);

			if (not parent) {
				parent = m_Scene->CreateEntityWithUUID(parent_id);
			}
			deserialized_entity.Reparent(parent);
		}

		if (family["Children"]) {
			for (auto child_node : family["Children"]) {
				auto child_id = child_node.as<uint64_t>();
				Entity child = m_Scene->FindEntityByUUID(child_id);

				if (not child) {
					child = m_Scene->CreateEntityWithUUID(child_id);
				}
				if (child) {
					child.Reparent(deserialized_entity);
				}
			}
		}
	}


	auto audio_sources = entity["Component::AudioSources"];
	if (audio_sources) {
		auto& sources = deserialized_entity.Add<Component::AudioSources>();

		if (audio_sources["SourcePaths"]) {
			for (auto source_path_node : audio_sources["SourcePaths"]) {
				auto path = std::filesystem::path(source_path_node.as<std::string>());
				if (not path.empty()) {
					sources.SourcePaths.emplace_back(path);
				}
			}
		}
	}

	auto animation_player = entity["Component::AnimationPlayer"];
	if (animation_player) {
		auto& anim = deserialized_entity.Add<Component::AnimationPlayer>();

		if (animation_player["CurrentAnimation"]) {
			anim.CurrentAnimation = animation_player["CurrentAnimation"].as<uint64_t>();
		}

		if (animation_player["Animations"]) {
			auto animations_node = animation_player["Animations"];
			for (auto it = animations_node.begin(); it != animations_node.end(); ++it) {
				std::string key = it->first.as<std::string>();
				AssetHandle value = it->second.as<uint64_t>();
				anim.Animations[key] = value;
			}
		}
	}

	if (auto text_node = entity["Component::Text"]) {
		auto& text = deserialized_entity.Add<Component::Text>();
		text.Font = text_node["Font"].as<uint64_t>();
		text.Color = text_node["Color"].as<glm::vec4>();
		text.Scale = text_node["Scale"].as<float>();
		text.Visible = text_node["Visible"].as<float>();
		if (text_node["Data"]) {
			text.Data = text_node["Data"].as<std::string>();
		}
	}

	if (auto node = entity["Component::SceneControl"]) {
		auto& sc = deserialized_entity.Add<Component::SceneControl>();
		sc.Persistent = node["Persistent"].as<bool>();
	}

}

void SceneSerializer::DeserializeNativeScript(YAML::Node& node, Entity& entity) {
	auto native_script = node["Component::NativeScript"];
	if (not native_script or not native_script["ScriptName"]) {
		return;
	}

	auto script_name = native_script["ScriptName"].as<std::string>();

	for (auto& pair : ScriptRegistry::GetRegistry()) {
		if (pair.first == script_name) {
			entity.Add<Component::NativeScript>().Bind(pair.first, pair.second);
			DeserializeNativeScriptFields(native_script, entity);
			return;
		}
	}

	EN_ERROR("Couldn't find NativeScript '{0}' for entity '{1}'", script_name, entity.GetTag());
}


void WriteFieldValueToFile(YAML::Emitter& out, FieldType field_type, void* field_value) {
	switch (field_type) {
		case FieldType::NONE:
			EN_CORE_ERROR("WriteFieldValueToFile field_type is NONE !");
			break;
		case FieldType::BOOL:   out << *static_cast<bool*>       (field_value); break;
		case FieldType::INT:    out << *static_cast<int*>        (field_value); break;
		case FieldType::FLOAT:  out << *static_cast<float*>      (field_value); break;
		case FieldType::DOUBLE: out << *static_cast<double*>     (field_value); break;
		case FieldType::VEC2:   out << *static_cast<glm::vec2*>  (field_value); break;
		case FieldType::VEC3:   out << *static_cast<glm::vec3*>  (field_value); break;
		case FieldType::VEC4:   out << *static_cast<glm::vec4*>  (field_value); break;
		case FieldType::PREFAB:
		case FieldType::STRING: out << *static_cast<std::string*>(field_value); break;
		case FieldType::ENTITY: out << *static_cast<uint64_t*>   (field_value); break;
	}
}

void SceneSerializer::SerializeNativeScriptFields(YAML::Emitter& out, Component::NativeScript& script) {
	if (script.NativeScriptFields.size() == 0) {
		return;
	}

	out << YAML::Key << "ScriptFields";
	out << YAML::BeginMap;

	for (auto& pair : script.NativeScriptFields) {
		auto& field = pair.second;

		out << YAML::Key << field.Name;
		out << YAML::BeginMap;

		out << YAML::Key << "Type"  << YAML::Value << field.TypeName();
		out << YAML::Key << "Value" << YAML::Value;

		WriteFieldValueToFile(out, field.Type, field.Value);

		out << YAML::EndMap; // field.Name
	}

	out << YAML::EndMap; // ScriptFields
}






// TODO we can do better than this...
// we create new values every hot reload...


void* GetFieldValueFromNode(YAML::Node field_value, FieldType field_type) {
	switch (field_type) {
		case FieldType::NONE: {
			EN_CORE_ERROR("GetFieldValueFromNode field_type is NONE !");
			return nullptr;
		}
		case FieldType::BOOL:   return static_cast<void*>(new bool       (field_value.as<bool>()));
		case FieldType::INT:    return static_cast<void*>(new int        (field_value.as<int>()));
		case FieldType::FLOAT:  return static_cast<void*>(new float      (field_value.as<float>()));
		case FieldType::DOUBLE: return static_cast<void*>(new double     (field_value.as<double>()));
		case FieldType::VEC2:   return static_cast<void*>(new glm::vec2  (field_value.as<glm::vec2>()));
		case FieldType::VEC3:   return static_cast<void*>(new glm::vec3  (field_value.as<glm::vec3>()));
		case FieldType::VEC4:   return static_cast<void*>(new glm::vec4  (field_value.as<glm::vec4>()));
		case FieldType::PREFAB:
		case FieldType::STRING: return static_cast<void*>(new std::string(field_value.as<std::string>()));
		case FieldType::ENTITY: return static_cast<void*>(new uint64_t   (field_value.as<uint64_t>()));
	}
	return nullptr;
}



void SceneSerializer::DeserializeNativeScriptFields(YAML::Node& node, Entity& entity) {
	if (not entity.Has<Component::NativeScript>()) {
		return;
	}

	auto& script = entity.Get<Component::NativeScript>();

	EN_CORE_ASSERT(
		script.Instance == nullptr,
		"script should never have an instance before serialization !"
	);


	if (not node["ScriptFields"] or not node["ScriptFields"].IsMap()) {
		return;
	}

	for (auto field : node["ScriptFields"]) {
		std::string field_name = field.first.as<std::string>();

		// field is removed from native script, do not load
		if (script.NativeScriptFields.find(field_name) == script.NativeScriptFields.end()) {
			continue;
		}

		YAML::Node& field_node = field.second;

		if (not field_node["Type"] or not field_node["Value"]) {
			continue;
		}

		const std::string& field_type_string = field_node["Type"].as<std::string>();
		FieldType field_type = NativeScriptField::NameType(field_type_string);

		void* field_value = GetFieldValueFromNode(field_node["Value"], field_type);

		script.NativeScriptFields[field_name].Value = field_value;
	}

}

}
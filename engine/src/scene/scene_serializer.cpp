#include "scene_serializer.h"

#include <pch.h>

#include "project/project.h"
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

SceneSerializer::SceneSerializer(Scene* scene)
	: m_Scene(scene) {
	m_ErrorTexture = Texture2D::Create(FULL_PATH_EDITOR("assets/textures/error.png"), false);
}

SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	: m_Scene(scene.get()) {
	m_ErrorTexture = Texture2D::Create(FULL_PATH_EDITOR("assets/textures/error.png"), false);
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

	if (not data["Scene"]) {
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
	if (not data["Scene"]) {
		return -1;
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

				auto new_id = UUID();
				DeserializeEntity(entity, new_id, name);
				return new_id;
			}

		}
	}
	return -1;
}


void SceneSerializer::ReloadNativeScriptFields(const std::string& filepath) {
	YAML::Node data = YAML::LoadFile(filepath);
	if (not data["Scene"]) {
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

void SceneSerializer::SerializePrefab(const std::string& filepath, Entity entity_to_prefab) {
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
}


Entity SceneSerializer::DeserializePrefab(const std::string& filepath) {
	YAML::Node data;
	try {
		data = YAML::LoadFile(filepath);
	}
	catch (const YAML::ParserException& e) {
		EN_CORE_ERROR("Failed to load prefab file '{0}'\n	{1}", filepath, e.what());
		return {};
	}

	if (not data["Prefab"] or not data["Root"]) {
		return {};
	}

	auto entities = data["Entities"];
	if (not entities) {
		return {};
	}

	std::unordered_map<uint64_t, uint64_t> uuid_map;

	// update uuid's
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
				family.remove("Parent");
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
	}

	// return root entity
	auto iter = uuid_map.find(data["Root"].as<uint64_t>());
	if (iter != uuid_map.end()) {
		return m_Scene->FindEntityByUUID(UUID(iter->second));
	}
	return {};
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
		out << YAML::Key << "mag_filter_linear" << YAML::Value << sprite.mag_filter_linear;

		if (sprite.SubTexture != nullptr) {
			out << YAML::Key << "SubTexture";
			out << YAML::BeginMap;

			out << YAML::Key << "TileSize" << YAML::Value << sprite.SubTexture->GetTileSize();
			out << YAML::Key << "TileIndex" << YAML::Value << sprite.SubTexture->GetTileIndex();
			out << YAML::Key << "TileSeparation" << YAML::Value << sprite.SubTexture->GetTileSeparation();

			out << YAML::EndMap;
		}

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

	if (entity.Has<Component::RigidBody>()) {
		out << YAML::Key << "Component::RigidBody";
		out << YAML::BeginMap;

		auto& rigid_body = entity.Get<Component::RigidBody>();
		out << YAML::Key << "Mass"       << YAML::Value << rigid_body.Mass;
		out << YAML::Key << "UseGravity" << YAML::Value << rigid_body.UseGravity;

		out << YAML::EndMap;
	}

	if (entity.Has<Component::Collider>()) {
		out << YAML::Key << "Component::Collider";
		out << YAML::BeginMap;

		auto& collider = entity.Get<Component::Collider>();
		out << YAML::Key << "Shape"  << YAML::Value << collider.Shape;
		out << YAML::Key << "Float"  << YAML::Value << collider.Float;
		out << YAML::Key << "Vector" << YAML::Value << collider.Vector;
		out << YAML::Key << "IsArea" << YAML::Value << collider.IsArea;

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


	auto transform = entity["Component::Transform"];
	if (transform) {
		auto& trans = deserialized_entity.Get<Component::Transform>();
		trans.Position = transform["Position"].as<glm::vec3>();
		trans.Rotation = transform["Rotation"].as<float>();
		trans.Scale = transform["Scale"].as<glm::vec2>();
	}

	auto spriteRenderer = entity["Component::SpriteRenderer"];
	if (spriteRenderer) {
		auto& sprite = deserialized_entity.Add<Component::SpriteRenderer>();
		sprite.Color = spriteRenderer["Color"].as<glm::vec4>();
		sprite.TileScale = spriteRenderer["TileScale"].as<float>();
		if (spriteRenderer["mag_filter_linear"]) {
			sprite.mag_filter_linear = spriteRenderer["mag_filter_linear"].as<bool>();
		}

		auto path = std::filesystem::path(spriteRenderer["TexturePath"].as<std::string>());
		if (not path.empty()) {
			sprite.TexturePath = path;
		}

		if (not sprite.TexturePath.empty()) {
			auto path = Project::GetAbsolutePath(sprite.TexturePath);
			if (std::filesystem::exists(path)) {
				sprite.Texture = Texture2D::Create(path.string(), sprite.mag_filter_linear);
			}
			else {
				sprite.Texture = m_ErrorTexture;
			}
		}

		if (spriteRenderer["SubTexture"]) {
			glm::vec2 tile_size = spriteRenderer["SubTexture"]["TileSize"].as<glm::vec2>();
			glm::vec2 tile_index = spriteRenderer["SubTexture"]["TileIndex"].as<glm::vec2>();
			glm::vec2 tile_separation = spriteRenderer["SubTexture"]["TileSeparation"].as<glm::vec2>();

			sprite.SubTexture = SubTexture2D::CreateFromTileIndex(
				sprite.Texture, tile_size, tile_index, tile_separation);
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

	auto rigid_body = entity["Component::RigidBody"];
	if (rigid_body) {
		auto& body = deserialized_entity.Add<Component::RigidBody>();
		body.Mass = rigid_body["Mass"].as<float>();
		body.UseGravity = rigid_body["UseGravity"].as<bool>();
	}

	auto collider = entity["Component::Collider"];
	if (collider) {
		auto& col = deserialized_entity.Add<Component::Collider>();
		col.Shape  = (Component::ColliderShape)collider["Shape"].as<int>();
		col.Float  = collider["Float"].as<float>();
		col.Vector = collider["Vector"].as<glm::vec3>();
		col.IsArea = collider["IsArea"].as<bool>();
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
				child.Reparent(deserialized_entity);
			}
		}
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

		script.NativeScriptFields[field_name] = { field_name, field_type, field_value };
	}

}

}
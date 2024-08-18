#include "components.h"

#include "scene/scriptable_entity.h"
#include "audio/audio.h"
#include "project/project.h"

namespace Enik {

// NativeScript
void copy_saved_value_to_instance(NativeScriptField& instance_field, NativeScriptField& saved_field) {
	switch (instance_field.Type) {
		case FieldType::NONE: {
			EN_CORE_ERROR("copy_saved_value_to_instance instance_field.Type is NONE !");
			return;
		}
		case FieldType::BOOL: {
			bool* instance_value = static_cast<bool*>(instance_field.Value);
			bool*    saved_value = static_cast<bool*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
		case FieldType::INT: {
			int* instance_value = static_cast<int*>(instance_field.Value);
			int*    saved_value = static_cast<int*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
		case FieldType::FLOAT: {
			float* instance_value = static_cast<float*>(instance_field.Value);
			float*    saved_value = static_cast<float*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
		case FieldType::DOUBLE: {
			double* instance_value = static_cast<double*>(instance_field.Value);
			double*    saved_value = static_cast<double*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
		case FieldType::VEC2: {
			glm::vec2* instance_value = static_cast<glm::vec2*>(instance_field.Value);
			glm::vec2*    saved_value = static_cast<glm::vec2*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
		case FieldType::VEC3: {
			glm::vec3* instance_value = static_cast<glm::vec3*>(instance_field.Value);
			glm::vec3*    saved_value = static_cast<glm::vec3*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
		case FieldType::VEC4: {
			glm::vec4* instance_value = static_cast<glm::vec4*>(instance_field.Value);
			glm::vec4*    saved_value = static_cast<glm::vec4*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
		case FieldType::PREFAB:
		case FieldType::STRING: {
			std::string* instance_value = static_cast<std::string*>(instance_field.Value);
			std::string*    saved_value = static_cast<std::string*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
		case FieldType::ENTITY: {
			uint64_t* instance_value = static_cast<uint64_t*>(instance_field.Value);
			uint64_t*    saved_value = static_cast<uint64_t*>(saved_field.Value);
			*instance_value = *saved_value;
			delete saved_value;
			return;
		}
	}
}

void Component::NativeScript::ApplyNativeScriptFieldsToInstance() {
	EN_CORE_ASSERT(Instance, "ApplyNativeScriptFieldsToInstance Instance is null !");

	for (auto& instance_field : Instance->OnEditorGetFields()) {
		if (NativeScriptFields.find(instance_field.Name) != NativeScriptFields.end()) {
			auto& saved_field = NativeScriptFields[instance_field.Name];
			if (saved_field.Value != nullptr) {
				copy_saved_value_to_instance(instance_field, saved_field);
			}
			saved_field.Value = instance_field.Value;
		}
	}
}

void* create_new_value_for_field(FieldType field_type, void* field_default) {
	switch (field_type) {
		case FieldType::NONE: {
			EN_CORE_ERROR("create_new_value_for_field field_type is NONE !");
			return nullptr;
		}
		case FieldType::BOOL:   return static_cast<void*>(new bool       (*(bool*       )field_default));
		case FieldType::INT:    return static_cast<void*>(new int        (*(int*        )field_default));
		case FieldType::FLOAT:  return static_cast<void*>(new float      (*(float*      )field_default));
		case FieldType::DOUBLE: return static_cast<void*>(new double     (*(double*     )field_default));
		case FieldType::VEC2:   return static_cast<void*>(new glm::vec2  (*(glm::vec2*  )field_default));
		case FieldType::VEC3:   return static_cast<void*>(new glm::vec3  (*(glm::vec3*  )field_default));
		case FieldType::VEC4:   return static_cast<void*>(new glm::vec4  (*(glm::vec4*  )field_default));
		case FieldType::PREFAB:
		case FieldType::STRING: return static_cast<void*>(new std::string(*(std::string*)field_default));
		case FieldType::ENTITY: return static_cast<void*>(new uint64_t   (*(uint64_t*   )field_default));
	}
	return nullptr;
}
void delete_field_value(FieldType field_type, void* field_value) {
	switch (field_type) {
		case FieldType::NONE: {
			EN_CORE_ERROR("delete_field_value field_type is NONE !");
			return;
		}
		case FieldType::BOOL:   delete static_cast<bool*>       (field_value); return;
		case FieldType::INT:    delete static_cast<int*>        (field_value); return;
		case FieldType::FLOAT:  delete static_cast<float*>      (field_value); return;
		case FieldType::DOUBLE: delete static_cast<double*>     (field_value); return;
		case FieldType::VEC2:   delete static_cast<glm::vec2*>  (field_value); return;
		case FieldType::VEC3:   delete static_cast<glm::vec3*>  (field_value); return;
		case FieldType::VEC4:   delete static_cast<glm::vec4*>  (field_value); return;
		case FieldType::PREFAB:
		case FieldType::STRING: delete static_cast<std::string*>(field_value); return;
		case FieldType::ENTITY: delete static_cast<uint64_t*>   (field_value); return;
	}
}


void Component::NativeScript::Bind(const std::string& script_name, const std::function<ScriptableEntity*()>& inst) {
	ScriptName = script_name;

	InstantiateScript = inst;

	DestroyScript = [](NativeScript* ns) {

		// delete fields
		for (auto& val : ns->NativeScriptFields) {
			auto& field = val.second;
			if (field.Value == nullptr) {
				delete_field_value(field.Type, field.Value);
			}
		}

		delete ns->Instance;
		ns->Instance = nullptr;
	};





	// retrieve fields of the script from a temporary instance
	ScriptableEntity* temp_instance = InstantiateScript();
	for (auto field : temp_instance->OnEditorGetFields()) {
		NativeScriptFields[field.Name] = NativeScriptField(
			field.Name,
			field.Type,
			create_new_value_for_field(field.Type, field.Value)
		);
	}
	delete temp_instance;
}




bool Component::Family::HasParent() {
	return (m_ParentUUID != 0);
}

Entity Component::Family::GetParent() {
	return FindEntityByUUID(m_ParentUUID);
}

void Component::Family::Reparent(Entity this_entity, Entity new_parent) {

	if (new_parent and HasEntityAsChild(new_parent)) {
		// can not make it new parent if it is our child
		return;
	}

	if (HasParent()) {
		GetParent().GetOrAdd<Component::Family>().RemoveChild(this_entity);
	}


	if (new_parent) {
		// add child to new parent
		new_parent.GetOrAdd<Component::Family>().AddChild(this_entity);
	}

	SetParent(new_parent);
}


void Component::Family::SetChildrenGlobalTransformRecursive(Component::Transform& transform) {
	for (Entity& child : Children) {
		Component::Transform& child_transform = child.Get<Component::Transform>();

		glm::mat4 child_global_transform = transform.GetTransform() * child_transform.GetTransform();

		glm::vec3 skew;
		glm::vec4 perspective;
		glm::quat rotation;
		glm::decompose(child_global_transform, child_transform.GlobalScale, rotation, child_transform.GlobalPosition, skew, perspective);

		child_transform.GlobalRotation = glm::eulerAngles(rotation).z;
		child.Get<Component::Family>().SetChildrenGlobalTransformRecursive(child_transform);
	}
}


void Component::Family::AddChild(Entity entity) {
	// check if it already is child
	for (Entity& child : Children) {
		if (child == entity) {
			return;
		}
	}

	Children.emplace_back(entity);
}

void Component::Family::RemoveChild(Entity entity) {
	for (size_t i = 0; i < Children.size(); ++i) {
		if (Children[i] == entity) {
			Children.erase(Children.begin() + i);
			return;
		}
	}
}

void Component::Family::SetParent(Entity& entity) {
	if (entity) {
		m_ParentUUID = entity.GetID();
	} else {
		m_ParentUUID = 0;
	}
}

bool Component::Family::HasEntityAsChild(Entity entity) {
    for (Entity& child : Children) {
		if (child == entity) {
			return true;
		}

		if (child.Has<Component::Family>()) {
			if (child.Get<Component::Family>().HasEntityAsChild(entity)) {
				return true;
			}
		}
	}
	return false;
}


void Component::AudioSources::Play(const std::string& name) {
	for (size_t i = 0; i < SourcePaths.size(); i++) {
		if (SourcePaths[i].stem() == name) {
			Audio::Play(Project::GetAbsolutePath(SourcePaths[i]));
		}
	}
}

}
#include "components.h"

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
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
		case FieldType::INT: {
			int* instance_value = static_cast<int*>(instance_field.Value);
			int*    saved_value = static_cast<int*>(saved_field.Value);
			*instance_value = *saved_value;
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
		case FieldType::FLOAT: {
			// *static_cast<float*>(instance_field.Value) = *static_cast<float*>(saved_field.Value);
			float* instance_value = static_cast<float*>(instance_field.Value);
			float*    saved_value = static_cast<float*>(saved_field.Value);
			*instance_value = *saved_value;
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
		case FieldType::DOUBLE: {
			double* instance_value = static_cast<double*>(instance_field.Value);
			double*    saved_value = static_cast<double*>(saved_field.Value);
			*instance_value = *saved_value;
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
		case FieldType::VEC2: {
			glm::vec2* instance_value = static_cast<glm::vec2*>(instance_field.Value);
			glm::vec2*    saved_value = static_cast<glm::vec2*>(saved_field.Value);
			*instance_value = *saved_value;
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
		case FieldType::VEC3: {
			glm::vec3* instance_value = static_cast<glm::vec3*>(instance_field.Value);
			glm::vec3*    saved_value = static_cast<glm::vec3*>(saved_field.Value);
			*instance_value = *saved_value;
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
		case FieldType::VEC4: {
			glm::vec4* instance_value = static_cast<glm::vec4*>(instance_field.Value);
			glm::vec4*    saved_value = static_cast<glm::vec4*>(saved_field.Value);
			*instance_value = *saved_value;
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
		case FieldType::STRING: {
			std::string* instance_value = static_cast<std::string*>(instance_field.Value);
			std::string*    saved_value = static_cast<std::string*>(saved_field.Value);
			*instance_value = *saved_value;
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
		case FieldType::ENTITY: {
			uint64_t* instance_value = static_cast<uint64_t*>(instance_field.Value);
			uint64_t*    saved_value = static_cast<uint64_t*>(saved_field.Value);
			*instance_value = *saved_value;
			if (saved_value != nullptr) {
				delete saved_value;
			}
			return;
		}
	}
}

void Component::NativeScript::ApplyNativeScriptFieldsToInstance() {
	EN_CORE_ASSERT(Instance, "ApplyNativeScriptFieldsToInstance Instance is null !");

	for (auto& instance_field : Instance->OnEditorGetFields()) {
		if (NativeScriptFields.find(instance_field.Name) != NativeScriptFields.end()) {
			auto& saved_field = NativeScriptFields[instance_field.Name];

			copy_saved_value_to_instance(instance_field, saved_field);
			saved_field.Value = instance_field.Value;
		}
	}
}

}
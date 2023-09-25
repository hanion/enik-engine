#pragma once
#include <string>
#include <base.h>


enum class FieldType {
	NONE,
	BOOL, INT, FLOAT, DOUBLE,
	VEC2, VEC3, VEC4,
	STRING,
	ENTITY
};

struct NativeScriptField {
	std::string Name;
	FieldType Type;
	void* Value;

	NativeScriptField()
		: Type(FieldType::NONE), Value(nullptr) {}

	NativeScriptField(const std::string& name, FieldType type, void* value)
		: Name(name), Type(type), Value(value) {}


	const std::string TypeName() {
		switch (Type) {
			case FieldType::NONE:   return std::string();
			case FieldType::BOOL:   return "bool";
			case FieldType::INT:    return "int";
			case FieldType::FLOAT:  return "float";
			case FieldType::DOUBLE: return "double";
			case FieldType::VEC2:   return "vec2";
			case FieldType::VEC3:   return "vec3";
			case FieldType::VEC4:   return "vec4";
			case FieldType::STRING: return "string";
			case FieldType::ENTITY: return "entity";
		}
		return std::string();
	}

	static const FieldType NameType(const std::string& name) {
		if      (name.empty()    ) { return FieldType::NONE;   }
		else if (name == "bool"  ) { return FieldType::BOOL;   }
		else if (name == "int"   ) { return FieldType::INT;    }
		else if (name == "float" ) { return FieldType::FLOAT;  }
		else if (name == "double") { return FieldType::DOUBLE; }
		else if (name == "vec2"  ) { return FieldType::VEC2;   }
		else if (name == "vec3"  ) { return FieldType::VEC3;   }
		else if (name == "vec4"  ) { return FieldType::VEC4;   }
		else if (name == "string") { return FieldType::STRING; }
		else if (name == "entity") { return FieldType::ENTITY; }

		EN_CORE_ERROR("Native Script Field Name is invalid! {}", name);
		return FieldType::NONE;
	}
};
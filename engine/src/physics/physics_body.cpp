#include "physics_body.h"
#include "Jolt/Physics/Body/MotionType.h"

namespace Enik {

const char* MotionTypeToString(JPH::EMotionType t) {
	switch (t) {
		case JPH::EMotionType::Static:    return "Static";
		case JPH::EMotionType::Dynamic:   return "Dynamic";
		case JPH::EMotionType::Kinematic: return "Kinematic";
	}
	return "";
}

JPH::EMotionType MotionTypeFromString(const std::string& str) {
	if (str == "Static")    { return JPH::EMotionType::Static; }
	if (str == "Dynamic")   { return JPH::EMotionType::Dynamic; }
	if (str == "Kinematic") { return JPH::EMotionType::Kinematic; }
	return JPH::EMotionType::Static;
}


}

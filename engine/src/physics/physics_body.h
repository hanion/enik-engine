#pragma once

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"


namespace Enik {

using PhysicsBody = JPH::Body;

const char* MotionTypeToString(JPH::EMotionType t);
JPH::EMotionType MotionTypeFromString(const std::string& str);


}

#include "components.h"
#include "asset/asset_manager.h"
#include "base.h"
#include "gtc/quaternion.hpp"
#include "scene/scriptable_entity.h"
#include "audio/audio.h"
#include "project/project.h"
#include "script_system/script_system.h"

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
	return (Parent != nullptr);
}

Entity Component::Family::GetParent() {
	if (HasParent()) {
		return *Parent;
	}
	return {};
}

void Component::Family::Reparent(Entity this_entity, Entity new_parent) {

	if (new_parent and HasEntityAsChild(new_parent)) {
		// can not make it new parent if it is our child
		return;
	}

	if (HasParent()) {
		GetParent().GetOrAddFamily().RemoveChild(this_entity);
	}


	if (new_parent) {
		// add child to new parent
		new_parent.GetOrAddFamily().AddChild(this_entity);
	}

	SetParent(new_parent);
}

void Component::Family::SetChildrenGlobalTransformRecursive(const Component::Transform& transform) {
	for (Entity& child : Children) {
		Component::Transform& child_transform = child.Get<Component::Transform>();

		glm::mat4 child_global_transform = transform.GetTransform() * child_transform.GetTransform();

		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(child_global_transform,
			child_transform.GlobalScale,
			child_transform.GlobalRotation,
			child_transform.GlobalPosition,
			skew, perspective
		);


		child.Get<Component::Family>().SetChildrenGlobalTransformRecursive(child_transform);
	}
}


void Component::Family::AddChild(const Entity& entity) {
	// check if it already is child
	for (Entity& child : Children) {
		if (child == entity) {
			return;
		}
	}

	Children.emplace_back(entity);
}

void Component::Family::RemoveChild(const Entity& entity) {
	for (size_t i = 0; i < Children.size(); ++i) {
		if (Children[i] == entity) {
			Children.erase(Children.begin() + i);
			return;
		}
	}
}

void Component::Family::SetParent(const Entity& entity) {
	Parent.reset();

	if (entity) {
		Parent = CreateRef<Entity>(entity);
	}
}

bool Component::Family::HasEntityAsChild(const Entity& entity) {
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

void Component::Family::SetGlobalPositionRotation(Component::Transform& tr, const glm::vec3& global_pos, const glm::quat& global_rot) {
	if (HasParent()) {
		const auto& parent_transform = GetParent().Get<Component::Transform>();
		tr.LocalPosition = glm::inverse(parent_transform.GlobalRotation) * (global_pos- parent_transform.GlobalPosition);
		tr.LocalRotation = glm::inverse(parent_transform.GlobalRotation) * global_rot;
	} else {
		tr.LocalPosition = global_pos;
		tr.LocalRotation = global_rot;
	}
}

void Component::Family::SetGlobalPosition(Component::Transform& tr, const glm::vec3& global) {
	if (HasParent()) {
		const auto& parent_transform = GetParent().Get<Component::Transform>();
		tr.LocalPosition = glm::inverse(parent_transform.GlobalRotation) * (global- parent_transform.GlobalPosition);
	} else {
		tr.LocalPosition = global;
	}
}
void Component::Family::SetGlobalRotation(Component::Transform& tr, const glm::quat& global) {
	if (HasParent()) {
		const auto& parent_transform = GetParent().Get<Component::Transform>();
		tr.LocalRotation = glm::inverse(parent_transform.GlobalRotation) * global;
	} else {
		tr.LocalRotation = global;
	}
}


void Component::AudioSources::Play(const std::string& name) {
	for (size_t i = 0; i < SourcePaths.size(); i++) {
		if (SourcePaths[i].stem().string() == name) {
			Audio::Play(Project::GetAbsolutePath(SourcePaths[i]));
		}
	}
}

void Component::AudioSources::Play(int index) {
	if (index < 0 or index >= (int)SourcePaths.size()) {
		EN_CORE_ERROR("Component::AudioSources::Play invalid index: {}", index);
		return;
	}

	Audio::Play(Project::GetAbsolutePath(SourcePaths[index]));
}



void Component::AnimationPlayer::Start(const std::string& name) {
	if (Animations.find(name) != Animations.end()) {
		CurrentAnimation = Animations[name];
		CurrentTime = 0.0f;
		Paused = false;
	}
}

void Component::AnimationPlayer::End() {
	if (!CurrentAnimation || !BoundEntity || !AssetManager::IsAssetHandleValid(CurrentAnimation)) {
		return;
	}

	Ref<Animation> anim = AssetManager::GetAsset<Animation>(CurrentAnimation);
	if (!anim) {
		return;
	}

	CurrentTime = anim->Duration;
	anim->Update((*BoundEntity.get()), CurrentTime);
	Paused = true;
	CurrentTime = 0.0f;

	if (OnEndCallback) {
		OnEndCallback(anim->Name);
		OnEndCallback = nullptr;
	}
}

void Component::AnimationPlayer::Kill() {
	Paused = true;
	CurrentTime = 0.0f;
}


void Component::AnimationPlayer::Update(const Timestep& dt) {
	if (Paused || !CurrentAnimation || !BoundEntity || !AssetManager::IsAssetHandleValid(CurrentAnimation)) {
		return;
	}

	Ref<Animation> anim = AssetManager::GetAsset<Animation>(CurrentAnimation);
	if (!anim) {
		return;
	}

	CurrentTime += dt;

	anim->Update((*BoundEntity.get()), CurrentTime);

	if (CurrentTime >= anim->Duration) {
		if (anim->Looping) {
			CurrentTime = 0.0f;
		} else {
			Paused = true;
			if (OnEndCallback) {
				OnEndCallback(anim->Name);
				OnEndCallback = nullptr;
			}
		}
	}
}

namespace Component {


glm::vec3 PhysicsBodyBase::GetLinearVelocity() const {
	EN_CORE_ASSERT(MotionType != JPH::EMotionType::Static,
		"PhysicsBodyBase::GetLinearVelocity can only be used on non-static bodies.");

	if (body) {
		auto v = body->GetLinearVelocity();
		return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
	}
	return glm::vec3(0.0f);
}

void PhysicsBodyBase::SetLinearVelocity(const glm::vec3& velocity) {
	EN_CORE_ASSERT(MotionType != JPH::EMotionType::Static,
		"PhysicsBodyBase::SetLinearVelocity can only be used on non-static bodies.");

	if (body) {
		body->SetLinearVelocityClamped({velocity.x, velocity.y, velocity.z});
	}
}

glm::quat PhysicsBodyBase::GetRotation() const {
	if (body) {
		auto rotation = body->GetRotation();
		return {rotation.GetW(), rotation.GetX(), rotation.GetY(), rotation.GetZ()};
	}
	return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

void PhysicsBodyBase::SetAngularVelocity(const glm::vec3& angular_velocity) {
	EN_CORE_ASSERT(MotionType != JPH::EMotionType::Static,
		"PhysicsBodyBase::SetAngularVelocity can only be used on non-static bodies.");

	if (body) {
		body->SetAngularVelocity({angular_velocity.x, angular_velocity.y, angular_velocity.z});
	}
}

glm::vec3 PhysicsBodyBase::GetAngularVelocity() const {
	EN_CORE_ASSERT(MotionType != JPH::EMotionType::Static,
		"PhysicsBodyBase::GetAngularVelocity can only be used on non-static bodies.");

	if (body) {
		auto angular_velocity = body->GetAngularVelocity();
		return {angular_velocity.GetX(), angular_velocity.GetY(), angular_velocity.GetZ()};
	}
	return glm::vec3(0.0f);
}

void PhysicsBodyBase::SetPosition(const glm::vec3& position) {
	if (body) {
		GetBodyInterface().SetPosition(body->GetID(), {position.x, position.y, position.z}, JPH::EActivation::Activate);
	}
}

JPH::BodyInterface& PhysicsBodyBase::GetBodyInterface() const {
	return ScriptSystem::GetPhysicsContext().GetPhysicsSystem()->GetBodyInterface();
}

void RigidBody::AddForce(const glm::vec3& force) {
	EN_CORE_ASSERT(MotionType == JPH::EMotionType::Dynamic,
		"PhysicsBodyBase::AddForce can only be used on Dynamic bodies.");

	if (body) {
		GetBodyInterface().AddForce(body->GetID(), {force.x, force.y, force.z});
	}
}
void RigidBody::AddImpulse(const glm::vec3& impulse) {
	EN_CORE_ASSERT(MotionType == JPH::EMotionType::Dynamic,
		"PhysicsBodyBase::AddImpulse can only be used on Dynamic bodies.");

	if (body) {
		GetBodyInterface().AddImpulse(body->GetID(), {impulse.x, impulse.y, impulse.z});
	}
}

void RigidBody::AddTorque(const glm::vec3& torque) {
	if (body) {
		GetBodyInterface().AddTorque(body->GetID(), {torque.x, torque.y, torque.z});
	}
}

void RigidBody::SetMass(float mass) {
	if (mass <= 0.0f) {
		return;
	}
	Mass = mass;
	if (body) {
		body->GetMotionProperties()->ScaleToMass(Mass);
	}
}

void RigidBody::SetGravityFactor(float gravity_factor) {
	GravityFactor = gravity_factor;
	if (body) {
		body->GetMotionProperties()->SetGravityFactor(GravityFactor);
	}
}



}
}
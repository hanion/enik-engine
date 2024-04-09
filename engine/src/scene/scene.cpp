#include "scene.h"

#include <glm/glm.hpp>

#include "renderer/renderer2D.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "script_system/script_system.h"
#include "physics/physics_world.h"
#include "scene/scene_serializer.h"

namespace Enik {

Scene::Scene() {
	ScriptSystem::SetSceneContext(this);
	PhysicsWorld::InitPhysicsWorld(&m_Registry);
}

Scene::~Scene() {
	DestroyScriptableEntities();
}

Entity Scene::CreateEntity(const std::string& name) {
	Entity entity = Entity(m_Registry.create(), this);
	entity.Add<Component::ID>();
	entity.Add<Component::Transform>();
	auto& tag = entity.Add<Component::Tag>();
	tag.Text = name.empty() ? "Empty Entity" : name;
	return entity;
}

Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name) {
	Entity entity = Entity(m_Registry.create(), this);
	entity.Add<Component::ID>(uuid);
	entity.Add<Component::Transform>();
	auto& tag = entity.Add<Component::Tag>();
	tag.Text = name.empty() ? "Empty Entity" : name;
	return entity;
}

// recursive
void Scene::DestroyEntity(Entity entity) {
	if (entity.HasFamily()) {
		for (auto& child : entity.GetChildren()) {
			DestroyEntity(child);
		}

		// remove this entity child from it's parent
		entity.Reparent({});
	}

	if (m_Registry.valid(entity)) {
		m_Registry.destroy(entity);
	}
}

Entity Scene::InstantiatePrefab(const std::filesystem::path& path) {
	std::filesystem::path canonical_path = Project::GetAbsolutePath(path);
	SceneSerializer serializer = SceneSerializer(this);
	return serializer.DeserializePrefab(canonical_path.string());
}

void Scene::OnUpdateEditor(Timestep ts, OrthographicCameraController& camera) {
	EN_PROFILE_SECTION("Scene::OnUpdateEditor");

	SetGlobalPositions();

	Renderer2D::BeginScene(camera.GetCamera());

	/* Get Sprites */ {
		EN_PROFILE_SECTION("Get Sprites");

		auto group = m_Registry.group<Component::SpriteRenderer>(entt::get<Component::Transform>);
		for (auto entity : group) {
			Component::Transform& transform   = group.get<Component::Transform>     (entity);
			Component::SpriteRenderer& sprite = group.get<Component::SpriteRenderer>(entity);

			Renderer2D::DrawQuad(transform, sprite, (int32_t)entity);
		}
	}

	Renderer2D::EndScene();
}

void Scene::OnUpdateRuntime(Timestep ts) {
	EN_PROFILE_SECTION("Scene::OnUpdateRuntime");


	SetGlobalPositions();

	/* Update Scripts */
	if (not m_IsPaused or m_StepFrames-- > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (not ns.Instance or ns.Instance == nullptr) {
				if (ns.InstantiateScript and ns.InstantiateScript != nullptr) {
					ns.Instance = ns.InstantiateScript();
					ns.Instance->m_Entity = Entity(entity, this);
					ns.ApplyNativeScriptFieldsToInstance();
					ns.Instance->OnCreate();
				}
			}
			else {
				ns.Instance->OnUpdate(ts);
			}
		});
	}



	// Rendering

	auto primary_camera = GetPrimaryCameraEntity();
	if (not primary_camera) {
		return;
	}

	Renderer2D::BeginScene(
		primary_camera.Get<Component::Camera>().Cam,
		primary_camera.Get<Component::Transform>().GetTransform()
	);

	/* Get Sprites */ {
		EN_PROFILE_SECTION("Get Sprites");

		auto group = m_Registry.group<Component::SpriteRenderer>(entt::get<Component::Transform>);
		for (auto entity : group) {
			Component::Transform& transform   = group.get<Component::Transform>     (entity);
			Component::SpriteRenderer& sprite = group.get<Component::SpriteRenderer>(entity);

			Renderer2D::DrawQuad(transform, sprite, (int32_t)entity);
		}
	}

	Renderer2D::EndScene();
}

void Scene::OnFixedUpdate() {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (not ns.Instance or ns.Instance == nullptr) {
				if (ns.InstantiateScript and ns.InstantiateScript != nullptr) {
					ns.Instance = ns.InstantiateScript();
					ns.Instance->m_Entity = Entity(entity, this);
					ns.ApplyNativeScriptFieldsToInstance();
					ns.Instance->OnCreate();
				}
			}
			else {
				ns.Instance->OnFixedUpdate();
			}
		});

		PhysicsWorld::Step();
	}
}


void Scene::OnKeyPressed(const KeyPressedEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnKeyPressed(event);
			}
		});
	}
}

void Scene::OnKeyReleased(const KeyReleasedEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnKeyReleased(event);
			}
		});
	}
}

void Scene::OnMouseButtonPressed(const MouseButtonPressedEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnMouseButtonPressed(event);
			}
		});
	}
}

void Scene::OnMouseButtonReleased(const MouseButtonReleasedEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnMouseButtonReleased(event);
			}
		});
	}
}

void Scene::OnMouseScrolled(const MouseScrolledEvent& event) {
	if (not m_IsPaused or m_StepFrames > 0) {
		m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
			if (ns.Instance and ns.Instance != nullptr) {
				ns.Instance->OnMouseScrolled(event);
			}
		});
	}
}

void Scene::OnViewportResize(uint32_t width, uint32_t height) {
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	auto view = m_Registry.view<Component::Camera>();
	for (auto entity : view) {
		Component::Camera& camera = view.get<Component::Camera>(entity);
		if (not camera.FixedAspectRatio) {
			camera.Cam.SetViewportSize(width, height);
		}
	}
}

void Scene::OnViewportResize(glm::vec2 position, uint32_t width, uint32_t height) {
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	auto view = m_Registry.view<Component::Camera>();
	for (auto entity : view) {
		Component::Camera& camera = view.get<Component::Camera>(entity);
		if (not camera.FixedAspectRatio) {
			camera.Cam.SetViewportSize(position, width, height);
		}
	}
}

void Scene::DestroyScriptableEntities() {
	m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
		if (ns.Instance) {
			ns.Instance->OnDestroy();
			ns.DestroyScript(&ns);
		}
	});
}

void Scene::ClearNativeScripts() {
	m_Registry.view<Component::NativeScript>().each([=](auto entity, auto& ns) {
		ns.InstantiateScript = nullptr;
		ns.DestroyScript(&ns);
	});
}

Entity Scene::GetPrimaryCameraEntity() {
	EN_PROFILE_SECTION("GetPrimaryCameraEntity");

	auto view = m_Registry.view<Component::Transform, Component::Camera>();
	for (auto entity : view) {
		Component::Camera& camera = view.get<Component::Camera>(entity);
		if (camera.Primary) {
			return Entity(entity, this);
		}
	}

	return Entity();
}

Entity Scene::FindEntityByUUID(UUID uuid) {
	auto view = m_Registry.view<Component::ID>();
	for (auto entity_handle : view) {
		const Component::ID& id = view.get<Component::ID>(entity_handle);
		if (id.uuid == uuid) {
			return Entity(entity_handle, this);
		}
	}
	return Entity();
}

Entity Scene::FindEntityByName(const std::string& name) {
	auto view = m_Registry.view<Component::Tag>();
	for (auto entity_handle : view) {
		const Component::Tag& tag = view.get<Component::Tag>(entity_handle);
		if (tag.Text == name) {
			return Entity(entity_handle, this);
		}
	}
	return Entity();
}

void Scene::SetGlobalPositions() {
	EN_PROFILE_SCOPE;

	// reset all global positions
	m_Registry.view<Component::Transform>().each(
		[=](auto entity, auto& transform) {
			transform.GlobalPosition = transform.Position;
		}
	);

	// find global positions
	auto group = m_Registry.group<Component::Family>(entt::get<Component::Transform>);
	for (auto entity : group) {
		Component::Transform& transform = group.get<Component::Transform>(entity);
		Component::Family&    family    = group.get<Component::Family>   (entity);
		transform.GlobalPosition = family.FindGlobalPosition(transform);
	}
}

}